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

//
// Created by nelonn on 9/4/25.
//

#include "AUtf8View.h"
#include "AString.h"
#include "AStringVector.h"
#include "AStaticVector.h"

#include <AUI/Common/AByteBuffer.h>
#include <simdutf.h>
#include <algorithm>
#include <charconv>

bool AUtf8View::contains(AChar c) const noexcept {
    for (auto it = begin(); it != end(); ++it) {
        if (*it == c) {
            return true;
        }
    }
    return false;
}

bool AUtf8View::contains(AUtf8View str) const noexcept {
    if (str.empty()) return true;
    if (str.length() > length()) return false;
    return std::search(bytes().begin(), bytes().end(), str.bytes().begin(), str.bytes().end()) != bytes().end();
}

AUtf8View::size_type AUtf8View::length() const noexcept {
    return simdutf::count_utf8(m_data, m_size);
}

AUtf8View AUtf8View::substr(size_type pos, size_type count) const noexcept {
    auto it = begin();
    for (; it != end() && pos > 0; ++it, --pos);
    auto begin_it = it;
    for (; it != end() && count > 0; ++it, --count);
    return AUtf8View(begin_it.data(), it.data() - begin_it.data());
}

bool AUtf8View::startsWith(AUtf8View prefix) const noexcept {
    if (prefix.size() > size()) {
        return false;
    }
    return bytes().substr(0, prefix.size()) == prefix.bytes();
}

bool AUtf8View::endsWith(AUtf8View suffix) const noexcept {
    if (suffix.size() > size()) {
        return false;
    }
    return bytes().substr(size() - suffix.size()) == suffix.bytes();
}

AUtf8View AUtf8View::trimLeft(AChar symbol) const noexcept {
    for (auto i = begin(); i != end(); ++i) {
        if (*i != symbol) {
            return AUtf8View(i.data(), end().data() - i.data());
        }
    }
    return {};
}

AUtf8View AUtf8View::trimRight(AChar symbol) const noexcept {
    for (auto i = rbegin(); i != rend(); ++i) {
        if (*i != symbol) {
            return AUtf8View(data(), i.base().data() - data());
        }
    }
    return {};
}

