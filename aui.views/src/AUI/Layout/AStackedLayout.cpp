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

#include "AStackedLayout.h"


void AStackedLayout::performLayout(int x, int y, int width, int height) {
    for (auto& v: mViews) {
        v->ensureAssUpdated();
        int finalX, finalY, finalWidth, finalHeight;
        auto margins = v->getMargin();
        // Use measured size instead of getMinimumSize() for better performance
        const auto& measuredSize = v->getMeasuredSize();
        if (v->getExpandingHorizontal() == 0) {
            finalWidth = glm::min(measuredSize.x + margins.horizontal(), width);
            finalX = (width - finalWidth) / 2;
        } else {
            finalX = 0;
            finalWidth = width;
        }
        if (v->getExpandingVertical() == 0) {
            finalHeight = glm::min(measuredSize.y + margins.vertical(), height);
            finalY = (height - finalHeight) / 2;
        } else {
            finalY = 0;
            finalHeight = height;
        }
        v->setGeometry(finalX + x + margins.left,
                       finalY + y + margins.top,
                       finalWidth - margins.horizontal(),
                       finalHeight - margins.vertical());
        // if view rejected the specified size, then we would recenter it
        if (v->getSize() != glm::ivec2(finalWidth - margins.horizontal(), finalHeight - margins.vertical())) {
            glm::ivec2 correctedSize = v->getSize() + margins.horizontal();
            glm::ivec2 correctedPos = (glm::ivec2(width, height) - correctedSize) / 2;
            v->setPosition(correctedPos + glm::ivec2(x + margins.left, y + margins.top));
        }
    }
}

void AStackedLayout::measure(glm::ivec2 availableSize) {
    // Measure all children with the same available size (minus margins)
    for (auto& v : mViews) {
        if (!(v->getVisibility() & Visibility::FLAG_CONSUME_SPACE))
            continue;
        v->measure({
            availableSize.x > 0 ? availableSize.x - v->getMargin().horizontal() : 0,
            availableSize.y > 0 ? availableSize.y - v->getMargin().vertical() : 0
        });
    }
}

glm::ivec2 AStackedLayout::getMinimumSize() {
    int minW = 0;
    int minH = 0;
    for (auto& v: mViews) {
        if (!!(v->getVisibility() & Visibility::FLAG_CONSUME_SPACE)) {
            // Use measured size if available for better performance
            const auto s = v->getMeasuredSize();
            minW = glm::max(s.x + v->getMargin().horizontal(), minW);
            minH = glm::max(s.y + v->getMargin().vertical(), minH);
        }
    }
    return { minW, minH };
}
