/*
 * AUI Framework - Declarative UI toolkit for modern C++20
 * Copyright (C) 2020-2026 Alex2772 and Contributors
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/// [AFlexLayout_example]
#include <AUI/Platform/Entry.h>
#include <AUI/Platform/AWindow.h>
#include <AUI/Util/UIBuildingHelpers.h>
#include <AUI/View/AForEachUI.h>
#include <AUI/Layout/AFlexLayout.h>
#include <AUI/View/AButton.h>
#include <AUI/View/AScrollArea.h>
#include <AUI/View/AGroupBox.h>

using namespace ass;
using namespace aui::declarative;

AUI_ENTRY {
    auto window = _new<AWindow>("AUI Flex", 960_dp, 780_dp);
    window->setContents(Vertical {
        AScrollArea::Builder().withContents(
            Vertical {
                GroupBox {
                    Label { "1. Row Wrap Reverse + Justify Center" },
                    Flex {
                        .flexDirection = AFlexDirection::ROW,
                        .flexWrap = AFlexWrap::WRAP_REVERSE,
                        .justifyContent = AFlexJustifyContent::CENTER,
                        .alignItems = AFlexAlignItems::CENTER,
                        .children = {
                            Button { .content = Label{ "Item 1" } } AUI_OVERRIDE_STYLE { MinSize { 90_dp, 30_dp }, FlexGrow { 1.f } },
                            Button { .content = Label{ "Item 2" } } AUI_OVERRIDE_STYLE { MinSize { 90_dp, 30_dp }, FlexGrow { 1.f } },
                            Button { .content = Label{ "Item 3" } } AUI_OVERRIDE_STYLE { MinSize { 90_dp, 30_dp }, FlexGrow { 1.f } },
                            Button { .content = Label{ "Item 4" } } AUI_OVERRIDE_STYLE { MinSize { 90_dp, 30_dp }, FlexGrow { 1.f } },
                            Button { .content = Label{ "Item 5" } } AUI_OVERRIDE_STYLE { MinSize { 90_dp, 30_dp }, FlexGrow { 1.f } },
                        }
                    } AUI_OVERRIDE_STYLE {
                        MinSize { {}, 100_dp },
                        LayoutSpacing { 4_dp },
                        Expanding { 1, 0 }
                    },
                },

                GroupBox {
                    Label { "2. Complex Shrinking & Basis" },
                    Vertical {
                        Label { "Container width: 200dp. Items: 100dp basis each." },
                        Flex {
                            .flexDirection = AFlexDirection::ROW,
                            .flexWrap = AFlexWrap::NOWRAP,
                            .children = {
                                _new<ALabel>("Shrink 1") AUI_OVERRIDE_STYLE { FlexBasis { 100_dp }, FlexShrink { 1.f }, BackgroundSolid { AColor::RED }, Margin { 2_dp } },
                                _new<ALabel>("Shrink 3") AUI_OVERRIDE_STYLE { FlexBasis { 100_dp }, FlexShrink { 3.f }, BackgroundSolid { AColor::GREEN }, Margin { 2_dp } },
                                _new<ALabel>("Shrink 0 (Fixed)") AUI_OVERRIDE_STYLE { FlexBasis { 100_dp }, FlexShrink { 0.f }, BackgroundSolid { AColor::BLUE }, Margin { 2_dp } },
                            }
                        } AUI_OVERRIDE_STYLE {
                            FixedSize { 200_dp, {} },
                            MinSize { {}, 40_dp }
                        },
                    }
                },

                GroupBox {
                    Label { "3. Nested Flex + Align Items/Self" },
                    Flex {
                        .flexDirection = AFlexDirection::ROW,
                        .alignItems = AFlexAlignItems::STRETCH,
                        .children = {
                            Flex {
                                .flexDirection = AFlexDirection::COLUMN,
                                .alignItems = AFlexAlignItems::STRETCH,
                                .children = {
                                    _new<AButton>("Inner 1") AUI_OVERRIDE_STYLE { AlignSelf { AFlexAlignItems::FLEX_START } },
                                    _new<AButton>("Inner 2") AUI_OVERRIDE_STYLE { AlignSelf { AFlexAlignItems::CENTER } },
                                    _new<AButton>("Inner 3") AUI_OVERRIDE_STYLE { AlignSelf { AFlexAlignItems::FLEX_END } },
                                }
                            } AUI_OVERRIDE_STYLE {
                                MinSize { 150_dp, 100_dp },
                                BackgroundSolid { AColor::GRAY }
                            },
                            _new<AButton>("Outer Item") AUI_OVERRIDE_STYLE { AlignSelf { AFlexAlignItems::CENTER } },
                        }
                    } AUI_OVERRIDE_STYLE {
                        Expanding { 1, 0 },
                        MinSize { {}, 120_dp },
                        LayoutSpacing { 10_dp }
                    },
                },

                GroupBox {
                    Label { "4. Multi-line Align Content: Space Around" },
                    Flex {
                        .flexWrap = AFlexWrap::WRAP,
                        .justifyContent = AFlexJustifyContent::SPACE_EVENLY,
                        .alignContent = AFlexAlignContent::SPACE_AROUND,
                        .children = AVector<_<AView>>::generate(8, [](size_t i) {
                            return _new<AButton>("Item {}"_format(i + 1)) AUI_OVERRIDE_STYLE { FixedSize { 80_dp, 30_dp } };
                        })
                    } AUI_OVERRIDE_STYLE {
                        MinSize { {}, 200_dp },
                        Expanding { 1, 0 }
                    },
                },

                GroupBox {
                    Label { "5. Cross-row Reordering (Order)" },
                    Flex {
                        .flexWrap = AFlexWrap::WRAP,
                        .children = {
                            Label { "(A) Order 10" } AUI_OVERRIDE_STYLE { FlexOrder { 10 }, BackgroundSolid { AColor::RED }, Padding { 4_dp }, Margin { 2_dp }, FixedSize { 100_dp, 30_dp } },
                            Label { "(B) Order 5" } AUI_OVERRIDE_STYLE { FlexOrder { 5 },  BackgroundSolid { AColor::GREEN }, Padding { 4_dp }, Margin { 2_dp }, FixedSize { 100_dp, 30_dp } },
                            Label { "(C) Order 1" } AUI_OVERRIDE_STYLE { FlexOrder { 1 },  BackgroundSolid { AColor::BLUE },  Padding { 4_dp }, Margin { 2_dp }, FixedSize { 100_dp, 30_dp } },
                            Label { "(D) Order 15" } AUI_OVERRIDE_STYLE { FlexOrder { 15 }, BackgroundSolid { 0xffff00_rgb }, Padding { 4_dp }, Margin { 2_dp }, FixedSize { 100_dp, 30_dp } },
                            Label { "(E) Order 2" } AUI_OVERRIDE_STYLE { FlexOrder { 2 },  BackgroundSolid { 0xff00ff_rgb }, Padding { 4_dp }, Margin { 2_dp }, FixedSize { 100_dp, 30_dp } }
                        }
                    } AUI_OVERRIDE_STYLE {
                        MinSize { 220_dp, {} },
                        Expanding { 1, 0 }
                    },
                },

                GroupBox {
                    Label { "6. Column Reverse + Baseline-ish (Center)" },
                    Flex {
                        .flexDirection = AFlexDirection::COLUMN_REVERSE,
                        .alignItems = AFlexAlignItems::CENTER,
                        .children = {
                            _new<ALabel>("Large Text") AUI_OVERRIDE_STYLE { FontSize { 24_pt }, BackgroundSolid { 0xffa500_rgb } },
                            _new<AButton>("Small Button"),
                            _new<ALabel>("Medium Text") AUI_OVERRIDE_STYLE { FontSize { 14_pt }, BackgroundSolid { 0x00ffff_rgb } },
                        }
                    } AUI_OVERRIDE_STYLE {
                        Expanding { 1, 0 },
                        MinSize { {}, 150_dp },
                        LayoutSpacing { 4_dp }
                    }
                },

                GroupBox {
                    Label { "7. Wrap Reverse + Align Content Center + Space Evenly" },
                    Flex {
                        .flexWrap = AFlexWrap::WRAP_REVERSE,
                        .justifyContent = AFlexJustifyContent::SPACE_EVENLY,
                        .alignContent = AFlexAlignContent::CENTER,
                        .children = AVector<_<AView>>::generate(6, [](size_t i) {
                            return _new<AButton>("Item {}"_format(i + 1)) AUI_OVERRIDE_STYLE { FixedSize { 80_dp, 40_dp } };
                        })
                    } AUI_OVERRIDE_STYLE {
                        MinSize { {}, 200_dp },
                        LayoutSpacing { 10_dp },
                        Expanding { 1, 0 }
                    },
                },

                GroupBox {
                    Label { "8. Column Wrap (The Final Boss)" },
                    Flex {
                        .flexDirection = AFlexDirection::COLUMN,
                        .flexWrap = AFlexWrap::WRAP,
                        .justifyContent = AFlexJustifyContent::SPACE_BETWEEN,
                        .alignContent = AFlexAlignContent::CENTER,
                        .children = AVector<_<AView>>::generate(10, [](size_t i) {
                            return _new<AButton>("C{}"_format(i + 1)) AUI_OVERRIDE_STYLE { FixedSize { 60_dp, 40_dp }, Margin { 2_dp } };
                        })
                    } AUI_OVERRIDE_STYLE {
                        FixedSize { {}, 150_dp },
                        BackgroundSolid { AColor::GRAY },
                        Expanding { 1, 0 }
                    },
                },

                GroupBox {
                    Label { "9. Deeply Nested Flex" },
                    Flex {
                        .children = {
                            Flex {
                                .flexDirection = AFlexDirection::COLUMN,
                                .children = {
                                    Label { "SB 1" },
                                    Label { "SB 2" },
                                    _new<ASpacerExpanding>(),
                                    Label { "SB Bottom" },
                                }
                            } AUI_OVERRIDE_STYLE {
                                MinSize { 60_dp, {} },
                                BackgroundSolid { AColor::BLACK.transparentize(0.2f) },
                                TextColor { AColor::WHITE },
                                Padding { 4_dp }
                            },
                            Flex {
                                .flexDirection = AFlexDirection::COLUMN,
                                .children = {
                                    Flex {
                                        .justifyContent = AFlexJustifyContent::SPACE_BETWEEN,
                                        .children = {
                                            Button { .content = Label {"Header Left" } },
                                            _new<ASpacerExpanding>(),
                                            Button { .content = Label {"Header Right" } },
                                        }
                                    } AUI_OVERRIDE_STYLE {
                                        Expanding { 1, 0 },
                                        Padding { 4_dp },
                                        BackgroundSolid { 0xcccccc_rgb }
                                    },
                                    Flex {
                                        .flexDirection = AFlexDirection::COLUMN,
                                        .children = {
                                            Label { "Content Area with items that should wrap properly if the container is resized and we have enough items here to trigger wrapping behavior." } AUI_OVERRIDE_STYLE { Expanding { 1, 0 } },
                                            Flex {
                                                .flexWrap = AFlexWrap::WRAP,
                                                .justifyContent = AFlexJustifyContent::CENTER,
                                                .children = {
                                                    Button { .content = Label { "A" } },
                                                    Button { .content = Label { "B" } },
                                                    Button { .content = Label { "C" } },
                                                }
                                            }
                                        }
                                    } AUI_OVERRIDE_STYLE {
                                        Expanding { 1, 1 },
                                        Padding { 10_dp }
                                    }
                                }
                            } AUI_OVERRIDE_STYLE {
                                FlexGrow { 1.f },
                                Expanding { 1, 1 }
                            }
                        }
                    } AUI_OVERRIDE_STYLE {
                        Expanding { 1, 0 },
                        MinSize { {}, 300_dp },
                        BackgroundSolid { 0xeeeeee_rgb },
                        Border { 1_px, AColor::GRAY }
                    },
                },
            } AUI_LET { it->setExpanding(); }
        )
    });
    window->show();
    return 0;
}
/// [AFlexLayout_example]

