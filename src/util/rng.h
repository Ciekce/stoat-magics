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

#include <bit>
#include <limits>
#include <random>

namespace stoat::util::rng {
    class Jsf64Rng {
    public:
        explicit constexpr Jsf64Rng(u64 seed) :
                m_b{seed}, m_c{seed}, m_d{seed} {
            for (i32 i = 0; i < 20; ++i) {
                nextU64();
            }
        }

        constexpr u64 nextU64() {
            const auto e = m_a - std::rotl(m_b, 7);
            m_a = m_b ^ std::rotl(m_c, 13);
            m_b = m_c + std::rotl(m_d, 37);
            m_c = m_d + e;
            m_d = e + m_a;
            return m_d;
        }

        [[nodiscard]] constexpr u128 nextU128() {
            const auto high = nextU64();
            const auto low = nextU64();
            return toU128(high, low);
        }

        [[nodiscard]] constexpr u32 nextU32() {
            return static_cast<u32>(nextU64() >> 32);
        }

        [[nodiscard]] constexpr u32 nextU32(u32 bound) {
            if (bound == 0) {
                return 0;
            }

            auto x = nextU32();
            auto m = static_cast<u64>(x) * static_cast<u64>(bound);
            auto l = static_cast<u32>(m);

            if (l < bound) {
                auto t = -bound;

                if (t >= bound) {
                    t -= bound;

                    if (t >= bound) {
                        t %= bound;
                    }
                }

                while (l < t) {
                    x = nextU32();
                    m = static_cast<u64>(x) * static_cast<u64>(bound);
                    l = static_cast<u32>(m);
                }
            }

            return static_cast<u32>(m >> 32);
        }

    private:
        u64 m_a{0xf1ea5eed};
        u64 m_b;
        u64 m_c;
        u64 m_d;
    };

    inline auto generateSingleSeed() {
        std::random_device generator{};
        return static_cast<u64>(generator()) << 32 | generator();
    }

    // splitmix64, suitable for seeding jsf64
    class SeedGenerator {
    public:
        explicit SeedGenerator(u64 seed = generateSingleSeed()) :
                m_state{seed} {}

        constexpr u64 nextSeed() {
            m_state += UINT64_C(0x9e3779b97f4a7c15);

            auto z = m_state;

            z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
            z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);

            return z ^ (z >> 31);
        }

    private:
        u64 m_state{};
    };
} // namespace stoat::util::rng
