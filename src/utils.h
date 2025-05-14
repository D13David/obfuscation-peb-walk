#pragma once

#if defined(_MSC_VER)
#define NO_OPTIMIZE_BEGIN __pragma(optimize("", off))
#define NO_OPTIMIZE_END   __pragma(optimize("", on))
#define NO_OPTIMIZE_ATTR
#elif defined(__clang__) || defined(__GNUC__)
#define NO_OPTIMIZE_BEGIN
#define NO_OPTIMIZE_END
#define NO_OPTIMIZE_ATTR __attribute__((optimize("O0")))
#else
#define NO_OPTIMIZE_BEGIN
#define NO_OPTIMIZE_END
#define NO_OPTIMIZE_ATTR
#endif

constexpr unsigned int FNV1A_SEED = 0x811c9dc5u;

//------------------------------------------------------------------------------
// Computes a 32-bit FNV-1a hash for a given string at compile time.
//------------------------------------------------------------------------------
constexpr unsigned int fnv1a32(const char* str, size_t length, unsigned int seed)
{
    unsigned int hash = seed;
    for (size_t i = 0; i < length; ++i)
    {
        hash ^= static_cast<unsigned int>(str[i]);
        hash *= 0x01000193u;
    }
    return hash;
}

//------------------------------------------------------------------------------
// Template overload of fnv1a32 that deduces the size of the input string.
// This version allows compile-time string hashing by inferring length.
//------------------------------------------------------------------------------
template<size_t N>
constexpr unsigned int fnv1a32(const char(&str)[N], unsigned int seed = FNV1A_SEED)
{
    return fnv1a32(str, N - 1, seed);
}

//------------------------------------------------------------------------------
// Combines the FNV-1a hashes of two strings using XOR.
// This can be used to create a derived seed from two identifiers.
//------------------------------------------------------------------------------
template<size_t N, size_t M>
constexpr unsigned int seed(const char(&s1)[N], const char(&s2)[M])
{
    return fnv1a32(s1) ^ fnv1a32(s2);
}

//------------------------------------------------------------------------------
// Applies the xorshift32 pseudo-random number generator to the input state.
// This is a simple PRNG useful for generating deterministic, reproducible numbers.
// Parameters:
//   - state: the initial value/state for the generator
// Returns:
//   - a pseudo-random value in the range [0, 0x7fffffff)
//------------------------------------------------------------------------------
constexpr unsigned int xorshift(unsigned int state) 
{
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state % 0x7fffffff;
}

//------------------------------------------------------------------------------
// A compile-time structure that stores an obfuscated version of the input string.
//------------------------------------------------------------------------------
template <unsigned int N>
struct ObfusctedString
{
    char Data[N]{};
    unsigned int Seed{};

    constexpr ObfusctedString(const char* data, unsigned int seed) 
    : Seed(seed)
    {
        for (unsigned int i = 0, rnd = seed; i < N; ++i)
        {
            Data[i] = data[i] ^ rnd;
            rnd = xorshift(rnd);
        }
    };
};

NO_OPTIMIZE_BEGIN
//------------------------------------------------------------------------------
// Encrypts or decrypts a string using XOR with a pseudo-random number sequence.
// The same function works for both encryption and decryption.
//------------------------------------------------------------------------------
inline const char* RecoverString(unsigned int seed, const char* src, char* dst, unsigned int size) NO_OPTIMIZE_ATTR
{
    for (unsigned int index = 0, rnd = seed; index < size; index++)
    {
        dst[index] = src[index] ^ rnd;
        rnd = xorshift(rnd);
    }
    return dst;
}
NO_OPTIMIZE_END

//------------------------------------------------------------------------------
// Macro for secure string obfuscation.
// Usage:
//   const char* myString = _S("Secret!");
//
// How it works:
//   - Obfuscates the string at compile time using a seed based on file and time.
//   - Decrypts the string at runtime using the same seed.
//   - Returns a pointer to the decrypted string stored in a static buffer.
//------------------------------------------------------------------------------
#define _S(str)                                                                                         \
    []() -> const char* {                                                                               \
        constexpr auto N = sizeof(str);                                                                 \
        constexpr auto Blob = ObfusctedString<N>(str, seed(__FILE__, __TIME__)); \
        static char buffer[N];                                                                          \
        return RecoverString(Blob.Seed, Blob.Data, buffer, N);                             \
    }()