#include "cwg_prng.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

int main(int argc, char* argv[])
{
    const std::uint64_t seed = argc > 1 ? std::stoull(argv[1]) : 1ULL;

#if defined(__SIZEOF_INT128__)
    cwg::Cwg128_64 rng(seed);

    std::cout << "CWG128-64 samples seeded with " << seed << ":\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << std::setw(2) << i << ": " << rng() << '\n';
    }

    std::uniform_int_distribution<int> die(1, 6);
    std::cout << "\nCWG128-64 die rolls:\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << die(rng) << (i == 9 ? '\n' : ' ');
    }

    cwg::Cwg128 rng128(seed);
    std::cout << "\nCWG128 128-bit samples:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << std::setw(2) << i << ": " << cwg::uint128_to_hex(rng128()) << '\n';
    }
#else
    std::cout << "\nCWG128 variants require compiler support for unsigned __int128.\n";
#endif

    return 0;
}
