#pragma once

constexpr unsigned int FNV1A_SEED = 0x811c9dc5u;

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

template<size_t N>
constexpr unsigned int fnv1a32(const char(&str)[N], unsigned int seed = FNV1A_SEED)
{
    return fnv1a32(str, N - 1, seed);
}

template<size_t N, size_t M>
constexpr unsigned int seed(const char(&s1)[N], const char(&s2)[M])
{
    return fnv1a32(s1) ^ fnv1a32(s2);
}

constexpr unsigned int xorshift(unsigned int state) 
{
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state % 0x7fffffff;
}

#pragma optimize("", off)
constexpr const char* EncryptDecrypt(unsigned int seed, const char* src, char* dst, unsigned int size)
{
    for (unsigned int index = 0, rnd = seed; index < size; index++)
    {
        dst[index] = src[index] ^ rnd;
        rnd = xorshift(rnd);
    }
    return dst;
}
#pragma optimize("", on)

template <unsigned int N>
struct ObfusctedString
{
    char Data[N]{};
    unsigned int Seed{};

    constexpr ObfusctedString(const char* data, unsigned int seed) 
    : Seed(seed)
    {
        EncryptDecrypt(seed, data, Data, N);
    };
};

#define ObfuscatedString(str)                                                                           \
    []() -> const char* {                                                                               \
        constexpr auto N = sizeof(str);                                                                 \
        constexpr auto Blob = ObfusctedString<N>(str, seed(__FILE__, __TIME__)); \
        static char buffer[N];                                                                          \
        return EncryptDecrypt(Blob.Seed, Blob.Data, buffer, N);                            \
    }()