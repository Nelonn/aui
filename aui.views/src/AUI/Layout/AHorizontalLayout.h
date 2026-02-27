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

#include "ALinearLayout.h"
#include <AUI/Util/ALayoutDirection.h>
#include <AUI/Util/Declarative/Containers.h>


/**
 * @brief Places views in a row.
 * @ingroup layout-managers
 * @details
 *
 * |             |                               |
 * |-------------|-------------------------------|
 * | Declarative notation | ` Horizontal { }` |
 *
 * Horizontal layout manager places views in a row, fitting their height to the container's height.
 * Has a `Horizontal` shortcut that allows easy usage in UI building:
 * ```cpp
 * setContents(
 *   Horizontal {
 *     _new<AButton>("1"),
 *     _new<AButton>("2"),
 *     _new<AButton>("3"),
 *   }
 * );
 * ```
 *
 * <img src="https://github.com/aui-framework/aui/raw/master/docs/imgs/Screenshot_20210714_035347.png">
 * <img src="https://github.com/aui-framework/aui/raw/master/docs/imgs/horizontal.jpg">
 */
class API_AUI_VIEWS AHorizontalLayout : public ALinearLayout<> {
private:
    int mSpacing = 0;

public:
    // for template<> logic
    static constexpr ALayoutDirection DIRECTION = ALayoutDirection::HORIZONTAL;

    AHorizontalLayout() {
    }

    AHorizontalLayout(int spacing) : mSpacing(spacing) {
    }

    void performLayout(int x, int y, int width, int height) override;
    void measure(glm::ivec2 availableSize) override;
    glm::ivec2 getMinimumSize() override;

    void setSpacing(int spacing) override;

    [[nodiscard]]
    int getSpacing() const { return mSpacing; }

    ALayoutDirection getLayoutDirection() override;
};

namespace aui::declarative {
/**
 * @brief Places views in a row.
 * <p>
 *  <img width="960" src="https://github.com/aui-framework/aui/raw/master/docs/imgs/horizontal.jpg">
 *
 *  <dl>
 *    <dt><b>View:</b> AViewContainer</dt>
 *    <dt><b>Layout manager:</b> AHorizontalLayout</dt>
 *  </dl>
 * </p>
 */
using Horizontal = aui::ui_building::view_container_layout<AHorizontalLayout>;
}
