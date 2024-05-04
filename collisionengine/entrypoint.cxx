#pragma region  Imports

#pragma region std

#include <iostream>
#include <vector>
#include <thread>
#include <random>

#pragma endregion

#pragma region Boost

#include <boost/crc.hpp>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion


/*
8-bit unsigned integer (unsigned char): Maximum value is 255
16-bit unsigned integer (unsigned short): Maximum value is 65,535
32-bit unsigned integer (unsigned int): Maximum value is 4,294,967,295
64-bit unsigned integer (unsigned long long): Maximum value is 18,446,744,073,709,551,615
*/

#pragma region Type Constants

#define UCHAR_MAX 0xFFui8   //  8-bit unsigned integral MAXIMUM

#define USHORT_MAX 0xFFFFui16    //  16-bit unsigned integral MAXIMUM

#define UINT_MAX 0xFFFFFFFFui32 //  32-bit unsigned integral MAXIMUM

#define UINT64_MAX 0xFFFFFFFFFFFFFFFFui64 //  64-bit unsigned integral MAXIMUM

#pragma endregion


#pragma region Globals

static void* data_ptr;
static size_t data_size;
static std::thread data_thread;

bool thread_state_flag = true;
#pragma endregion

__singleton __nothrow void __stackcall data_collision_thread() noexcept {

    bool hasreset = false; // if / when generated data sets exceeds 18446744073709551615 we need to know so dataset size does not drop down to lower size 

    size_t iterations = 0x1;

    while (thread_state_flag)
    {
        for (auto i = 0; i < data_size; ++i)
            reinterpret_cast<byte*>(data_ptr)[i] = static_cast<std::uint8_t>(NULL); // reset data

        for (auto i = 0; i < data_size; ++i) {
            for (auto x = 0; x < (UCHAR_MAX + 1); ++x) {
                if (i) {

                    auto preiter_passes = (i > 1 ? (i - 1) : 1);

                    for (auto y = 0; y < i; ++y) {

                        break;
                    }
                }
                else {
                    reinterpret_cast<byte*>(data_ptr)[x] = static_cast<byte>(x);
                }
            }
        }

        data_size = iterations;
        ++iterations;
    }
}