AString AUtf8View::uppercase() const {
    std::string buf{bytes()};
    {
        auto p = reinterpret_cast<unsigned char *>(buf.data());
        unsigned char* pExtChar = 0;
        while (*p) {
            if ((*p >= 0x61) && (*p <= 0x7a)) // US ASCII
                (*p) -= 0x20;
            else if (*p > 0xc0) {
                pExtChar = p;
                p++;
                switch (*pExtChar) {
                    case 0xc3: // Latin 1
                        if ((*p >= 0xa0)
                            && (*p <= 0xbe)
                            && (*p != 0xb7))
                            (*p) -= 0x20; // US ASCII shift
                        break;
                    case 0xc4: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0xb7)
                            && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0xb9)
                                 && (*p <= 0xbe)
                                 && (!(*p % 2))) // Even
                            (*p)--; // Prev char is upr
                        else if (*p == 0xbf) {
                            *pExtChar = 0xc5;
                            (*p) = 0x80;
                        }
                        break;
                    case 0xc5: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0x88)
                            && (!(*p % 2))) // Even
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0x8a)
                                 && (*p <= 0xb7)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0xb9)
                                 && (*p <= 0xbe)
                                 && (!(*p % 2))) // Even
                            (*p)--; // Prev char is upr
                        break;
                    case 0xc6: // Latin Exteneded
                        switch (*p) {
                            case 0x83:
                            case 0x85:
                            case 0x88:
                            case 0x8c:
                            case 0x92:
                            case 0x99:
                            case 0xa1:
                            case 0xa3:
                            case 0xa5:
                            case 0xa8:
                            case 0xad:
                            case 0xb0:
                            case 0xb4:
                            case 0xb6:
                            case 0xb9:
                            case 0xbd:
                                (*p)--; // Prev char is upr
                                break;
                            default:
                                break;
                        }
                        break;
                    case 0xc7: // Latin Exteneded
                        if (*p == 0x86)
                            (*p) = 0x84;
                        else if (*p == 0x85)
                            (*p)--; // Prev char is upr
                        else if (*p == 0x89)
                            (*p) = 0x87;
                        else if (*p == 0x88)
                            (*p)--; // Prev char is upr
                        else if (*p == 0x8c)
                            (*p) = 0x8a;
                        else if (*p == 0x8b)
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0x8d)
                                 && (*p <= 0x9c)
                                 && (!(*p % 2))) // Even
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0x9e)
                                 && (*p <= 0xaf)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if (*p == 0xb3)
                            (*p) = 0xb1;
                        else if (*p == 0xb2)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xb4)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xb8)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xba)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xbc)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xbe)
                            (*p)--; // Prev char is upr
                        break;
                    case 0xc8: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0x9f)
                            && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0xa2)
                                 && (*p <= 0xb3)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if (*p == 0xbc)
                            (*p)--; // Prev char is upr
                        break;
                    case 0xcd: // Greek & Coptic
                        switch (*p) {
                            case 0xb1:
                            case 0xb3:
                            case 0xb7:
                                (*p)--; // Prev char is upr
                                break;
                            default:
                                if (*p == 0xbb) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0xbd;
                                }
                                else if (*p == 0xbc) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0xbe;
                                }
                                else if (*p == 0xbd) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0xbf;
                                }
                                break;
                        }
                        break;
                    case 0xce: // Greek & Coptic
                        if (*p == 0xac)
                            (*p) = 0x86;
                        else if (*p == 0xad)
                            (*p) = 0x88;
                        else if (*p == 0xae)
                            (*p) = 0x89;
                        else if (*p == 0xaf)
                            (*p) = 0x8a;
                        else if ((*p >= 0xb1)
                                 && (*p <= 0xbf))
                            (*p) -= 0x20; // US ASCII shift
                        break;
                    case 0xcf: // Greek & Coptic
                        if (*p == 0xb4)
                            (*p) = 0x8f;
                        else if (*p == 0x92)
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0x98)
                                 && (*p <= 0xaf)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if (*p == 0x91)
                            (*p) = 0xb4;
                        else if (*p == 0xb8)
                            (*p)--; // Prev char is upr
                        else if (*p == 0xb2)
                            (*p) = 0xb9;
                        else if (*p == 0xbc)
                            (*p)--; // Prev char is upr
                        else if (*p == 0x8c) {
                            *pExtChar = 0xce;
                            (*p) = 0x8c;
                        }
                        else if (*p == 0x8d) {
                            *pExtChar = 0xce;
                            (*p) = 0x8e;
                        }
                        else if (*p == 0x8e) {
                            *pExtChar = 0xce;
                            (*p) = 0x8f;
                        }
                        else if ((*p >= 0x80)
                                 && (*p <= 0x8b)
                                 && (*p != 0x82)) {
                            *pExtChar = 0xce;
                            (*p) += 0x20;
                        }
                        else if (*p == 0xb3) {
                            *pExtChar = 0xcd;
                            (*p) = 0xbf;
                        }
                        break;
                    case 0xd0: // Cyrillic
                        if ((*p >= 0xb0)
                            && (*p <= 0xbf))
                            (*p) -= 0x20; // US ASCII shift
                        break;
                    case 0xd1: // Cyrillic supplement
                        if ((*p >= 0x90)
                            && (*p <= 0x9f)) {
                            *pExtChar = 0xd0;
                            (*p) -= 0x10;
                        }
                        else if ((*p >= 0x80)
                                 && (*p <= 0x8f)) {
                            *pExtChar = 0xd0;
                            (*p) += 0x20;
                        }
                        else if ((*p >= 0xa0)
                                 && (*p <= 0xbf)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        break;
                    case 0xd2: // Cyrillic supplement
                        if (*p == 0x80)
                            (*p)++; // Prev char is upr
                        else if ((*p >= 0x8a)
                                 && (*p <= 0xbf)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        break;
                    case 0xd3: // Cyrillic supplement
                        if ((*p >= 0x81)
                            && (*p <= 0x8e)
                            && (!(*p % 2))) // Even
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0x90)
                                 && (*p <= 0xbf)
                                 && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        break;
                    case 0xd4: // Cyrillic supplement & Armenian
                        if ((*p >= 0x80)
                            && (*p <= 0xaf)
                            && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        break;
                    case 0xd5: // Armenian
                        if ((*p >= 0x80)
                            && (*p <= 0x96)
                            && (*p % 2)) // Odd
                            (*p)--; // Prev char is upr
                        else if ((*p >= 0xa1)
                                 && (*p <= 0xaf)) {
                            *pExtChar = 0xd4;
                            (*p) += 0x10;
                        }
                        break;
                    case 0xe1: // Three byte code
                        pExtChar = p;
                        p++;
                        switch (*pExtChar) {
                            case 0x82: // Georgian
                                break;
                            case 0x83: // Georgian
                                if ((*p >= 0x90)
                                    && (*p <= 0xaf)) {
                                    *pExtChar = 0x82;
                                    (*p) += 0x10;
                                }
                                else if (((*p >= 0xb0)
                                          && ((*p <= 0xb5)
                                              || (*p == 0xb7)))
                                         || (*p == 0xbd))
                                    (*p) -= 0x30;
                                break;
                            case 0xb8: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (*p % 2)) // Odd
                                    (*p)--; // Prev char is upr
                                break;
                            case 0xb9: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (*p % 2)) // Odd
                                    (*p)--; // Prev char is upr
                                break;
                            case 0xba: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x94)
                                    && (*p % 2)) // Odd
                                    (*p)--; // Prev char is upr
                                else if ((*p >= 0x9e)
                                         && (*p <= 0xbf)
                                         && (*p % 2)) // Odd
                                    (*p)--; // Prev char is upr
                                break;
                            case 0xbb: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (*p % 2)) // Odd
                                    (*p)--; // Prev char is upr
                                break;
                            case 0xbc: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) += 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) += 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) += 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0x87))
                                    (*p) += 0x08;
                                break;
                            case 0xbd: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) += 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) += 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) += 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0x87))
                                    (*p) += 0x08;
                                break;
                            case 0xbe: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) += 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) += 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) += 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0xb1))
                                    (*p) += 0x08;
                                break;
                            case 0xbf: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x84))
                                    (*p) += 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x93))
                                    (*p) += 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa4))
                                    (*p) += 0x08;
                                break;
                            default:
                                break;
                        }
                        break;
                    case 0xf0: // Four byte code
                        pExtChar = p;
                        p++;
                        switch (*pExtChar) {
                            case 0x90:
                                pExtChar = p;
                                p++;
                                switch (*pExtChar) {
                                    case 0x92: // Osage
                                        break;
                                    case 0x93: // Osage
                                        if ((*p >= 0x80)
                                            && (*p <= 0x93))
                                            (*p) += 0x18;
                                        else if ((*p >= 0x98)
                                                 && (*p <= 0xa7)) {
                                            *pExtChar = 0x92;
                                            (*p) += 0x18;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case 0x9E:
                                pExtChar = p;
                                p++;
                                switch (*pExtChar) {
                                    case 0xA4: // Adlam
                                        if ((*p >= 0xa2)
                                            && (*p <= 0xc3))
                                            (*p) -= 0x22;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                pExtChar = 0;
            }
            p++;
        }
    }

    return buf;
}

