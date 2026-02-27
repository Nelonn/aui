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
#include <AUI/Util/Declarative/Containers.h>


/**
 * @brief Places views in a stack (along z axis).
 * @ingroup layout-managers
 * @details
 *
 * |             |                               |
 * |-------------|-------------------------------|
 * | Declarative notation | `Stacked { }`<br/> `Centered { }` |
 *
 * Stacked layout manager places views stacking them onto each other. If the [view](aview.md) is expanding, then it
 * uses all space available. Otherwise, the [view](aview.md) is centered.
 *
 * AStackedLayout has two declarative notations - `Stacked` and `Centered`. They are equivalent, so you can express
 * your intentions more clearly by specifying `Stacked` to stack views and `Centered` to center them.
 *
 * First view appears below the second one, and so on. The last view is above all other views within this Stacked
 * layout.
 *
 * <img src="https://github.com/aui-framework/aui/raw/master/docs/imgs/stacked2.jpg">
 */
class API_AUI_VIEWS AStackedLayout : public ALinearLayout<> {
public:
    AStackedLayout() {
    }

    virtual ~AStackedLayout() = default;

    void performLayout(int x, int y, int width, int height) override;

    void measure(glm::ivec2 availableSize) override;

    glm::ivec2 getMinimumSize() override;
};

namespace aui::declarative {
/**
 * @brief Places views in a stack, centering them.
 * <p>
 *  <img width="960" src="https://github.com/aui-framework/aui/raw/master/docs/imgs/stacked2.jpg">
 *
 *  <dl>
 *    <dt><b>View:</b> AViewContainer</dt>
 *    <dt><b>Layout manager:</b> AStackedLayout</dt>
 *  </dl>
 * </p>
 */
using Stacked = aui::ui_building::view_container_layout<AStackedLayout>;

/**
 * <p>
 * <code>Center</code> is an alias to Stacked. When Stacked is used only for centering views, you can use
 * this alias in order to improve understanding of your code.
 * </p>
 */
using Centered = Stacked;
}
