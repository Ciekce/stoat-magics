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

#include <array>
#include <span>

#include "../core.h"
#include "../util/bits.h"
#include "../util/multi_array.h"
#include "data.h"

namespace stoat::attacks {
    namespace pext {
        extern const util::MultiArray<Bitboard, Colors::kCount, kLanceDataTableSize> g_lanceAttacks;

        [[nodiscard]] inline std::span<const Bitboard, kLanceDataTableSize> lanceAttacks(Color c) {
            assert(c);
            return g_lanceAttacks[c.idx()];
        }

        extern const std::array<Bitboard, kBishopData.tableSize> g_bishopAttacks;
        extern const std::array<Bitboard, kRookData.tableSize> g_rookAttacks;
    } // namespace pext

    [[nodiscard]] inline Bitboard lanceAttacks(Square sq, Color c, Bitboard occ) {
        const auto& sqData = pext::lanceData(c).squares[sq.idx()];

        const usize idx = util::pext(occ.raw(), sqData.mask.raw(), sqData.shift);
        return pext::lanceAttacks(c)[sqData.offset + idx];
    }

    [[nodiscard]] inline Bitboard bishopAttacks(Square sq, Bitboard occ) {
        const auto& sqData = pext::kBishopData.squares[sq.idx()];

        const usize idx = util::pext(occ.raw(), sqData.mask.raw(), sqData.shift);
        return pext::g_bishopAttacks[sqData.offset + idx];
    }

    [[nodiscard]] inline Bitboard rookAttacks(Square sq, Bitboard occ) {
        const auto& sqData = pext::kRookData.squares[sq.idx()];

        const usize idx = util::pext(occ.raw(), sqData.mask.raw(), sqData.shift);
        return pext::g_rookAttacks[sqData.offset + idx];
    }
} // namespace stoat::attacks::sliders
