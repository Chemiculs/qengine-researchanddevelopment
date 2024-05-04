#ifndef QDEF_H
#define QDEF_H

//#include <functional>
#include <tuple>
#include <string>

#ifdef _MSC_VER
#define WINDOWS_IGNORE_PACKING_MISMATCH
#endif

#pragma region Constants

#define NULL 0x0

#pragma endregion


#pragma region Macros

#define RAND(high, low) rand() % high + low \

#pragma region Memory

#define XORWORD(word, xval) for(auto m = 0; m < sizeof(decltype(xval)); ++m) word ^= reinterpret_cast<unsigned char*>(&xval)[m] \

#define RtlZeroMemory(block, size) (memset(block, 0x00ui8, size)) \

#define UNIQUE(param_) std::unique_ptr<unsigned char[], crypto::e_malloc::free_delete_t>(reinterpret_cast<unsigned char*>(param_)) \

#pragma endregion

#pragma region Template Functions

    /* scrambled comparison operation */
template<typename T, typename T2>
__forceinline bool __fastcall compare_memory(T && word1, T2 && word2) {

    bool same = true;

    if (sizeof(decltype(word1)) != sizeof(decltype(word2))) {

        /* align bits of smaller word type to the larger one */
        decltype(sizeof(decltype(word1)) > sizeof(decltype(word2)) ? word1 : word2) word_c = (sizeof(decltype(word1)) > sizeof(decltype(word2)) ? static_cast<decltype(word_c)>(word2) : static_cast<decltype(word_c)>(word1));

        if (sizeof(decltype(word1)) > sizeof(decltype(word2)))
            for (auto i = 0; i < sizeof(decltype(word1)); ++i)
                if (same)
                    same = (reinterpret_cast<byte*>(&word1)[i] == reinterpret_cast<byte*>(&word_c)[i]);
                else
                    continue;
        else
            for (auto i = 0; i < sizeof(decltype(word2)); ++i)
                if (same)
                    same = (reinterpret_cast<byte*>(&word2)[i] == reinterpret_cast<byte*>(&word_c)[i]);
                else
                    continue;
    }
    else {
        for (auto i = 0; i < sizeof(decltype(word1)); ++i)
            if (same)
                same = (reinterpret_cast<byte*>(&word1)[i] == reinterpret_cast<byte*>(&word2)[i]);
            else
                continue;
    }

    return same;
}

__forceinline bool __cdecl compare_string(std::string string1, std::string string2) {
    if (string1.size() != string2.size())
        return false;

    for (auto i = 0; i < string1.size(); ++i)
        if (!compare_memory(string1[i], string2[i]))
            return false;

    return true;
}

__forceinline bool __cdecl compare_wstring(std::wstring wstring1, std::wstring wstring2) {
    if (wstring1.size() != wstring2.size())
        return false;

    for (auto i = 0; i < wstring1.size(); ++i)
        if (!compare_memory(wstring1[i], wstring2[i]))
            return false;

    return true;
}

#pragma endregion

#pragma region Xor Operations Floating Point

// workaround for lack of support for bitwise operations on floating-point types by the C++ standard library
// ensure the type of both arguments is the same. you must declare the argument before passing it, or use  a typecast or the apropriate declaration suffix (e.g 255ULL to declare an unsigned long long with a value of 255 so it won't default to int / short)
#ifdef __clang__
#define XOR_FORCE(val, xor_val) \
    ({ \
        auto _return = (val); \
        auto xor_val_c = (xor_val); \
        auto val_ptr = reinterpret_cast<unsigned char*>(const_cast<decltype(_return)*>(&(_return))); \
        auto xor_ptr = reinterpret_cast<unsigned char*>(const_cast<decltype(xor_val_c)*>(&(xor_val_c))); \
        for(auto x = 0; x < sizeof(decltype(_return)); ++x){ \
            val_ptr[x] ^= xor_ptr[x]; \
        } \
        _return;\
    }) 

#elif defined (_MSC_VER)

template<typename T>
__forceinline decltype(auto) XOR_FORCE(T val, T xor_val) {
    T _return = val;
    auto val_ptr = reinterpret_cast<unsigned char*>(&_return);
    auto xor_ptr = reinterpret_cast<unsigned char*>(&xor_val);
    for (auto x = 0; x < sizeof(T); ++x) {
        val_ptr[x] ^= xor_ptr[x];
    }
    return _return;
}

#endif

#pragma endregion

#pragma endregion



#endif