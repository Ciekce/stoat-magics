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

#include "../types.h"

#include <bit>

#include "../arch.h"

#if ST_FAST_PEXT
    #include <immintrin.h>
#endif

namespace stoat::util {
    namespace fallback {
        [[nodiscard]] constexpr i32 ctz(u128 v) {
            i32 count{};

            for (u128 bit = 1; bit != 0 && (v & bit) == 0; bit <<= 1) {
                ++count;
            }

            return count;
        }

        [[nodiscard]] constexpr u128 pext(u128 v, u128 mask) {
            u128 dst{};

            for (u128 bit = 1; mask != 0; bit <<= 1) {
                if ((v & mask & -mask) != 0) {
                    dst |= bit;
                }

                mask &= mask - 1;
            }

            return dst;
        }

        [[nodiscard]] constexpr u128 pdep(u128 v, u128 mask) {
            u128 dst{};

            for (u128 bit = 1; mask != 0; bit <<= 1) {
                if ((v & bit) != 0) {
                    dst |= mask & -mask;
                }

                mask &= mask - 1;
            }

            return dst;
        }
    } // namespace fallback

    [[nodiscard]] constexpr i32 ctz(u128 v) {
        if (std::is_constant_evaluated()) {
            return fallback::ctz(v);
        }

        const auto [high, low] = fromU128(v);

        if (low == 0) {
            return 64 + __builtin_ctzll(high);
        } else {
            return __builtin_ctzll(low);
        }
    }

    [[nodiscard]] constexpr i32 popcount(u128 v) {
        const auto [high, low] = fromU128(v);
        return std::popcount(high) + std::popcount(low);
    }

    [[nodiscard]] constexpr u128 pext(u128 v, u128 mask, i32 shift) {
#if ST_HAS_FAST_PEXT
        if (std::is_constant_evaluated()) {
            return fallback::pext(v, mask);
        }

        const auto [vHigh, vLow] = fromU128(v);
        const auto [maskHigh, maskLow] = fromU128(mask);

        assert(shift == std::popcount(maskLow));

        const auto high = _pext_u64(vHigh, maskHigh);
        const auto low = _pext_u64(vLow, maskLow);

        return (static_cast<u128>(high) << shift) | static_cast<u128>(low);
#else
        return fallback::pext(v, mask);
#endif
    }

    [[nodiscard]] constexpr u128 pext(u128 v, u128 mask) {
        return pext(v, mask, std::popcount(static_cast<u64>(mask)));
    }

    [[nodiscard]] constexpr u128 pdep(u128 v, u128 mask, i32 shift) {
#if ST_HAS_FAST_PEXT
        if (std::is_constant_evaluated()) {
            return fallback::pdep(v, mask);
        }

        const u64 vHigh = v >> shift;
        const u64 vLow = v;

        const auto [maskHigh, maskLow] = fromU128(mask);

        assert(shift == std::popcount(maskLow));

        const auto high = _pdep_u64(vHigh, maskHigh);
        const auto low = _pdep_u64(vLow, maskLow);

        return toU128(high, low);
#else
        return fallback::pdep(v, mask);
#endif
    }

    [[nodiscard]] constexpr u128 pdep(u128 v, u128 mask) {
        return pdep(v, mask, std::popcount(static_cast<u64>(mask)));
    }
} // namespace stoat::util
