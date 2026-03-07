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
// Created by nelonn on 9/4/26.
//

#pragma once

#include <AUI/Common/AUtf8.h>
#include <AUI/Common/AChar.h>
#include <AUI/Common/AOptional.h>
#include <AUI/Traits/values.h>
#include <string_view>
#include <fmt/format.h>

/**
 * @brief Represents a UTF-8 encoded string view with Unicode-aware operations.
 * @ingroup core
 * @details
 * AUtf8View provides a non-owning view over UTF-8 encoded data with comprehensive Unicode support.
 * Unlike std::string_view, AUtf8View understands UTF-8 encoding and provides operations on
 * Unicode code points rather than raw bytes.
 *
 * Each Unicode character (codepoint) is encoded using 1-4 consecutive code units.
 *
 * For owning version, see AString. For std::string_view compatible interface, see AStringView.
 */
class API_AUI_CORE AUtf8View {
private:
    const char* m_data;
    size_t m_size;

public:
    using iterator = AUtf8ConstIterator;
    using const_iterator = AUtf8ConstIterator;
    using reverse_iterator = AUtf8ConstReverseIterator;
    using const_reverse_iterator = AUtf8ConstReverseIterator;
    using value_type = AChar;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_type NPOS = static_cast<size_type>(-1);

    static constexpr auto TO_NUMBER_BASE_BIN = 2;
    static constexpr auto TO_NUMBER_BASE_OCT = 8;
    static constexpr auto TO_NUMBER_BASE_DEC = 10;
    static constexpr auto TO_NUMBER_BASE_HEX = 16;

    /**
     * @brief Default constructor - creates an empty view
     */
    constexpr AUtf8View() noexcept
        : m_data(nullptr), m_size(0) {}

    /**
     * @brief Construct from raw UTF-8 data pointer and size in bytes
     * @param data Pointer to UTF-8 encoded data
     * @param sizeInBytes Size of the data in bytes
     */
    constexpr AUtf8View(const char* data, size_t sizeInBytes) noexcept
        : m_data(data), m_size(sizeInBytes) {}

    /**
     * @brief Construct from null-terminated UTF-8 C-string
     * @param str Null-terminated UTF-8 string
     */
    constexpr AUtf8View(const char* str) noexcept
        : m_data(str), m_size(str ? std::char_traits<char>::length(str) : 0) {}

    /**
     * @brief Construct from std::string_view
     * @param str String view containing UTF-8 encoded data
     */
    constexpr AUtf8View(std::string_view str) noexcept
        : m_data(str.data()), m_size(str.size()) {}

    /**
     * @brief Construct from std::string
     * @param str String containing UTF-8 encoded data
     */
    explicit AUtf8View(const std::string& str) noexcept
        : m_data(str.data()), m_size(str.size()) {}

    /**
     * @brief Construct from AStringView
     * @param str String view containing UTF-8 encoded data
     */
    constexpr AUtf8View(AStringView str) noexcept
        : m_data(str.data()), m_size(str.size()) {}

    /**
     * @brief Returns raw byte data as std::string_view
     * @return String view of the underlying UTF-8 bytes
     */
    [[nodiscard]]
    constexpr std::string_view bytes() const noexcept {
        return std::string_view(m_data, m_size);
    }

    /**
     * @brief Returns pointer to the raw UTF-8 data
     * @return Pointer to the first byte of the string
     */
    [[nodiscard]]
    constexpr const char* data() const noexcept {
        return m_data;
    }

    /**
     * @brief Returns the number of bytes in the UTF-8 encoded string
     * @return Size in bytes
     * @sa length
     */
    [[nodiscard]]
    constexpr size_type sizeBytes() const noexcept {
        return m_size;
    }

    /**
     * @brief Returns the number of bytes in the UTF-8 encoded string
     * @return Size in bytes
     * @sa length
     */
    [[nodiscard]]
    constexpr size_type size() const noexcept {
        return m_size;
    }

    /**
     * @brief Returns the number of Unicode characters in the string
     * @return Number of Unicode code points
     * @sa sizeBytes
     */
    [[nodiscard]]
    size_type length() const noexcept;

