# Collatz

This repository contains a small C++ implementation of Collatz-Weyl
pseudorandom number generators based on the 128-bit variants described in the
Collatz-Weyl generator paper.

The implementation is header-only. The example program shows how to use the
generator like a standard C++ PRNG engine.

## Files

- `cwg_prng.hpp`: header-only PRNG implementation.
- `cwg_example.cpp`: example program that prints sample values and uses the
  generator with `std::uniform_int_distribution`.

The header provides:

- `cwg::SplitMix`: SplitMix helper used for seeding.
- `cwg::Cwg128_64`: 128-bit internal state with 64-bit output. This is the
  easiest engine to use with standard C++ distributions.
- `cwg::Cwg128`: 128-bit output generator.
- `cwg::uint128_to_hex`: helper for printing `unsigned __int128` values.

CWG64 is intentionally not included.

## Requirements

Use a compiler that supports `unsigned __int128`, such as GCC or Clang.

The example uses C++17:

```bash
g++ -std=c++17 -O2 cwg_example.cpp -o cwg_example
```

Run it with the default seed:

```bash
./cwg_example
```

Or provide a seed:

```bash
./cwg_example 12345
```

## Basic Usage

Include the header and create a `cwg::Cwg128_64` engine:

```cpp
#include "cwg_prng.hpp"

#include <cstdint>
#include <iostream>
#include <random>

int main()
{
    cwg::Cwg128_64 rng(12345);

    std::uint64_t value = rng();
    std::cout << value << '\n';

    std::uniform_int_distribution<int> die(1, 6);
    std::cout << die(rng) << '\n';
}
```

`Cwg128_64` exposes the standard PRNG interface:

```cpp
cwg::Cwg128_64 rng(seed);
rng();
rng.next();
rng.discard(1000);
cwg::Cwg128_64::min();
cwg::Cwg128_64::max();
```

You can also pass an explicit odd Weyl increment:

```cpp
cwg::Cwg128_64 rng(12345, 0x9e3779b97f4a7c15ULL);
```

The increment must be odd.

## 128-Bit Output

Use `cwg::Cwg128` when you want 128-bit output:

```cpp
cwg::Cwg128 rng(12345);
cwg::uint128 value = rng();
std::cout << cwg::uint128_to_hex(value) << '\n';
```

For convenience, `Cwg128` also has `next()` when you only need the low 64 bits:

```cpp
std::uint64_t low64 = rng.next();
```

## Notes

This code is intended for experimentation with Collatz-Weyl generators. Do not
use it for cryptography unless you perform independent statistical and security
validation for your application.