AString AUtf8View::lowercase() const {
    std::string buf{bytes()};
    {
        auto p = reinterpret_cast<unsigned char *>(buf.data());
        unsigned char* pExtChar = 0;
        while (*p) {
            if ((*p >= 0x41) && (*p <= 0x5a)) // US ASCII
                (*p) += 0x20;
            else if (*p > 0xc0) {
                pExtChar = p;
                p++;
                switch (*pExtChar) {
                    case 0xc3: // Latin 1
                        if ((*p >= 0x80)
                            && (*p <= 0x9e)
                            && (*p != 0x97))
                            (*p) += 0x20; // US ASCII shift
                        break;
                    case 0xc4: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0xb7)
                            && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0xb9)
                                 && (*p <= 0xbe)
                                 && (*p % 2)) // Odd
                            (*p)++; // Next char is lwr
                        else if (*p == 0xbf) {
                            *pExtChar = 0xc5;
                            (*p) = 0x80;
                        }
                        break;
                    case 0xc5: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0x88)
                            && (*p % 2)) // Odd
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0x8a)
                                 && (*p <= 0xb7)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0xb9)
                                 && (*p <= 0xbe)
                                 && (*p % 2)) // Odd
                            (*p)++; // Next char is lwr
                        break;
                    case 0xc6: // Latin Exteneded
                        switch (*p) {
                            case 0x82:
                            case 0x84:
                            case 0x87:
                            case 0x8b:
                            case 0x91:
                            case 0x98:
                            case 0xa0:
                            case 0xa2:
                            case 0xa4:
                            case 0xa7:
                            case 0xac:
                            case 0xaf:
                            case 0xb3:
                            case 0xb5:
                            case 0xb8:
                            case 0xbc:
                                (*p)++; // Next char is lwr
                                break;
                            default:
                                break;
                        }
                        break;
                    case 0xc7: // Latin Exteneded
                        if (*p == 0x84)
                            (*p) = 0x86;
                        else if (*p == 0x87)
                            (*p) = 0x89;
                        else if (*p == 0x8a)
                            (*p) = 0x8b;
                        else if (*p == 0x8c)
                            (*p) = 0x8d;
                        else if ((*p >= 0x8d)
                                 && (*p <= 0x9c)
                                 && (*p % 2)) // Odd
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0x9e)
                                 && (*p <= 0xaf)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if (*p == 0xb1)
                            (*p) = 0xb3;
                        else if (*p == 0xb2)
                            (*p)++; // Next char is lwr
                        else if (*p == 0xb4)
                            (*p)++; // Next char is lwr
                        else if (*p == 0xb8)
                            (*p)++; // Next char is lwr
                        else if (*p == 0xba)
                            (*p)++; // Next char is lwr
                        else if (*p == 0xbc)
                            (*p)++; // Next char is lwr
                        else if (*p == 0xbe)
                            (*p)++; // Next char is lwr
                        break;
                    case 0xc8: // Latin Exteneded
                        if ((*p >= 0x80)
                            && (*p <= 0x9f)
                            && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0xa2)
                                 && (*p <= 0xb3)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if (*p == 0xbc)
                            (*p)++; // Next char is lwr
                        break;
                    case 0xcd: // Greek & Coptic
                        switch (*p) {
                            case 0xb0:
                            case 0xb2:
                            case 0xb6:
                                (*p)++; // Next char is lwr
                                break;
                            default:
                                if (*p == 0xba) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0x91;
                                }
                                else if (*p == 0xbb) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0x92;
                                }
                                else if (*p == 0xbc) {
                                    *pExtChar = 0xcf;
                                    (*p) = 0x93;
                                }
                                break;
                        }
                        break;
                    case 0xce: // Greek & Coptic
                        if (*p == 0x86)
                            (*p) = 0xac;
                        else if (*p == 0x88)
                            (*p) = 0xad;
                        else if (*p == 0x89)
                            (*p) = 0xae;
                        else if (*p == 0x8a)
                            (*p) = 0xaf;
                        else if ((*p >= 0x8c)
                                 && (*p <= 0xcf)) {
                            *pExtChar = 0xcf;
                            (*p) += 0x20;
                        }
                        else if ((*p >= 0xd1)
                                 && (*p <= 0xbf))
                            (*p) += 0x20; // US ASCII shift
                        break;
                    case 0xcf: // Greek & Coptic
                        if (*p == 0x8f)
                            (*p) = 0xb4;
                        else if (*p == 0x91)
                            (*p) = 0xb8;
                        else if (*p == 0xb4)
                            (*p) = 0xb2;
                        else if (*p == 0xb9)
                            (*p) = 0xba;
                        else if ((*p >= 0x98)
                                 && (*p <= 0xaf)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if (*p == 0xbc)
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0x80)
                                 && (*p <= 0x8b)
                                 && (*p != 0x82)) {
                            *pExtChar = 0xce;
                            (*p) -= 0x20;
                        }
                        else if (*p == 0xbf) {
                            *pExtChar = 0xcd;
                            (*p) = 0xb3;
                        }
                        break;
                    case 0xd0: // Cyrillic
                        if ((*p >= 0x90)
                            && (*p <= 0xbf))
                            (*p) += 0x20; // US ASCII shift
                        break;
                    case 0xd1: // Cyrillic supplement
                        if ((*p >= 0x80)
                            && (*p <= 0x8f)) {
                            *pExtChar = 0xd0;
                            (*p) -= 0x20;
                        }
                        else if ((*p >= 0x90)
                                 && (*p <= 0x9f)) {
                            *pExtChar = 0xd0;
                            (*p) += 0x10;
                        }
                        else if ((*p >= 0xa0)
                                 && (*p <= 0xbf)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        break;
                    case 0xd2: // Cyrillic supplement
                        if (*p == 0x81)
                            (*p)--; // Next char is lwr
                        else if ((*p >= 0x8a)
                                 && (*p <= 0xbf)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        break;
                    case 0xd3: // Cyrillic supplement
                        if ((*p >= 0x80)
                            && (*p <= 0x8d)
                            && (*p % 2)) // Odd
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0x90)
                                 && (*p <= 0xbf)
                                 && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        break;
                    case 0xd4: // Cyrillic supplement & Armenian
                        if ((*p >= 0x80)
                            && (*p <= 0xaf)
                            && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        break;
                    case 0xd5: // Armenian
                        if ((*p >= 0x80)
                            && (*p <= 0x96)
                            && (!(*p % 2))) // Even
                            (*p)++; // Next char is lwr
                        else if ((*p >= 0xa1)
                                 && (*p <= 0xaf)) {
                            *pExtChar = 0xd4;
                            (*p) -= 0x10;
                        }
                        break;
                    case 0xe1: // Three byte code
                        pExtChar = p;
                        p++;
                        switch (*pExtChar) {
                            case 0x82: // Georgian
                                break;
                            case 0x83: // Georgian
                                if ((*p >= 0x90)
                                    && (*p <= 0xaf)) {
                                    *pExtChar = 0x82;
                                    (*p) -= 0x10;
                                }
                                else if (((*p >= 0xb0)
                                          && ((*p <= 0xb5)
                                              || (*p == 0xb7)))
                                         || (*p == 0xbd))
                                    (*p) += 0x30;
                                break;
                            case 0xb8: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (!(*p % 2))) // Even
                                    (*p)++; // Next char is lwr
                                break;
                            case 0xb9: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (!(*p % 2))) // Even
                                    (*p)++; // Next char is lwr
                                break;
                            case 0xba: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x94)
                                    && (!(*p % 2))) // Even
                                    (*p)++; // Next char is lwr
                                else if ((*p >= 0x9e)
                                         && (*p <= 0xbf)
                                         && (!(*p % 2))) // Even
                                    (*p)++; // Next char is lwr
                                break;
                            case 0xbb: // Latin extened
                                if ((*p >= 0x80)
                                    && (*p <= 0xbf)
                                    && (!(*p % 2))) // Even
                                    (*p)++; // Next char is lwr
                                break;
                            case 0xbc: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) -= 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0x87))
                                    (*p) -= 0x08;
                                break;
                            case 0xbd: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) -= 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0x87))
                                    (*p) -= 0x08;
                                break;
                            case 0xbe: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x87))
                                    (*p) -= 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x97))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa7))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xb0)
                                         && (*p <= 0xb1))
                                    (*p) -= 0x08;
                                break;
                            case 0xbf: // Greek extened
                                if ((*p >= 0x80)
                                    && (*p <= 0x84))
                                    (*p) -= 0x08;
                                else if ((*p >= 0x90)
                                         && (*p <= 0x93))
                                    (*p) -= 0x08;
                                else if ((*p >= 0xa0)
                                         && (*p <= 0xa4))
                                    (*p) -= 0x08;
                                break;
                            default:
                                break;
                        }
                        break;
                    case 0xf0: // Four byte code
                        pExtChar = p;
                        p++;
                        switch (*pExtChar) {
                            case 0x90:
                                pExtChar = p;
                                p++;
                                switch (*pExtChar) {
                                    case 0x92: // Osage
                                        if ((*p >= 0x80)
                                            && (*p <= 0x93))
                                            (*p) -= 0x18;
                                        else if ((*p >= 0x98)
                                                 && (*p <= 0xa7)) {
                                            *pExtChar = 0x93;
                                            (*p) -= 0x18;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case 0x9E:
                                pExtChar = p;
                                p++;
                                switch (*pExtChar) {
                                    case 0xA4: // Adlam
                                        if ((*p >= 0xa2)
                                            && (*p <= 0xc3))
                                            (*p) += 0x22;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                pExtChar = 0;
            }
            p++;
        }
    }

    return buf;
}

