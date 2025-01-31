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

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>

#include "bitboard.h"
#include "pext/pext.h"
#include "util/blocking_queue.h"
#include "util/rng.h"

using namespace stoat;

namespace {
    constexpr u64 kSeed = 69420;

    constexpr usize kAttempts = 10000000;
    constexpr i32 kThreads = 16;

    constexpr bool kBlackMagic = true;

    [[nodiscard]] usize getIdx(u128 mask, u128 occ, u128 magic, i32 shift) {
        if constexpr (kBlackMagic) {
            return static_cast<usize>(((occ | ~mask) * magic) >> shift);
        } else {
            return static_cast<usize>((occ * magic) >> shift);
        }
    }

    struct Magic {
        u128 magic;
        i32 shift;
    };

    [[nodiscard]] std::optional<u128> findMagic(Square sq, u128 mask, i32 bits, const auto& attackGetter) {
        assert(mask != 0);

        const auto count = 1 << bits;
        const auto shift = 128 - bits;

        util::rng::Jsf64Rng rng{kSeed};

        std::vector<Bitboard> used{};
        used.resize(count);

        for (usize i = 0; i < kAttempts; ++i) {
            bool failed = false;
            std::memset(used.data(), 0, used.size() * sizeof(Bitboard));

            const auto candidate = rng.nextU128() & rng.nextU128() & rng.nextU128();

            u128 occ = 0;
            while (true) {
                const auto idx = getIdx(mask, occ, candidate, shift);
                const Bitboard attacks = attackGetter(sq, Bitboard{occ});

                assert(!attacks.empty());

                if (used[idx].empty()) {
                    used[idx] = attacks;
                } else if (used[idx] != attacks) {
                    failed = true;
                    break;
                }

                occ = (occ - mask) & mask;
                if (occ == 0) {
                    break;
                }
            }

            if (!failed) {
                return candidate;
            }
        }

        return {};
    }

    std::optional<Magic> findOptimalMagic(Square sq, Bitboard mask, const auto& attackGetter) {
        assert(!mask.empty());

        std::optional<Magic> best{};

        for (i32 bits = mask.popcount(); bits > 0; --bits) {
            if (const auto magic = findMagic(sq, mask.raw(), bits, attackGetter)) {
                best = Magic{
                    .magic = *magic,
                    .shift = 128 - bits,
                };
            } else {
                break;
            }
        }

        return best;
    }

    void findMagics(
        std::string_view piece,
        const attacks::pext::internal::PieceData& data,
        const auto& attackGetter,
        Bitboard allowed = Bitboards::kAll
    ) {
        std::array<Magic, Squares::kCount> magics{};
        std::mutex magicMutex{};

        util::BlockingQueue<Square> queue{};

        std::vector<std::thread> threads{};
        threads.reserve(kThreads);

        for (i32 i = 0; i < kThreads; ++i) {
            threads.emplace_back([&] {
                while (true) {
                    const auto sq = queue.wait();

                    if (!sq) {
                        break;
                    }

                    const auto& sqData = data.squares[sq.idx()];

                    if (const auto magic = findOptimalMagic(sq, sqData.mask, attackGetter)) {
                        const std::scoped_lock lock{magicMutex};
                        std::cout << "found " << piece << " magic for " << sq << " with shift " << magic->shift
                                  << std::endl;
                        magics[sq.idx()] = *magic;
                    } else {
                        const std::scoped_lock lock{magicMutex};
                        std::cerr << "failed to find " << piece << " magic for square " << sq << std::endl;
                    }
                }
            });
        }

        for (i32 sqIdx = 0; sqIdx < Squares::kCount; ++sqIdx) {
            const auto sq = Square::fromRaw(sqIdx);

            if (!allowed.getSquare(sq)) {
                continue;
            }

            queue.push(sq);
        }

        for (i32 i = 0; i < kThreads; ++i) {
            queue.push(Squares::kNone);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        for (i32 sqIdx = 0; sqIdx < Squares::kCount; ++sqIdx) {
            const auto sq = Square::fromRaw(sqIdx);
            if (magics[sq.idx()].magic == 0 && allowed.getSquare(sq)) {
                return;
            }
        }

        std::ofstream stream{std::string{piece} + (kBlackMagic ? "_black" : "_white") + "_magic.txt", std::ios::binary};

        stream << "constexpr std::array k" << piece << "Shifts = {";

        for (i32 rank = 0; rank < 9; ++rank) {
            stream << "\n   ";
            for (i32 file = 0; file < 9; ++file) {
                const auto sq = Square::fromFileRank(file, rank);
                stream << ' ' << magics[sq.idx()].shift << ',';
            }
        }

        stream << "\n};\n\n";

        stream << std::hex;
        stream << "constexpr std::array k" << piece << "Magics = {";

        for (i32 sqIdx = 0; sqIdx < Squares::kCount; ++sqIdx) {
            const auto sq = Square::fromRaw(sqIdx);

            const u64 high = magics[sq.idx()].magic >> 64;
            const u64 low = magics[sq.idx()].magic;

            stream << "\n    U128(0x" << high << ", 0x" << low << "),";
        }

        stream << "\n};\n";

        std::cout << "wrote out " << piece << " magics" << std::endl;
    }
} // namespace

int main() {
    findMagics(
        "BlackLance",
        attacks::pext::lanceData(Colors::kBlack),
        [](Square sq, Bitboard occ) { return attacks::lanceAttacks(sq, Colors::kBlack, occ); },
        ~(Bitboards::kRankA | Bitboards::kRankB)
    );

    findMagics(
        "WhiteLance",
        attacks::pext::lanceData(Colors::kWhite),
        [](Square sq, Bitboard occ) { return attacks::lanceAttacks(sq, Colors::kWhite, occ); },
        ~(Bitboards::kRankI | Bitboards::kRankH)
    );

    findMagics("Bishop", attacks::pext::kBishopData, attacks::bishopAttacks);
    findMagics("Rook", attacks::pext::kRookData, attacks::rookAttacks);
}
