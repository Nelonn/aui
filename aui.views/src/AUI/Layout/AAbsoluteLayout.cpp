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

#include "AAbsoluteLayout.h"

void AAbsoluteLayout::performLayout(int x, int y, int width, int height) {
    for (const auto& i : mViews) {
        if (i.pivotX && i.pivotY) {
            i.view->setGeometry(
                i.pivotX->getValuePx(), i.pivotY->getValuePx(),
                i.sizeX ? i.sizeX->getValuePx() : i.view->getMinimumSize().x,
                i.sizeY ? i.sizeY->getValuePx() : i.view->getMinimumSize().y);
            continue;
        }
        i.view->setGeometry(i.view->getPosition(), i.view->getSize());
    }
}

glm::ivec2 AAbsoluteLayout::getMinimumSize() {
    glm::ivec2 v = { 0, 0 };
    for (const auto& i : mViews) {
        int x = i.pivotX.valueOr(0).getValuePx();
        if (i.sizeX) {
            x += i.sizeX->getValuePx();
        }
        int y = i.pivotY.valueOr(0).getValuePx();
        if (i.sizeY) {
            y += i.sizeY->getValuePx();
        }
        v = glm::max(v, { x, y });
    }
    return v;
}

void AAbsoluteLayout::add(aui::detail::AbsoluteLayoutCell cell) {
    mViews << std::move(cell);
}

void AAbsoluteLayout::addView(const _<AView>& view, AOptional<size_t> index) {
    addViewBasicImpl({
        .view = view
    }, index);
}