AString AUtf8View::removedAll(AChar c) {
    AString result;
    result.reserve(sizeBytes());
    for (auto it = begin(); it != end(); ++it) {
        if (*it != c) {
            result += *it;
        }
    }
    return result;
}

AStringVector AUtf8View::split(AChar c) const {
    AStringVector result;
    size_t start = 0;
    for (size_t i = 0; i < length(); ++i) {
        if ((*this)[i] == c) {
            result << substr(start, i - start);
            start = i + 1;
        }
    }
    if (start < length()) {
        result << substr(start);
    }
    return result;
}

bool AUtf8View::toBool() const {
    return *this == "true";
}

AOptional<int32_t> AUtf8View::toInt() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    int32_t result;
    auto str = trimmed.bytes();
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    
    if (ec == std::errc{}) {
        return result;
    }
    return std::nullopt;
}

AOptional<int64_t> AUtf8View::toLong() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    int64_t result;
    auto str = trimmed.bytes();
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    
    if (ec == std::errc{}) {
        return result;
    }
    return std::nullopt;
}

AOptional<uint32_t> AUtf8View::toUInt() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    uint32_t result;
    auto str = trimmed.bytes();
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    
    if (ec == std::errc{}) {
        return result;
    }
    return std::nullopt;
}

AOptional<uint64_t> AUtf8View::toULong() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    uint64_t result;
    auto str = trimmed.bytes();
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    
    if (ec == std::errc{}) {
        return result;
    }
    return std::nullopt;
}

AOptional<float> AUtf8View::toFloat() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    float result;
    auto str = trimmed.toStdString();
    try {
        size_t pos;
        result = std::stof(str, &pos);
        if (pos == str.size()) {
            return result;
        }
    } catch (...) {
    }
    return std::nullopt;
}

AOptional<double> AUtf8View::toDouble() const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    double result;
    auto str = trimmed.toStdString();
    try {
        size_t pos;
        result = std::stod(str, &pos);
        if (pos == str.size()) {
            return result;
        }
    } catch (...) {
    }
    return std::nullopt;
}

AOptional<int> AUtf8View::toNumber(aui::ranged_number<int, 2, 36> base) const noexcept {
    if (empty()) return std::nullopt;
    
    auto trimmed = trim();
    if (trimmed.empty()) return std::nullopt;
    
    int result;
    auto str = trimmed.bytes();
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result, static_cast<int>(base));
    
    if (ec == std::errc{}) {
        return result;
    }
    return std::nullopt;
}
