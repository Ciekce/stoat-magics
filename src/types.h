/*
 Copyright (c) 2025 Ciekce

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

namespace stoat {
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using u128 = unsigned __int128;

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using i128 = __int128;

    using f32 = float;
    using f64 = double;

    using usize = std::size_t;

    [[nodiscard]] constexpr u128 toU128(u64 high, u64 low) {
        return (static_cast<u128>(high) << 64) | static_cast<u128>(low);
    }

    [[nodiscard]] constexpr std::pair<u64, u64> fromU128(u128 v) {
        return {static_cast<u64>(v >> 64), static_cast<u64>(v)};
    }
} // namespace stoat

#define U128(High, Low) (stoat::toU128(UINT64_C(High), UINT64_C(Low)))

#define ST_STRINGIFY_(S) #S
#define ST_STRINGIFY(S) ST_STRINGIFY_(S)
