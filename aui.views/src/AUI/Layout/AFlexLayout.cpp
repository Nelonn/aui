/*
 * AUI Framework - Declarative UI toolkit for modern C++20
 * Copyright (C) 2020-2025 Alex2772 and Contributors
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "AFlexLayout.h"
#include <AUI/Core.h>
#include <AUI/View/AView.h>
#include <AUI/View/AViewContainer.h>
#include <AUI/Common/ABoxFields.h>
#include <AUI/Enum/Visibility.h>
#include <AUI/Util/ALayoutDirection.h>
#include <numeric>
#include <algorithm>
#include <limits>

namespace {

constexpr int kMaxSize = std::numeric_limits<int>::max() / 2;

struct FlexItem {
    _<AView>         view;
    ABoxFields       margin;
    AFlexAlignItems  alignSelf;
    int              order        = 0;

    // flex algorithm state
    int              mainBasis    = 0;
    int              mainMinSize  = 0;
    int              mainMaxSize  = kMaxSize;
    int              crossMinSize = 0;
    int              crossMaxSize = kMaxSize;
    int              crossContentSize = 0; // actual content cross-size (for non-stretch)
    float            grow         = 0.f;
    float            shrink       = 1.f;

    // resolved during layout
    int              mainSize     = 0;
    int              crossSize    = 0;
    bool             frozen       = false;
};

struct FlexLine {
    AVector<FlexItem> items;
    int mainSize  = 0;
    int crossSize = 0;
};

} // namespace

static int clampSize(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

// ---------------------------------------------------------------------------
// §9 — Main layout algorithm
// ---------------------------------------------------------------------------
void AFlexLayout::performLayout(int x, int y, int width, int height) {
    if (mViews.empty()) return;

    const bool is_row      = mFlexDirection == AFlexDirection::ROW
                          || mFlexDirection == AFlexDirection::ROW_REVERSE;
    const bool is_main_rev = mFlexDirection == AFlexDirection::ROW_REVERSE
                          || mFlexDirection == AFlexDirection::COLUMN_REVERSE;
    const bool is_wrap_rev = mFlexWrap == AFlexWrap::WRAP_REVERSE;

    auto getMain  = [&](glm::ivec2 v) { return is_row ? v.x : v.y; };
    auto getCross = [&](glm::ivec2 v) { return is_row ? v.y : v.x; };
    auto makeVec  = [&](int main, int cross) -> glm::ivec2 {
        return is_row ? glm::ivec2(main, cross) : glm::ivec2(cross, main);
    };

    const int container_main  = is_row ? width  : height;
    const int container_cross = is_row ? height : width;

    // -----------------------------------------------------------------------
    // §9.1  Generate flex items
    // -----------------------------------------------------------------------
    AVector<FlexItem> all_items;
    all_items.reserve(mViews.size());

    for (auto& view : mViews) {
        if (!(view->getVisibility() & Visibility::FLAG_CONSUME_SPACE)) continue;

        view->ensureAssUpdated();

        FlexItem item;
        item.view   = view;
        item.grow   = view->getFlexGrow();
        item.shrink = view->getFlexShrink();
        item.order  = view->getOrder();
        item.margin = view->getMargin();

        item.alignSelf = view->getAlignSelf();
        if (item.alignSelf == AFlexAlignItems::AUTO) {
            item.alignSelf = mAlignItems;
        }

        const auto min_size = view->getMeasuredSize();
        const auto max_size = view->getMaxSize();

        item.mainMinSize  = getMain(min_size);
        item.mainMaxSize  = getMain(max_size) > 0 ? getMain(max_size) : kMaxSize;
        item.crossMinSize = getCross(min_size);
        item.crossMaxSize = getCross(max_size) > 0 ? getCross(max_size) : kMaxSize;

        // §9.2  Flex base size
        if (auto basis = view->getFlexBasis()) {
            item.mainBasis = static_cast<int>(basis->getValuePx());
        } else {
            item.mainBasis = getMain(min_size);
        }
        item.mainBasis = clampSize(item.mainBasis, item.mainMinSize, item.mainMaxSize);

        // Cross content size — used for non-STRETCH items
        item.crossContentSize = getCross(min_size);

        item.mainSize  = item.mainBasis;
        item.crossSize = item.crossContentSize;

        all_items.push_back(std::move(item));
    }

    // §9.1  Sort by `order`
    std::stable_sort(all_items.begin(), all_items.end(),
        [](const FlexItem& a, const FlexItem& b) { return a.order < b.order; });

    // -----------------------------------------------------------------------
    // §9.3  Line breaking
    // -----------------------------------------------------------------------
    AVector<FlexLine> lines;

    if (mFlexWrap == AFlexWrap::NOWRAP) {
        FlexLine line;
        line.items = std::move(all_items);
        lines.push_back(std::move(line));
    } else {
        FlexLine current_line;
        int      current_main = 0;

        for (auto& item : all_items) {
            const int item_outer = item.mainBasis + getMain(item.margin.occupiedSize());

            if (!current_line.items.empty()) {
                const int hypothetical = current_main + mSpacing + item_outer;
                if (hypothetical > container_main) {
                    lines.push_back(std::move(current_line));
                    current_line = FlexLine{};
                    current_main = 0;
                }
            }

            if (!current_line.items.empty()) current_main += mSpacing;
            current_main += item_outer;
            current_line.items.push_back(std::move(item));
        }
        if (!current_line.items.empty())
            lines.push_back(std::move(current_line));
    }

    // -----------------------------------------------------------------------
    // §9.7  Resolve flexible lengths  (per line)
    // -----------------------------------------------------------------------
    for (auto& line : lines) {
        const int n         = static_cast<int>(line.items.size());
        const int gap_total = mSpacing * std::max(0, n - 1);

        for (auto& item : line.items) {
            item.mainSize = item.mainBasis;
            item.frozen   = false;
        }

        // §9.7 step 3: freeze inflexible items
        // items with grow=0 when growing, or shrink=0 when shrinking
        {
            int used = gap_total;
            for (auto& item : line.items)
                used += item.mainBasis + getMain(item.margin.occupiedSize());
            const bool growing = (container_main - used) > 0;

            for (auto& item : line.items) {
                if (growing && item.grow == 0.f) {
                    item.mainSize = clampSize(item.mainBasis, item.mainMinSize, item.mainMaxSize);
                    item.frozen   = true;
                } else if (!growing && item.shrink == 0.f) {
                    item.mainSize = clampSize(item.mainBasis, item.mainMinSize, item.mainMaxSize);
                    item.frozen   = true;
                }
            }
        }

        // initial_free_space based on clamped basis
        const int initial_free_space = [&]() {
            int used = gap_total;
            for (auto& item : line.items)
                used += item.mainBasis + getMain(item.margin.occupiedSize());
            return container_main - used;
        }();
        const bool growing = initial_free_space > 0;

        // §9.7 main loop
        for (;;) {
            bool has_unfrozen = false;
            for (auto& item : line.items)
                if (!item.frozen) { has_unfrozen = true; break; }
            if (!has_unfrozen) break;

            float total_grow      = 0.f;
            float total_shrink_sb = 0.f;
            int   frozen_used     = gap_total;
            int   unfrozen_outer  = 0;

            for (auto& item : line.items) {
                const int outer = item.mainSize + getMain(item.margin.occupiedSize());
                if (item.frozen) {
                    frozen_used += outer;
                } else {
                    unfrozen_outer += item.mainBasis + getMain(item.margin.occupiedSize());
                    total_grow     += item.grow;
                    total_shrink_sb += item.shrink * static_cast<float>(item.mainBasis);
                }
            }

            const int remaining = container_main - frozen_used - unfrozen_outer;

            if (growing  && total_grow      <= 0.f) break;
            if (!growing && total_shrink_sb <= 0.f) break;

            bool any_clamped = false;
            for (auto& item : line.items) {
                if (item.frozen) continue;

                int new_size;
                if (growing) {
                    const float ratio = item.grow / total_grow;
                    new_size = item.mainBasis + static_cast<int>(static_cast<float>(remaining) * ratio);
                } else {
                    const float ratio = (item.shrink * static_cast<float>(item.mainBasis)) / total_shrink_sb;
                    new_size = item.mainBasis + static_cast<int>(static_cast<float>(remaining) * ratio);
                }

                const int clamped = clampSize(new_size, item.mainMinSize, item.mainMaxSize);
                item.mainSize = clamped;

                if (clamped != new_size) {
                    item.frozen = true;
                    any_clamped = true;
                }
            }

            if (!any_clamped) {
                // No violations — freeze all unfrozen and exit
                for (auto& item : line.items) item.frozen = true;
                break;
            }
        }

        // -----------------------------------------------------------------------
        // §9.4  Cross size of each item (tentative — stretch finalized later)
        // -----------------------------------------------------------------------
        for (auto& item : line.items) {
            if (item.alignSelf == AFlexAlignItems::STRETCH) {
                item.crossSize = item.crossMinSize; // will be updated after line cross size is known
            } else {
                item.crossSize = clampSize(item.crossContentSize, item.crossMinSize, item.crossMaxSize);
            }
        }

        // §9.4 step 8: line sizes
        line.mainSize  = gap_total;
        line.crossSize = 0;
        for (auto& item : line.items) {
            line.mainSize  += item.mainSize  + getMain(item.margin.occupiedSize());
            line.crossSize  = std::max(line.crossSize,
                item.crossSize + getCross(item.margin.occupiedSize()));
        }
    }

    // -----------------------------------------------------------------------
    // §9.6  Align content — stretch lines
    // -----------------------------------------------------------------------
    {
        int total_lines_cross = 0;
        for (auto& line : lines) {
            total_lines_cross += line.crossSize;
        }
        const int n_lines     = static_cast<int>(lines.size());
        const int cross_space = container_cross - total_lines_cross
                              - mSpacing * std::max(0, n_lines - 1);

        if (mAlignContent == AFlexAlignContent::STRETCH && cross_space > 0 && n_lines > 0) {
            const int per_line = cross_space / n_lines;
            int       remainder = cross_space - per_line * n_lines;
            for (auto& line : lines) {
                line.crossSize += per_line + (remainder > 0 ? 1 : 0);
                if (remainder > 0) --remainder;
            }
        }
    }

    // -----------------------------------------------------------------------
    // §9.5 + §9.6  Positioning
    // -----------------------------------------------------------------------
    if (is_wrap_rev) {
        std::reverse(lines.begin(), lines.end());
    }

    const int n_lines          = static_cast<int>(lines.size());
    int       total_lines_cross = 0;
    for (auto& line : lines) total_lines_cross += line.crossSize;

    const int cross_space = container_cross - total_lines_cross
                          - mSpacing * std::max(0, n_lines - 1);

    int line_cross_start = 0;
    int line_cross_gap   = mSpacing;

    switch (mAlignContent) {
        case AFlexAlignContent::FLEX_START:
        case AFlexAlignContent::STRETCH:
            line_cross_start = 0;
            break;
        case AFlexAlignContent::FLEX_END:
            line_cross_start = cross_space;
            break;
        case AFlexAlignContent::CENTER:
            line_cross_start = cross_space / 2;
            break;
        case AFlexAlignContent::SPACE_BETWEEN:
            line_cross_start = 0;
            if (n_lines > 1)
                line_cross_gap += cross_space / (n_lines - 1);
            break;
        case AFlexAlignContent::SPACE_AROUND:
            if (n_lines > 0) {
                const int gap    = cross_space / n_lines;
                line_cross_start = gap / 2;
                line_cross_gap  += gap;
            }
            break;
        case AFlexAlignContent::SPACE_EVENLY:
            if (n_lines > 0) {
                const int gap    = cross_space / (n_lines + 1);
                line_cross_start = gap;
                line_cross_gap  += gap;
            }
            break;
    }

    int cross_cursor = line_cross_start;

    for (auto& line : lines) {
        const int n_items   = static_cast<int>(line.items.size());
        const int free_main = container_main - line.mainSize;

        // -----------------------------------------------------------------------
        // §9.5  justify-content
        // -----------------------------------------------------------------------
        int main_start = 0;
        int item_gap   = mSpacing;

        switch (mJustifyContent) {
            case AFlexJustifyContent::FLEX_START:
                main_start = 0;
                break;
            case AFlexJustifyContent::FLEX_END:
                main_start = free_main;
                break;
            case AFlexJustifyContent::CENTER:
                main_start = free_main / 2;
                break;
            case AFlexJustifyContent::SPACE_BETWEEN:
                main_start = 0;
                if (n_items > 1)
                    item_gap += free_main / (n_items - 1);
                break;
            case AFlexJustifyContent::SPACE_AROUND:
                if (n_items > 0) {
                    const int gap = free_main / n_items;
                    main_start    = gap / 2;
                    item_gap     += gap;
                }
                break;
            case AFlexJustifyContent::SPACE_EVENLY:
                if (n_items > 0) {
                    const int gap = free_main / (n_items + 1);
                    main_start    = gap;
                    item_gap     += gap;
                }
                break;
        }

        int main_cursor = main_start;

        for (auto& item : line.items) {
            const int margin_main_start = getMain(item.margin.leftTop());
            const int margin_main_end   = getMain(item.margin.rightBottom());
            const int margin_cross      = getCross(item.margin.occupiedSize());

            // §9.4 step 11: finalize STRETCH cross size
            int item_cross;
            if (item.alignSelf == AFlexAlignItems::STRETCH) {
                item_cross = clampSize(line.crossSize - margin_cross,
                    item.crossMinSize, item.crossMaxSize);
            } else {
                item_cross = item.crossSize;
            }

            // §9.6  align-self (cross axis offset inside the line)
            int item_cross_offset;
            switch (item.alignSelf) {
                case AFlexAlignItems::FLEX_END:
                    item_cross_offset = line.crossSize - margin_cross - item_cross
                                      + getCross(item.margin.leftTop());
                    break;
                case AFlexAlignItems::CENTER:
                    item_cross_offset = (line.crossSize - margin_cross - item_cross) / 2
                                      + getCross(item.margin.leftTop());
                    break;
                case AFlexAlignItems::FLEX_START:
                case AFlexAlignItems::STRETCH:
                case AFlexAlignItems::BASELINE: // approximate with flex-start
                default:
                    item_cross_offset = getCross(item.margin.leftTop());
                    break;
            }

            // §9.5  Main-axis position
            int main_pos;
            if (!is_main_rev) {
                main_pos = main_cursor + margin_main_start;
            } else {
                const int item_outer = item.mainSize + margin_main_start + margin_main_end;
                main_pos = container_main - main_cursor - item_outer + margin_main_end;
            }

            const glm::ivec2 pos  = glm::ivec2(x, y) + makeVec(main_pos, cross_cursor + item_cross_offset);
            const glm::ivec2 size = makeVec(item.mainSize, item_cross);
            item.view->setGeometry(pos, size);

            main_cursor += item.mainSize + margin_main_start + margin_main_end + item_gap;
        }

        cross_cursor += line.crossSize + line_cross_gap;
    }
}

void AFlexLayout::measure(glm::ivec2 availableSize) {
    const bool is_row = mFlexDirection == AFlexDirection::ROW
                      || mFlexDirection == AFlexDirection::ROW_REVERSE;
    const int container_main  = is_row ? availableSize.x : availableSize.y;
    const int container_cross = is_row ? availableSize.y : availableSize.x;

    for (auto& view : mViews) {
        if (!(view->getVisibility() & Visibility::FLAG_CONSUME_SPACE)) continue;

        if (mFlexWrap == AFlexWrap::NOWRAP) {
            if (is_row) {
                view->measure({ 0, container_cross - view->getMargin().vertical() });
            } else {
                view->measure({ container_cross - view->getMargin().horizontal(), 0 });
            }
        } else {
            if (is_row) {
                view->measure({ container_main - view->getMargin().horizontal(), 0 });
            } else {
                view->measure({ 0, container_main - view->getMargin().vertical() });
            }
        }
    }
}

glm::ivec2 AFlexLayout::getMinimumSize() {
    const bool is_row  = mFlexDirection == AFlexDirection::ROW
                      || mFlexDirection == AFlexDirection::ROW_REVERSE;

    int min_main  = 0;
    int n         = 0;
    int max_item_cross = 0;

    for (auto& v : mViews) {
        if (!(v->getVisibility() & Visibility::FLAG_CONSUME_SPACE)) continue;
        // Use measured size for better performance
        const auto minContentSize = v->getMeasuredSize();
        const int item_main  = (is_row ? minContentSize.x : minContentSize.y)
                             + (is_row ? v->getMargin().horizontal() : v->getMargin().vertical());
        const int item_cross = (is_row ? minContentSize.y : minContentSize.x)
                             + (is_row ? v->getMargin().vertical()   : v->getMargin().horizontal());

        if (mFlexWrap != AFlexWrap::NOWRAP) {
            min_main = std::max(min_main, item_main);
        } else {
            min_main += item_main;
        }
        max_item_cross = std::max(max_item_cross, item_cross);
        ++n;
    }
    if (n > 1 && mFlexWrap == AFlexWrap::NOWRAP) {
        min_main += mSpacing * (n - 1);
    }

    if (is_row) return { min_main, max_item_cross };
    return { max_item_cross, min_main };
}

ALayoutDirection AFlexLayout::getLayoutDirection() {
    return (mFlexDirection == AFlexDirection::ROW || mFlexDirection == AFlexDirection::ROW_REVERSE)
        ? ALayoutDirection::HORIZONTAL
        : ALayoutDirection::VERTICAL;
}
