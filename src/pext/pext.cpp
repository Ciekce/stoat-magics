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

#include "pext.h"

namespace stoat::attacks::pext {
    namespace {
        template <const internal::PieceData& kData, i32... kDirs>
        std::array<Bitboard, kData.tableSize> generateAttacks() {
            std::array<Bitboard, kData.tableSize> dst{};

            for (i32 sqIdx = 0; sqIdx < Squares::kCount; ++sqIdx) {
                const auto sq = Square::fromRaw(sqIdx);
                const auto& sqData = kData.squares[sq.idx()];

                const auto entries = 1 << sqData.mask.popcount();

                for (i32 i = 0; i < entries; ++i) {
                    const auto occ = Bitboard{util::pdep(i, sqData.mask.raw())};

                    auto& attacks = dst[sqData.offset + i];

                    for (const auto dir : {kDirs...}) {
                        attacks |= attacks::internal::generateSlidingAttacks(sq, dir, occ);
                    }
                }
            }

            return dst;
        }
    } // namespace

    const util::MultiArray<Bitboard, Colors::kCount, kLanceDataTableSize> g_lanceAttacks = {
        generateAttacks<lanceData(Colors::kBlack), offsets::kNorth>(),
        generateAttacks<lanceData(Colors::kWhite), offsets::kSouth>(),
    };

    const std::array<Bitboard, kBishopData.tableSize> g_bishopAttacks = generateAttacks<
        kBishopData,
        offsets::kNorthWest,
        offsets::kNorthEast,
        offsets::kSouthWest,
        offsets::kSouthEast>();

    const std::array<Bitboard, kRookData.tableSize> g_rookAttacks =
        generateAttacks<kRookData, offsets::kNorth, offsets::kSouth, offsets::kWest, offsets::kEast>();
} // namespace stoat::attacks::sliders::pext