    /**
     * @brief Checks if the string is empty
     * @return true if the string contains no characters
     */
    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_size == 0;
    }

    /**
     * @brief Unchecked access to the UTF-8 character at the specified position
     * @param i The position of the character to return
     * @return The character at the specified position
     */
    [[nodiscard]]
    AChar operator[](size_type i) const {
        if (empty() || i >= length()) {
            return AChar();
        }
        return *(begin() + static_cast<int>(i));
    }

    /**
     * @brief Returns a substring [pos, pos + count)
     * @param pos The starting position of the substring (in code points)
     * @param count The number of characters to include (in code points)
     * @return Substring view
     * @details
     * If the requested substring extends past the end of the string, the returned
     * substring is [pos, size()).
     */
    [[nodiscard]]
    AUtf8View substr(size_type pos = 0, size_type count = NPOS) const noexcept;

    /**
     * @brief Checks if the string starts with the given prefix
     * @param prefix The prefix to check
     * @return true if the string starts with prefix
     */
    [[nodiscard]]
    bool startsWith(AUtf8View prefix) const noexcept;

    /**
     * @brief Checks if the string ends with the given suffix
     * @param suffix The suffix to check
     * @return true if the string ends with suffix
     */
    [[nodiscard]]
    bool endsWith(AUtf8View suffix) const noexcept;

    /**
     * @brief Checks if the string contains the given character
     * @param c The character to search for
     * @return true if the character is found
     */
    [[nodiscard]]
    bool contains(AChar c) const noexcept;

    /**
     * @brief Checks if the string contains the given substring
     * @param str The substring to search for
     * @return true if the substring is found
     */
    [[nodiscard]]
    bool contains(AUtf8View str) const noexcept;

    /**
     * @brief Equality comparison
     * @param other Other view to compare
     * @return true if both views contain the same bytes
     */
    [[nodiscard]]
    constexpr bool operator==(AUtf8View other) const noexcept {
        return bytes() == other.bytes();
    }

    /**
     * @brief Inequality comparison
     * @param other Other view to compare
     * @return true if views contain different bytes
     */
    [[nodiscard]]
    constexpr bool operator!=(AUtf8View other) const noexcept {
        return bytes() != other.bytes();
    }

    /**
     * @brief Trims whitespace from the left side
     * @param symbol The character to trim (default: space)
     * @return Trimmed view
     */
    [[nodiscard]]
    AUtf8View trimLeft(AChar symbol = ' ') const noexcept;

    /**
     * @brief Trims whitespace from the right side
     * @param symbol The character to trim (default: space)
     * @return Trimmed view
     */
    [[nodiscard]]
    AUtf8View trimRight(AChar symbol = ' ') const noexcept;

    /**
     * @brief Trims whitespace from both sides
     * @param symbol The character to trim (default: space)
     * @return Trimmed view
     */
    [[nodiscard]]
    AUtf8View trim(AChar symbol = ' ') const noexcept {
        return trimLeft(symbol).trimRight(symbol);
    }

    /**
     * @brief Returns the first character of the string
     * @return First character, or null character if empty
     */
    [[nodiscard]]
    AChar first() const {
        if (empty()) {
            return AChar();
        }
        return *begin();
    }

    /**
     * @brief Returns the last character of the string
     * @return Last character, or null character if empty
     */
    [[nodiscard]]
    AChar last() const {
        if (empty()) {
            return AChar();
        }
        auto it = end();
        --it;
        return *it;
    }

    /**
     * @brief Converts the string to uppercase
     * @return Uppercase string (as AString since case conversion may change byte length)
     */
    AString uppercase() const;

    /**
     * @brief Converts the string to lowercase
     * @return Lowercase string (as AString since case conversion may change byte length)
     */
    AString lowercase() const;

    /**
     * @brief Removes all occurrences of a character
     * @param c Character to remove
     * @return New string with all occurrences removed
     */
    AString removedAll(AChar c);

    /**
     * @brief Splits the string by a delimiter character
     * @param c Delimiter character
     * @return Vector of substrings
     */
    AStringVector split(AChar c) const;

    // Iterators

    /**
     * @brief Returns iterator to the beginning
     */
    [[nodiscard]]
    constexpr iterator begin() const noexcept {
        return AUtf8ConstIterator(m_data, m_data, m_data + m_size, 0);
    }

    /**
     * @brief Returns iterator to the end
     */
    [[nodiscard]]
    constexpr iterator end() const noexcept {
        return AUtf8ConstIterator(m_data, m_data, m_data + m_size, m_size);
    }

    /**
     * @brief Returns const iterator to the beginning
     */
    [[nodiscard]]
    constexpr const_iterator cbegin() const noexcept {
        return begin();
    }

    /**
     * @brief Returns const iterator to the end
     */
    [[nodiscard]]
    constexpr const_iterator cend() const noexcept {
        return end();
    }

    /**
     * @brief Returns reverse iterator to the beginning
     */
    [[nodiscard]]
    constexpr reverse_iterator rbegin() const noexcept {
        return AUtf8ConstReverseIterator(end());
    }

    /**
     * @brief Returns reverse iterator to the end
     */
    [[nodiscard]]
    constexpr reverse_iterator rend() const noexcept {
        return AUtf8ConstReverseIterator(begin());
    }

    /**
     * @brief Returns const reverse iterator to the beginning
     */
    [[nodiscard]]
    constexpr const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    /**
     * @brief Returns const reverse iterator to the end
     */
    [[nodiscard]]
    constexpr const_reverse_iterator crend() const noexcept {
        return rend();
    }

    // Conversion methods

    /**
     * @brief Converts the string to boolean value
     * @return true if the string equals "true", false otherwise
     */
    [[nodiscard]]
    bool toBool() const;

    /**
     * @brief Converts the string to int value
     * @return Optional containing the integer value, or nullopt if conversion fails
     * @details
     * Uses base 10 by default. If the string starts with 0x or 0X, base 16 is used.
     */
    [[nodiscard]]
    AOptional<int32_t> toInt() const noexcept;

    /**
     * @brief Converts the string to long value
     * @return Optional containing the long value, or nullopt if conversion fails
     * @details
     * Uses base 10 by default. If the string starts with 0x or 0X, base 16 is used.
     */
    [[nodiscard]]
    AOptional<int64_t> toLong() const noexcept;

    /**
     * @brief Converts the string to unsigned int value
     * @return Optional containing the unsigned int value, or nullopt if conversion fails
     * @details
     * Uses base 10 by default. If the string starts with 0x or 0X, base 16 is used.
     */
    [[nodiscard]]
    AOptional<uint32_t> toUInt() const noexcept;

    /**
     * @brief Converts the string to unsigned long value
     * @return Optional containing the unsigned long value, or nullopt if conversion fails
     * @details
     * Uses base 10 by default. If the string starts with 0x or 0X, base 16 is used.
     */
    [[nodiscard]]
    AOptional<uint64_t> toULong() const noexcept;

    /**
     * @brief Converts the string to a float number
     * @return Optional containing the float value, or nullopt if conversion fails
     */
    [[nodiscard]]
    AOptional<float> toFloat() const noexcept;

    /**
     * @brief Converts the string to a double number
     * @return Optional containing the double value, or nullopt if conversion fails
     */
    [[nodiscard]]
    AOptional<double> toDouble() const noexcept;

    /**
     * @brief Converts the string to an integer using specified base
     * @param base Number base (2-36)
     * @return Optional containing the integer value, or nullopt if conversion fails
     */
    [[nodiscard]]
    AOptional<int> toNumber(aui::ranged_number<int, 2, 36> base) const noexcept;

    // Exception-throwing variants

    [[nodiscard]]
    int32_t toIntOrException() const {
        return toInt().valueOrException(fmt::format("bad to number conversion: {}", bytes()).c_str());
    }

    [[nodiscard]]
    int64_t toLongOrException() const {
        return toLong().valueOrException(fmt::format("bad to number conversion: {}", bytes()).c_str());
    }

    [[nodiscard]]
    uint32_t toUIntOrException() const {
        return toUInt().valueOrException(fmt::format("bad to number conversion: {}", bytes()).c_str());
    }

    [[nodiscard]]
    uint64_t toULongOrException() const {
        return toULong().valueOrException(fmt::format("bad to number conversion: {}", bytes()).c_str());
    }

    [[nodiscard]]
    float toFloatOrException() const noexcept {
        return toFloat().valueOrException(fmt::format("bad float: {}", bytes()).c_str());
    }

    [[nodiscard]]
    double toDoubleOrException() const noexcept {
        return toDouble().valueOrException(fmt::format("bad double: {}", bytes()).c_str());
    }

    [[nodiscard]]
    int toNumberOrException(aui::ranged_number<int, 2, 36> base = TO_NUMBER_BASE_DEC) const {
        return toNumber(base).valueOrException(fmt::format("bad to number conversion: {}", bytes()).c_str());
    }

    /**
     * @brief Converts to std::string
     * @return std::string containing the UTF-8 bytes
     */
    [[nodiscard]]
    std::string toStdString() const {
        return std::string(bytes());
    }

    /**
     * @brief Converts to AStringView
     * @return AStringView of the underlying data
     */
    [[nodiscard]]
    operator AStringView() const noexcept {
        return AStringView(m_data, m_size);
    }
};

template<>
struct std::hash<AUtf8View>
{
    size_t operator()(const AUtf8View& t) const noexcept
    {
        return std::hash<std::string_view>()(t.bytes());
    }
};

inline std::ostream& operator<<(std::ostream& o, const AUtf8View& s)
{
    o << s.bytes();
    return o;
}

#if defined(FMT_VERSION) && (FMT_VERSION < 100000)
template <> struct fmt::detail::is_string<AUtf8View>: std::false_type {};
#endif

template <> struct fmt::formatter<AUtf8View>: fmt::formatter<std::string_view> {
    auto format(const AUtf8View& s, fmt::format_context& ctx) const {
        return fmt::formatter<std::string_view>::format(s.bytes(), ctx);
    }
};

// gtest printer for AUtf8View
inline void PrintTo(AUtf8View s, std::ostream* stream) {
    *stream << s.bytes();
}
