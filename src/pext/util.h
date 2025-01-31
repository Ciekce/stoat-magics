/*
 * Copyright (c) 2025 Ciekce
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "../types.h"

#include "../bitboard.h"
#include "../core.h"
#include "../util/multi_array.h"

namespace stoat::attacks::internal {
    [[nodiscard]] constexpr Bitboard edges(i32 dir) {
        switch (dir) {
            case offsets::kNorth:
                return Bitboards::kRankA;
            case offsets::kSouth:
                return Bitboards::kRankI;
            case offsets::kWest:
                return Bitboards::kFile9;
            case offsets::kEast:
                return Bitboards::kFile1;
            case offsets::kNorthWest:
                return Bitboards::kRankA | Bitboards::kFile9;
            case offsets::kNorthEast:
                return Bitboards::kRankA | Bitboards::kFile1;
            case offsets::kSouthWest:
                return Bitboards::kRankI | Bitboards::kFile9;
            case offsets::kSouthEast:
                return Bitboards::kRankI | Bitboards::kFile1;
            default:
                assert(false);
                return Bitboards::kEmpty;
        }
    }

    [[nodiscard]] constexpr Bitboard generateSlidingAttacks(Square src, i32 dir, Bitboard occ) {
        assert(src);

        auto blockers = edges(dir);
        auto bit = Bitboard::fromSquare(src);

        if (!(blockers & bit).empty()) {
            return Bitboards::kEmpty;
        }

        blockers |= occ;

        const bool right = dir < 0;
        const auto shift = dir < 0 ? -dir : dir;

        Bitboard dst{};

        do {
            if (right) {
                bit >>= shift;
            } else {
                bit <<= shift;
            }

            dst |= bit;
        } while ((bit & blockers).empty());

        return dst;
    }
} // namespace stoat::attacks::internal
