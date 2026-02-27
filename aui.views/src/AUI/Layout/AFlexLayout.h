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

#pragma once

#include <AUI/Layout/ALinearLayout.h>
#include <AUI/Util/AFlex.h>
#include <AUI/Util/Declarative/Containers.h>

/**
 * @brief Flexbox layout manager.
 * @ingroup layout-managers
 * @details
 * Implements W3C Flexbox layout algorithm.
 */
class API_AUI_VIEWS AFlexLayout : public ALinearLayout<> {
public:
    AFlexLayout() = default;

    void performLayout(int x, int y, int width, int height) override;
    void measure(glm::ivec2 availableSize) override;
    glm::ivec2 getMinimumSize() override;

    ALayoutDirection getLayoutDirection() override;

    void setFlexDirection(AFlexDirection flexDirection) {
        mFlexDirection = flexDirection;
    }

    void setFlexWrap(AFlexWrap flexWrap) {
        mFlexWrap = flexWrap;
    }

    void setJustifyContent(AFlexJustifyContent justifyContent) {
        mJustifyContent = justifyContent;
    }

    void setAlignItems(AFlexAlignItems alignItems) {
        mAlignItems = alignItems;
    }

    void setAlignContent(AFlexAlignContent alignContent) {
        mAlignContent = alignContent;
    }

    void setSpacing(int spacing) override {
        mSpacing = spacing;
    }

    [[nodiscard]]
    AFlexDirection getFlexDirection() const {
        return mFlexDirection;
    }

    [[nodiscard]]
    AFlexWrap getFlexWrap() const {
        return mFlexWrap;
    }

    [[nodiscard]]
    AFlexJustifyContent getJustifyContent() const {
        return mJustifyContent;
    }

    [[nodiscard]]
    AFlexAlignItems getAlignItems() const {
        return mAlignItems;
    }

    [[nodiscard]]
    AFlexAlignContent getAlignContent() const {
        return mAlignContent;
    }

    [[nodiscard]]
    int getSpacing() const {
        return mSpacing;
    }

private:
    AFlexDirection mFlexDirection = AFlexDirection::ROW;
    AFlexWrap mFlexWrap = AFlexWrap::NOWRAP;
    AFlexJustifyContent mJustifyContent = AFlexJustifyContent::FLEX_START;
    AFlexAlignItems mAlignItems = AFlexAlignItems::STRETCH;
    AFlexAlignContent mAlignContent = AFlexAlignContent::STRETCH;
    int mSpacing = 0;
};

namespace aui::declarative {
/**
 * @brief Places views in a flex box.
 * <p>
 *  <dl>
 *    <dt><b>View:</b> AViewContainer</dt>
 *    <dt><b>Layout manager:</b> AFlexLayout</dt>
 *  </dl>
 * </p>
 */
struct Flex {
    AFlexDirection flexDirection = AFlexDirection::ROW;
    AFlexWrap flexWrap = AFlexWrap::NOWRAP;
    AFlexJustifyContent justifyContent = AFlexJustifyContent::FLEX_START;
    AFlexAlignItems alignItems = AFlexAlignItems::STRETCH;
    AFlexAlignContent alignContent = AFlexAlignContent::STRETCH;
    int spacing = 0;

    AVector<_<AView>> children;

    _<AView> operator()() {
        auto flex = std::make_unique<AFlexLayout>();
        flex->setFlexDirection(flexDirection);
        flex->setFlexWrap(flexWrap);
        flex->setJustifyContent(justifyContent);
        flex->setAlignItems(alignItems);
        flex->setAlignContent(alignContent);
        flex->setSpacing(spacing);

        auto c = _new<AViewContainer>();
        c->setLayout(std::move(flex));
        c->setViews(std::move(children));
        return c;
    }
};
}
