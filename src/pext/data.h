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

#include "../bitboard.h"
#include "../core.h"
#include "util.h"

namespace stoat::attacks::pext {
    namespace internal {
        struct SquareData {
            Bitboard mask;
            u32 offset;
            i32 shift;
        };

        struct PieceData {
            std::array<SquareData, Squares::kCount> squares;
            u32 tableSize;
        };

        template <i32... kDirs>
        consteval PieceData generatePieceData() {
            PieceData dst{};

            for (i32 sqIdx = 0; sqIdx < Squares::kCount; ++sqIdx) {
                const auto sq = Square::fromRaw(sqIdx);
                auto& sqData = dst.squares[sq.idx()];

                for (const auto dir : {kDirs...}) {
                    const auto attacks = attacks::internal::generateSlidingAttacks(sq, dir, Bitboards::kEmpty);
                    sqData.mask |= attacks & ~attacks::internal::edges(dir);
                }

                sqData.offset = dst.tableSize;
                sqData.shift = std::popcount(static_cast<u64>(sqData.mask.raw()));

                dst.tableSize += 1 << sqData.mask.popcount();
            }

            return dst;
        }
    } // namespace internal

    constexpr std::array<internal::PieceData, Colors::kCount> kLanceData = {
        internal::generatePieceData<offsets::kNorth>(), // black
        internal::generatePieceData<offsets::kSouth>(), // white
    };

    static_assert(kLanceData[0].tableSize == kLanceData[1].tableSize);

    [[nodiscard]] constexpr const internal::PieceData& lanceData(Color c) {
        assert(c);
        return kLanceData[c.idx()];
    }

    constexpr usize kLanceDataTableSize = kLanceData[0].tableSize;

    constexpr auto kBishopData =
        internal::generatePieceData<offsets::kNorthWest, offsets::kNorthEast, offsets::kSouthWest, offsets::kSouthEast>(
        );

    constexpr auto kRookData =
        internal::generatePieceData<offsets::kNorth, offsets::kSouth, offsets::kWest, offsets::kEast>();
} // namespace stoat::attacks::pext
