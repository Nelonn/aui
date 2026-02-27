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
#include <AUI/Reflect/AEnumerate.h>

/**
 * @brief Direction of the flex box.
 * @ingroup views
 * @see [AFlexLayout]
 */
enum class AFlexDirection {
    /**
    * @brief Items are placed in a row.
    */
    ROW,

    /**
    * @brief Items are placed in a row, but in reverse order.
    */
    ROW_REVERSE,

    /**
    * @brief Items are placed in a column.
    */
    COLUMN,

    /**
    * @brief Items are placed in a column, but in reverse order.
    */
    COLUMN_REVERSE,
};

AUI_ENUM_VALUES(AFlexDirection,
                AFlexDirection::ROW,
                AFlexDirection::ROW_REVERSE,
                AFlexDirection::COLUMN,
                AFlexDirection::COLUMN_REVERSE)

/**
 * @brief Wrap mode of the flex box.
 * @ingroup views
 * @see [AFlexLayout]
 */
enum class AFlexWrap {
    /**
    * @brief Items are placed in a single line.
    */
    NOWRAP,

    /**
    * @brief Items wrap to the next line.
    */
    WRAP,

    /**
    * @brief Items wrap to the next line, but in reverse order.
    */
    WRAP_REVERSE,
};

AUI_ENUM_VALUES(AFlexWrap,
                AFlexWrap::NOWRAP,
                AFlexWrap::WRAP,
                AFlexWrap::WRAP_REVERSE)

/**
 * @brief Justification of the flex box items along the main axis.
 * @ingroup views
 * @see [AFlexLayout]
 */
enum class AFlexJustifyContent {
    /**
    * @brief Items are packed toward the start of the line.
    */
    FLEX_START,

    /**
    * @brief Items are packed toward the end of the line.
    */
    FLEX_END,

    /**
    * @brief Items are centered along the line.
    */
    CENTER,

    /**
    * @brief Items are evenly distributed in the line; first item is on the start, last item on the end.
    */
    SPACE_BETWEEN,

    /**
    * @brief Items are evenly distributed in the line with equal space around them.
    */
    SPACE_AROUND,

    /**
    * @brief Items are evenly distributed in the line with equal space between them and the edges.
    */
    SPACE_EVENLY,
};

AUI_ENUM_VALUES(AFlexJustifyContent,
                AFlexJustifyContent::FLEX_START,
                AFlexJustifyContent::FLEX_END,
                AFlexJustifyContent::CENTER,
                AFlexJustifyContent::SPACE_BETWEEN,
                AFlexJustifyContent::SPACE_AROUND,
                AFlexJustifyContent::SPACE_EVENLY)

/**
 * @brief Alignment of the flex box items along the cross axis.
 * @ingroup views
 * @see [AFlexLayout]
 */
enum class AFlexAlignItems {
    /**
    * @brief Item inherits its parent's align-items value.
    */
    AUTO,

    /**
    * @brief Items are stretched to fill the container.
    */
    STRETCH,

    /**
    * @brief Items are aligned to the start of the cross axis.
    */
    FLEX_START,

    /**
    * @brief Items are aligned to the end of the cross axis.
    */
    FLEX_END,

    /**
    * @brief Items are centered along the cross axis.
    */
    CENTER,

    /**
    * @brief Items are aligned such as their baselines align.
    */
    BASELINE,
};

AUI_ENUM_VALUES(AFlexAlignItems,
                AFlexAlignItems::AUTO,
                AFlexAlignItems::STRETCH,
                AFlexAlignItems::FLEX_START,
                AFlexAlignItems::FLEX_END,
                AFlexAlignItems::CENTER,
                AFlexAlignItems::BASELINE)

/**
 * @brief Alignment of the flex box lines along the cross axis when there is extra space.
 * @ingroup views
 * @see [AFlexLayout]
 */
enum class AFlexAlignContent {
    /**
    * @brief Lines are stretched to take up the remaining space.
    */
    STRETCH,

    /**
    * @brief Lines are packed toward the start of the cross axis.
    */
    FLEX_START,

    /**
    * @brief Lines are packed toward the end of the cross axis.
    */
    FLEX_END,

    /**
    * @brief Lines are centered along the cross axis.
    */
    CENTER,

    /**
    * @brief Lines are evenly distributed; first line is on the start, last line on the end.
    */
    SPACE_BETWEEN,

    /**
    * @brief Lines are evenly distributed with equal space around them.
    */
    SPACE_AROUND,

    SPACE_EVENLY,
};

AUI_ENUM_VALUES(AFlexAlignContent,
                AFlexAlignContent::STRETCH,
                AFlexAlignContent::FLEX_START,
                AFlexAlignContent::FLEX_END,
                AFlexAlignContent::CENTER,
                AFlexAlignContent::SPACE_BETWEEN,
                AFlexAlignContent::SPACE_AROUND)