typedef struct collision_t {

    std::size_t index_x;
    std::size_t index_y;

    inline __fastcall collision_t(std::size_t xx, std::size_t yy) noexcept {

        index_x = std::move(xx);
        index_y = std::move(yy);
    }
};
__singleton __nothrow std::int32_t __stackcall main() noexcept {

    std::uint8_t ub = 0x0;
    std::uint16_t us = 0x0;
    std::uint32_t ui = 0x0;

    boost::crc_32_type crc32_inst;
    std::vector<std::uint32_t> hash_collection_CRC{};

    std::vector<std::uint32_t> hash_collection_32{};
    std::vector<std::uint64_t> hash_collection_64{};

#pragma region Algorithm(s) output generation

    std::cout << "[+] Generating hashes for all potential 16-bit data values..." << std::endl;

    for (auto i = 0; i <= USHORT_MAX; ++i) {

        const auto& hash32 = qhash::qhash32(&us, sizeof(us));

        const auto& hash64 = qhash::qhash64(&us, sizeof(us));

        hash_collection_32.push_back(hash32.hash);

        hash_collection_64.push_back(hash64.hash);

        crc32_inst.process_bytes(&us, sizeof(us));

        const std::uint32_t& crcsum = crc32_inst.checksum();

        hash_collection_CRC.push_back(crcsum);

        ++us;
    }

#pragma endregion

    std::size_t collision_counter32 = static_cast<std::size_t>(NULL);

    std::size_t collision_counter64 = static_cast<std::size_t>(NULL);

    std::size_t collision_counterCRC = static_cast<std::size_t>(NULL);

    std::vector<collision_t> collisions_detected_vector32{};

    std::vector<collision_t> collisions_detected_vector64{};

    std::vector<collision_t> collisions_detected_vectorCRC{};

    std::cout << "[+] Checking for hash collisions in all potential 16-bit values... " << std::endl;

    const auto collision_inlined_test = [

        &hash_collection_32,
            &hash_collection_64,
            &hash_collection_CRC,
            &collision_counter32,
            &collision_counter64,
            &collision_counterCRC,
            &collisions_detected_vector32,
            &collisions_detected_vector64,
            &collisions_detected_vectorCRC
    ](

        const std::int32_t collision_list_index,
        std::int32_t& ii,
        std::int32_t& xx
        ) noexcept {   //  0 = 32, 1 = 64, 2 = crc32

            switch (collision_list_index) {

            case 0: { // qhash32 list call

                if (hash_collection_32[ii] == hash_collection_32[xx]) {

                    bool is_collision_contained = false;

                    for (std::int32_t y = 0; y < collisions_detected_vector32.size(); ++y) {

                        if ((collisions_detected_vector32[y].index_x == ii && collisions_detected_vector32[y].index_y == xx) || (collisions_detected_vector32[y].index_x == xx && collisions_detected_vector32[y].index_y == ii)) {

                            is_collision_contained = true;
                            continue;
                        }
                    }
                    if (!is_collision_contained) {

                        collisions_detected_vector32.push_back(

                            collision_t(
                                ii,
                                xx
                            )
                        );

                        ++collision_counter32;
                    }
                }

                break;
            }

            case 1: {   // qhash64 list call

                if (hash_collection_64[ii] == hash_collection_64[xx]) {

                    bool is_collision_contained = false;

                    for (std::int32_t y = 0; y < collisions_detected_vector32.size(); ++y) {

                        if ((collisions_detected_vector64[y].index_x == ii && collisions_detected_vector64[y].index_y == xx) || (collisions_detected_vector64[y].index_x == xx && collisions_detected_vector64[y].index_y == ii)) {

                            is_collision_contained = true;
                            continue;
                        }
                    }
                    if (!is_collision_contained) {

                        collisions_detected_vector64.push_back(

                            collision_t(
                                ii,
                                xx
                            )
                        );

                        ++collision_counter64;
                    }
                }

                break;
            }

            case 2: {   //  CRC32 list call

                if (hash_collection_CRC[ii] == hash_collection_CRC[xx]) {

                    bool is_collision_contained = false;

                    for (std::int32_t y = 0; y < collisions_detected_vectorCRC.size(); ++y) {

                        if ((collisions_detected_vectorCRC[y].index_x == ii && collisions_detected_vectorCRC[y].index_y == xx) || (collisions_detected_vectorCRC[y].index_x == xx && collisions_detected_vectorCRC[y].index_y == ii)) {

                            is_collision_contained = true;
                            continue;
                        }
                    }
                    if (!is_collision_contained) {

                        collisions_detected_vectorCRC.push_back(

                            collision_t(
                                ii,
                                xx
                            )
                        );

                        ++collision_counterCRC;
                    }
                }

                break;
            }

            default: {
                break;
            }

            }

        };

#pragma region Algorithm output collision testing

        for (std::int32_t i = 0; i <= USHORT_MAX; ++i) {

            for (std::int32_t x = 0; x <= USHORT_MAX; ++x) {

                if (x == i) {   //  skip checking the current entry in the array against itself as this is illogical

                    continue;
                }

                for (auto w = 0; w < 3; ++w)
                    collision_inlined_test(w, i, x);

            }
        }

#pragma region Macros

#define SAVE_STDOUT_FLAGS( __obj__ ) __obj__ = std::cout.flags()

#define SET_STDOUT_INTEGRAL( __obj__ ) std::cout.flags(__obj__)

#define PRINT_HEXADECIMAL std::hex << std::noshowbase

#define STDOUT_PRINTBLOCK_SEPERATOR() for(std::int32_t __iter__ = 0; __iter__ < 3; ++__iter__)std::cout << "[--------------------------------------------------------------------------------------]" << std::endl;

#pragma endregion

        STDOUT_PRINTBLOCK_SEPERATOR();

        std::cout << "[+] Done, qhash32 collision counter: " << collision_counter32 << std::endl;

        std::int32_t iterator = static_cast<std::uint32_t>(NULL);

        std::ios_base::fmtflags std_flag;

        for (const auto& cdtc32 : collisions_detected_vector32) {

            SAVE_STDOUT_FLAGS(std_flag);

            std::cout << "[+] Detection# " << iterator << ": Hash value of 0x" << PRINT_HEXADECIMAL << hash_collection_32[cdtc32.index_x] << " was detected as a match between differential 16-bit origin data entries of value(s): 0x" << cdtc32.index_x << " and, 0x" << cdtc32.index_y << std::endl;

            SET_STDOUT_INTEGRAL(std_flag);

            ++iterator;
        }

        STDOUT_PRINTBLOCK_SEPERATOR();

        iterator = 0;   //  reset iterator

        std::cout << "[+] Done, qhash64 collision counter: " << collision_counter64 << std::endl;

        for (const auto& cdtc64 : collisions_detected_vector64) {

            SAVE_STDOUT_FLAGS(std_flag);

            std::cout << "[+] Detection# " << iterator << ": Hash value of 0x" << PRINT_HEXADECIMAL << hash_collection_32[cdtc64.index_x] << " was detected as a match between differential 16-bit origin data entries of value(s): 0x" << cdtc64.index_x << " and, 0x" << cdtc64.index_y << std::endl;

            SET_STDOUT_INTEGRAL(std_flag);

            ++iterator;
        }

        STDOUT_PRINTBLOCK_SEPERATOR();

        iterator = 0;   //  reset iterator

        std::cout << "[+] Done, CRC32 collision counter: " << collision_counterCRC << std::endl;

        for (const auto& cdtcCRC : collisions_detected_vectorCRC) {

            SAVE_STDOUT_FLAGS(std_flag);

            std::cout << "[+] Detection# " << iterator << ": Hash value of 0x" << PRINT_HEXADECIMAL << hash_collection_32[cdtcCRC.index_x] << " was detected as a match between differential 16-bit origin data entries of value(s): 0x" << cdtcCRC.index_x << " and, 0x" << cdtcCRC.index_y << std::endl;

            SET_STDOUT_INTEGRAL(std_flag);

            ++iterator;
        }

#pragma endregion

        std::cin.get();

        return 0;
}