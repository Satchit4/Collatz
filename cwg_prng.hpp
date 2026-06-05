#ifndef CWG_PRNG_HPP
#define CWG_PRNG_HPP

#include <cstdint>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace cwg {

class SplitMix {
public:
    using result_type = std::uint64_t;

    explicit SplitMix(std::uint64_t seed = 0) : state_(seed) {}

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    result_type operator()() { return next64(); }

    result_type next64()
    {
        std::uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    result_type next63()
    {
        std::uint64_t z = (state_ += 0x9e3779b97f4a7c15ULL) & 0x7fffffffffffffffULL;
        z = ((z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL) & 0x7fffffffffffffffULL;
        z = ((z ^ (z >> 27)) * 0x94d049bb133111ebULL) & 0x7fffffffffffffffULL;
        return z ^ (z >> 31);
    }

private:
    std::uint64_t state_;
};

#if defined(__SIZEOF_INT128__)
using uint128 = unsigned __int128;

inline std::string uint128_to_hex(uint128 value)
{
    static constexpr char digits[] = "0123456789abcdef";

    std::string out(34, '0');
    out[0] = '0';
    out[1] = 'x';

    for (int i = 33; i >= 2; --i) {
        out[static_cast<std::size_t>(i)] = digits[static_cast<unsigned>(value & 0xfU)];
        value >>= 4;
    }

    return out;
}

class Cwg128_64 {
public:
    using result_type = std::uint64_t;

    explicit Cwg128_64(std::uint64_t seed_value = 1) { seed(seed_value); }

    Cwg128_64(std::uint64_t seed_value, std::uint64_t odd_weyl_increment)
    {
        seed(seed_value, odd_weyl_increment);
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    void seed(std::uint64_t seed_value = 1)
    {
        SplitMix seeder(seed_value);
        x_ = (static_cast<uint128>(seeder.next64()) << 64) | seeder.next64();
        a_ = 0;
        weyl_ = 0;
        s_ = (seeder.next63() << 1) | 1ULL;
        warmup();
    }

    void seed(std::uint64_t seed_value, std::uint64_t odd_weyl_increment)
    {
        if ((odd_weyl_increment & 1ULL) == 0) {
            throw std::invalid_argument("CWG Weyl increment must be odd.");
        }

        SplitMix seeder(seed_value);
        x_ = (static_cast<uint128>(seeder.next64()) << 64) | seeder.next64();
        a_ = 0;
        weyl_ = 0;
        s_ = odd_weyl_increment;
        warmup();
    }

    result_type operator()() { return next(); }

    uint128 CWG128_64() { return next_u128(); }
    void CWG128_64_initializer() { warmup(); }

    result_type next()
    {
        return static_cast<std::uint64_t>(next_u128());
    }

    uint128 next_u128()
    {
        x_ = (x_ | uint128{1}) * ((a_ += static_cast<std::uint64_t>(x_)) >> 1)
            ^ (weyl_ += s_);
        return static_cast<uint128>(a_ >> 48) ^ x_;
    }

    void discard(std::uint64_t count)
    {
        for (std::uint64_t i = 0; i < count; ++i) {
            next_u128();
        }
    }

private:
    void warmup() { discard(48); }

    uint128 x_ = 0;
    std::uint64_t a_ = 0;
    std::uint64_t weyl_ = 0;
    std::uint64_t s_ = 1;
};

class Cwg128 {
public:
    using result_type = uint128;

    explicit Cwg128(std::uint64_t seed_value = 1) { seed(seed_value); }

    Cwg128(std::uint64_t seed_value, uint128 odd_weyl_increment)
    {
        seed(seed_value, odd_weyl_increment);
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~uint128{0}; }

    void seed(std::uint64_t seed_value = 1)
    {
        SplitMix seeder(seed_value);
        c_[1] = seeder.next64();
        c_[0] = (static_cast<uint128>(seeder.next64()) << 64)
            | ((static_cast<uint128>(seeder.next63()) << 1) | uint128{1});
        c_[2] = 0;
        c_[3] = 0;
        warmup();
    }

    void seed(std::uint64_t seed_value, uint128 odd_weyl_increment)
    {
        if ((odd_weyl_increment & uint128{1}) == 0) {
            throw std::invalid_argument("CWG Weyl increment must be odd.");
        }

        SplitMix seeder(seed_value);
        c_[0] = odd_weyl_increment;
        c_[1] = seeder.next64();
        c_[2] = 0;
        c_[3] = 0;
        warmup();
    }

    result_type operator()() { return next_u128(); }

    uint128 CWG128() { return next_u128(); }
    void CWG128_initializer() { warmup(); }

    std::uint64_t next()
    {
        return static_cast<std::uint64_t>(next_u128());
    }

    uint128 next_u128()
    {
        c_[1] = (c_[1] >> 1) * ((c_[2] += c_[1]) | uint128{1}) ^ (c_[3] += c_[0]);
        return (c_[2] >> 96) ^ c_[1];
    }

    void discard(std::uint64_t count)
    {
        for (std::uint64_t i = 0; i < count; ++i) {
            next_u128();
        }
    }

private:
    void warmup() { discard(96); }

    uint128 c_[4] = {1, 0, 0, 0};
};
#endif

} // namespace cwg

#endif
