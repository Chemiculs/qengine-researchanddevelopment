#ifndef HASH_T_H
#define HASH_T_H

/* TODO: make a thread pooling system to limit thread count, make each thread cover X amount of data  */

#pragma region Imports

#include <string>

#include "../qmem.hpp"

#pragma endregion

#pragma region Preprocessor 

#pragma region SSE / AVX

#ifdef __AVX__
// fix for an msvc bug where __SSE__ preprocessor refuses to list as active when it should be as avx indicates this features presence as well

#ifndef __SSE__

#define __SSE__

#endif

#ifndef __SSE2__

#define __SSE2__

#endif

#endif

#ifdef __SSE__

#ifdef __clang__

#include <emmintrin.h>

#elif defined(_MSC_VER)

#include <xmmintrin.h> 

#endif

#endif

#ifdef __AVX__

#ifdef __clang__

#include <avxintrin.h>

#elif defined(_MSC_VER)

#include <immintrin.h> 

#endif

#endif

#pragma endregion

#pragma endregion

namespace qhash_t {

    /*
   /***************************************************************************************************************
   *																											 *
   *  QHash_T is the lower wrapper of the polymorphic type engine which ensures secure data access and integrity *
   *																											 *
   ****************************************************************************************************************
   */


#pragma region Globals


    extern qcallback::qmem_exception_rogue_c violation_callback;

    extern qmem::qwatcher* qwatch_threads;

    extern bool thread_switch;

#pragma endregion

#pragma region Type Definitions

    typedef struct qmemory_data {
        /* index inside the threads data queue */
        uintptr_t data_queue_index;
        /* index of the thread to which this data is assigned */
        bool thread_index;

        __forceinline __fastcall qmemory_data(uintptr_t data_queue_index_, bool thread_index_) : data_queue_index(data_queue_index_), thread_index(thread_index_) { }

        qmemory_data() {}
    };

#pragma endregion

#pragma region Global Static Methods / Callbacks

    static __forceinline void __fastcall violation_callback_d(qexcept::q_rogueaccess except, void* data) {
        //default callback for security breaches (do nothing?)

        if (except.id != qexcept::MEMORY_ALTERATION) // ensure this callback has been raised due to memory alteration
            return;
    }

    static __forceinline void __fastcall init_hash_t(qcallback::qmem_exception_rogue_c&& callback = violation_callback_d) {
        violation_callback = callback;

        qwatch_threads = new qmem::qwatcher[2]{ qmem::qwatcher(violation_callback) , qmem::qwatcher(violation_callback) };
    }

    static __forceinline qmemory_data __fastcall schedule_memory_work(void* data, size_t size) {
        thread_switch = thread_switch ? false : true; // reverse the switch 

        auto index = qwatch_threads[thread_switch].append_memory_region(data, size);

        return qmemory_data(index, thread_switch);
    }

#pragma endregion

#pragma region Types

#pragma region WORD

#pragma region 8-bit

    class h_int8 {

    private:

#pragma region Globals

        char _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_int8(char value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_int8(const h_int8& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline char __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(char value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }


        __forceinline decltype(_value)* get_raw_memory_address() {
            return &_value;
        }

#pragma endregion

        void illegal_set(char value) { _value = value; }

#pragma region Operators

        __forceinline h_int8 __fastcall operator+(const char value) const {
            return h_int8(get() + value);
        };

        __forceinline h_int8 __fastcall operator-(const char value) const {
            return h_int8(get() - value);
        }

        __forceinline h_int8 __fastcall operator/(const char value) const {
            return h_int8(get() / value);
        }

        __forceinline h_int8 __fastcall operator*(const char value) const {
            return h_int8(get() * value);
        }

        __forceinline h_int8 __fastcall operator&(const char value) const {
            return h_int8(get() & value);
        }

        __forceinline h_int8 __fastcall operator|(const char value) const {
            return h_int8(get() | value);
        }

        __forceinline h_int8 __fastcall operator%(const char value) const {
            return h_int8(get() % value);
        }

        __forceinline h_int8 __fastcall operator^(const char value) const {
            return h_int8(get() ^ value);
        }

        __forceinline h_int8 __fastcall operator<<(const char value) const {
            return h_int8(get() << value);
        }

        __forceinline h_int8 __fastcall operator>>(const char value) const {
            return h_int8(get() >> value);
        }

        __forceinline h_int8& __fastcall operator+=(const char value) {
            set(static_cast<char>(get() + value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator-=(const char value) {
            set(static_cast<char>(get() - value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_int8& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_int8& __fastcall operator*=(const char value) {
            set(static_cast<char>(get() * value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator/=(const char value) {
            set(static_cast<char>(get() / value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator%=(const char value) {
            set(static_cast<char>(get() % value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator^=(const char value) {
            set(static_cast<char>(get() ^ value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator&=(const char value) {
            set(static_cast<char>(get() & value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator|=(const char value) {
            set(static_cast<char>(get() | value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator<<=(const char value) {
            set(static_cast<char>(get() << value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator>>=(const char value) {
            set(static_cast<char>(get() >> value));
            return *this;
        }

        __forceinline h_int8& __fastcall operator=(const char value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator char() const {
            return get();
        }

        __forceinline ~h_int8() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_uint8 {

    private:

#pragma region Globals

        byte _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_uint8(byte value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_uint8(const h_uint8& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline byte __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(byte value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_uint8 __fastcall operator+(const byte value) const {
            return h_uint8(get() + value);
        };

        __forceinline h_uint8 __fastcall operator-(const byte value) const {
            return h_uint8(get() - value);
        }

        __forceinline h_uint8 __fastcall operator/(const byte value) const {
            return h_uint8(get() / value);
        }

        __forceinline h_uint8 __fastcall operator*(const byte value) const {
            return h_uint8(get() * value);
        }

        __forceinline h_uint8 __fastcall operator&(const byte value) const {
            return h_uint8(get() & value);
        }

        __forceinline h_uint8 __fastcall operator|(const byte value) const {
            return h_uint8(get() | value);
        }

        __forceinline h_uint8 __fastcall operator%(const byte value) const {
            return h_uint8(get() % value);
        }

        __forceinline h_uint8 __fastcall operator^(const byte value) const {
            return h_uint8(get() ^ value);
        }

        __forceinline h_uint8 __fastcall operator<<(const byte value) const {
            return h_uint8(get() << value);
        }

        __forceinline h_uint8 __fastcall operator>>(const byte value) const {
            return h_uint8(get() >> value);
        }

        __forceinline h_uint8& __fastcall operator+=(const byte value) {
            set(static_cast<byte>(get() + value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator-=(const byte value) {
            set(static_cast<byte>(get() - value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_uint8& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_uint8& __fastcall operator*=(const byte value) {
            set(static_cast<byte>(get() * value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator/=(const byte value) {
            set(static_cast<byte>(get() / value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator%=(const byte value) {
            set(static_cast<byte>(get() % value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator^=(const byte value) {
            set(static_cast<byte>(get() ^ value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator&=(const byte value) {
            set(static_cast<byte>(get() & value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator|=(const byte value) {
            set(static_cast<byte>(get() | value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator<<=(const byte value) {
            set(static_cast<byte>(get() << value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator>>=(const byte value) {
            set(static_cast<byte>(get() >> value));
            return *this;
        }

        __forceinline h_uint8& __fastcall operator=(const byte value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator byte() const {
            return get();
        }

        __forceinline ~h_uint8() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#pragma endregion

#pragma region 16-bit

    class h_int16 {

    private:

#pragma region Globals

        short _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_int16(short value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_int16(const h_int16& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline short __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(short value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_int16 __fastcall operator+(const short value) const {
            return h_int16(get() + value);
        };

        __forceinline h_int16 __fastcall operator-(const short value) const {
            return h_int16(get() - value);
        }

        __forceinline h_int16 __fastcall operator/(const short value) const {
            return h_int16(get() / value);
        }

        __forceinline h_int16 __fastcall operator*(const short value) const {
            return h_int16(get() * value);
        }

        __forceinline h_int16 __fastcall operator&(const short value) const {
            return h_int16(get() & value);
        }

        __forceinline h_int16 __fastcall operator|(const short value) const {
            return h_int16(get() | value);
        }

        __forceinline h_int16 __fastcall operator%(const short value) const {
            return h_int16(get() % value);
        }

        __forceinline h_int16 __fastcall operator^(const short value) const {
            return h_int16(get() ^ value);
        }

        __forceinline h_int16 __fastcall operator<<(const short value) const {
            return h_int16(get() << value);
        }

        __forceinline h_int16 __fastcall operator>>(const short value) const {
            return h_int16(get() >> value);
        }

        __forceinline h_int16& __fastcall operator+=(const short value) {
            set(static_cast<short>(get() + value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator-=(const short value) {
            set(static_cast<short>(get() - value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_int16& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_int16& __fastcall operator*=(const short value) {
            set(static_cast<short>(get() * value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator/=(const short value) {
            set(static_cast<short>(get() / value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator%=(const short value) {
            set(static_cast<short>(get() % value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator^=(const short value) {
            set(static_cast<short>(get() ^ value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator&=(const short value) {
            set(static_cast<short>(get() & value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator|=(const short value) {
            set(static_cast<short>(get() | value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator<<=(const short value) {
            set(static_cast<short>(get() << value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator>>=(const short value) {
            set(static_cast<short>(get() >> value));
            return *this;
        }

        __forceinline h_int16& __fastcall operator=(const short value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator short() const {
            return get();
        }

        __forceinline ~h_int16() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_uint16 {

    private:

#pragma region Globals

        ushort _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_uint16(ushort value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_uint16(const h_uint16& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline ushort __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(ushort value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_uint16 __fastcall operator+(const ushort value) const {
            return h_uint16(get() + value);
        };

        __forceinline h_uint16 __fastcall operator-(const ushort value) const {
            return h_uint16(get() - value);
        }

        __forceinline h_uint16 __fastcall operator/(const ushort value) const {
            return h_uint16(get() / value);
        }

        __forceinline h_uint16 __fastcall operator*(const ushort value) const {
            return h_uint16(get() * value);
        }

        __forceinline h_uint16 __fastcall operator&(const ushort value) const {
            return h_uint16(get() & value);
        }

        __forceinline h_uint16 __fastcall operator|(const ushort value) const {
            return h_uint16(get() | value);
        }

        __forceinline h_uint16 __fastcall operator%(const ushort value) const {
            return h_uint16(get() % value);
        }

        __forceinline h_uint16 __fastcall operator^(const ushort value) const {
            return h_uint16(get() ^ value);
        }

        __forceinline h_uint16 __fastcall operator<<(const ushort value) const {
            return h_uint16(get() << value);
        }

        __forceinline h_uint16 __fastcall operator>>(const ushort value) const {
            return h_uint16(get() >> value);
        }

        __forceinline h_uint16& __fastcall operator+=(const ushort value) {
            set(static_cast<ushort>(get() + value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator-=(const ushort value) {
            set(static_cast<ushort>(get() - value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_uint16& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_uint16& __fastcall operator*=(const ushort value) {
            set(static_cast<ushort>(get() * value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator/=(const ushort value) {
            set(static_cast<ushort>(get() / value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator%=(const ushort value) {
            set(static_cast<ushort>(get() % value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator^=(const ushort value) {
            set(static_cast<ushort>(get() ^ value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator&=(const ushort value) {
            set(static_cast<ushort>(get() & value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator|=(const ushort value) {
            set(static_cast<ushort>(get() | value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator<<=(const ushort value) {
            set(static_cast<ushort>(get() << value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator>>=(const ushort value) {
            set(static_cast<ushort>(get() >> value));
            return *this;
        }

        __forceinline h_uint16& __fastcall operator=(const ushort value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator ushort() const {
            return get();
        }

        __forceinline ~h_uint16() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#pragma endregion

#pragma region 32-bit

    class h_int32 {

    private:

#pragma region Globals

        int _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_int32(int value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_int32(const h_int32& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline int __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(int value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_int32 __fastcall operator+(const int value) const {
            return h_int32(get() + value);
        };

        __forceinline h_int32 __fastcall operator-(const int value) const {
            return h_int32(get() - value);
        }

        __forceinline h_int32 __fastcall operator/(const int value) const {
            return h_int32(get() / value);
        }

        __forceinline h_int32 __fastcall operator*(const int value) const {
            return h_int32(get() * value);
        }

        __forceinline h_int32 __fastcall operator&(const int value) const {
            return h_int32(get() & value);
        }

        __forceinline h_int32 __fastcall operator|(const int value) const {
            return h_int32(get() | value);
        }

        __forceinline h_int32 __fastcall operator%(const int value) const {
            return h_int32(get() % value);
        }

        __forceinline h_int32 __fastcall operator^(const int value) const {
            return h_int32(get() ^ value);
        }

        __forceinline h_int32 __fastcall operator<<(const int value) const {
            return h_int32(get() << value);
        }

        __forceinline h_int32 __fastcall operator>>(const int value) const {
            return h_int32(get() >> value);
        }

        __forceinline h_int32& __fastcall operator+=(const int value) {
            set(static_cast<int>(get() + value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator-=(const int value) {
            set(static_cast<int>(get() - value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_int32& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_int32& __fastcall operator*=(const int value) {
            set(static_cast<int>(get() * value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator/=(const int value) {
            set(static_cast<int>(get() / value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator%=(const int value) {
            set(static_cast<int>(get() % value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator^=(const int value) {
            set(static_cast<int>(get() ^ value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator&=(const int value) {
            set(static_cast<int>(get() & value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator|=(const int value) {
            set(static_cast<int>(get() | value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator<<=(const int value) {
            set(static_cast<int>(get() << value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator>>=(const int value) {
            set(static_cast<int>(get() >> value));
            return *this;
        }

        __forceinline h_int32& __fastcall operator=(const int value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator int() const {
            return get();
        }

        __forceinline ~h_int32() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_uint32 {

    private:

#pragma region Globals

        uint _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_uint32(uint value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_uint32(const h_uint32& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline uint __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(uint value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_uint32 __fastcall operator+(const uint value) const {
            return h_uint32(get() + value);
        };

        __forceinline h_uint32 __fastcall operator-(const uint value) const {
            return h_uint32(get() - value);
        }

        __forceinline h_uint32 __fastcall operator/(const uint value) const {
            return h_uint32(get() / value);
        }

        __forceinline h_uint32 __fastcall operator*(const uint value) const {
            return h_uint32(get() * value);
        }

        __forceinline h_uint32 __fastcall operator&(const uint value) const {
            return h_uint32(get() & value);
        }

        __forceinline h_uint32 __fastcall operator|(const uint value) const {
            return h_uint32(get() | value);
        }

        __forceinline h_uint32 __fastcall operator%(const uint value) const {
            return h_uint32(get() % value);
        }

        __forceinline h_uint32 __fastcall operator^(const uint value) const {
            return h_uint32(get() ^ value);
        }

        __forceinline h_uint32 __fastcall operator<<(const uint value) const {
            return h_uint32(get() << value);
        }

        __forceinline h_uint32 __fastcall operator>>(const uint value) const {
            return h_uint32(get() >> value);
        }

        __forceinline h_uint32& __fastcall operator+=(const uint value) {
            set(static_cast<int>(get() + value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator-=(const uint value) {
            set(static_cast<int>(get() - value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_uint32& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_uint32& __fastcall operator*=(const uint value) {
            set(static_cast<int>(get() * value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator/=(const uint value) {
            set(static_cast<int>(get() / value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator%=(const uint value) {
            set(static_cast<int>(get() % value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator^=(const uint value) {
            set(static_cast<int>(get() ^ value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator&=(const uint value) {
            set(static_cast<int>(get() & value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator|=(const uint value) {
            set(static_cast<int>(get() | value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator<<=(const uint value) {
            set(static_cast<int>(get() << value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator>>=(const uint value) {
            set(static_cast<int>(get() >> value));
            return *this;
        }

        __forceinline h_uint32& __fastcall operator=(const uint value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator int() const {
            return get();
        }

        __forceinline ~h_uint32() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#pragma endregion

#pragma region 64-bit

#ifdef _WIN64

    class h_int64 {

    private:

#pragma region Globals

        int64_t _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_int64(int64_t value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_int64(const h_int64& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline int64_t __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(int64_t value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_int64 __fastcall operator+(const int64_t value) const {
            return h_int64(get() + value);
        };

        __forceinline h_int64 __fastcall operator-(const int64_t value) const {
            return h_int64(get() - value);
        }

        __forceinline h_int64 __fastcall operator/(const int64_t value) const {
            return h_int64(get() / value);
        }

        __forceinline h_int64 __fastcall operator*(const int64_t value) const {
            return h_int64(get() * value);
        }

        __forceinline h_int64 __fastcall operator&(const int64_t value) const {
            return h_int64(get() & value);
        }

        __forceinline h_int64 __fastcall operator|(const int64_t value) const {
            return h_int64(get() | value);
        }

        __forceinline h_int64 __fastcall operator%(const int64_t value) const {
            return h_int64(get() % value);
        }

        __forceinline h_int64 __fastcall operator^(const int64_t value) const {
            return h_int64(get() ^ value);
        }

        __forceinline h_int64 __fastcall operator<<(const int64_t value) const {
            return h_int64(get() << value);
        }

        __forceinline h_int64 __fastcall operator>>(const int64_t value) const {
            return h_int64(get() >> value);
        }

        __forceinline h_int64& __fastcall operator+=(const int64_t value) {
            set(static_cast<int>(get() + value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator-=(const int64_t value) {
            set(static_cast<int>(get() - value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_int64& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_int64& __fastcall operator*=(const int64_t value) {
            set(static_cast<int>(get() * value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator/=(const int64_t value) {
            set(static_cast<int>(get() / value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator%=(const int64_t value) {
            set(static_cast<int>(get() % value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator^=(const int64_t value) {
            set(static_cast<int>(get() ^ value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator&=(const int64_t value) {
            set(static_cast<int>(get() & value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator|=(const int64_t value) {
            set(static_cast<int>(get() | value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator<<=(const int64_t value) {
            set(static_cast<int>(get() << value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator>>=(const int64_t value) {
            set(static_cast<int>(get() >> value));
            return *this;
        }

        __forceinline h_int64& __fastcall operator=(const int64_t value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator int() const {
            return get();
        }

        __forceinline ~h_int64() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_uint64 {

    private:

#pragma region Globals

        uint64_t _value{};

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_uint64(uint64_t value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __fastcall h_uint64(const h_uint64& other) {
            if (&other == this)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline uint64_t __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(uint64_t value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_uint64 __fastcall operator+(const uint64_t value) const {
            return h_uint64(get() + value);
        };

        __forceinline h_uint64 __fastcall operator-(const uint64_t value) const {
            return h_uint64(get() - value);
        }

        __forceinline h_uint64 __fastcall operator/(const uint64_t value) const {
            return h_uint64(get() / value);
        }

        __forceinline h_uint64 __fastcall operator*(const uint64_t value) const {
            return h_uint64(get() * value);
        }

        __forceinline h_uint64 __fastcall operator&(const uint64_t value) const {
            return h_uint64(get() & value);
        }

        __forceinline h_uint64 __fastcall operator|(const uint64_t value) const {
            return h_uint64(get() | value);
        }

        __forceinline h_uint64 __fastcall operator%(const uint64_t value) const {
            return h_uint64(get() % value);
        }

        __forceinline h_uint64 __fastcall operator^(const uint64_t value) const {
            return h_uint64(get() ^ value);
        }

        __forceinline h_uint64 __fastcall operator<<(const uint64_t value) const {
            return h_uint64(get() << value);
        }

        __forceinline h_uint64 __fastcall operator>>(const uint64_t value) const {
            return h_uint64(get() >> value);
        }

        __forceinline h_uint64& __fastcall operator+=(const uint64_t value) {
            set(static_cast<int>(get() + value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator-=(const uint64_t value) {
            set(static_cast<int>(get() - value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline h_uint64& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline h_uint64& __fastcall operator*=(const uint64_t value) {
            set(static_cast<int>(get() * value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator/=(const uint64_t value) {
            set(static_cast<int>(get() / value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator%=(const uint64_t value) {
            set(static_cast<int>(get() % value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator^=(const uint64_t value) {
            set(static_cast<int>(get() ^ value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator&=(const uint64_t value) {
            set(static_cast<int>(get() & value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator|=(const uint64_t value) {
            set(static_cast<int>(get() | value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator<<=(const uint64_t value) {
            set(static_cast<int>(get() << value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator>>=(const uint64_t value) {
            set(static_cast<int>(get() >> value));
            return *this;
        }

        __forceinline h_uint64& __fastcall operator=(const uint64_t value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator int() const {
            return get();
        }

        __forceinline ~h_uint64() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#endif

#pragma endregion

#pragma endregion

#pragma region Floating Point

#pragma region 32-bit

    class h_float {

    private:

#pragma region Globals

        float _value;

        qmem::qwatcher watcher;

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_float(float value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_float(const h_float& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline float __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(float value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_float __fastcall operator+(const float value) const {
            return h_float(get() + value);
        };

        __forceinline h_float __fastcall operator-(const float value) const {
            return h_float(get() - value);
        }

        __forceinline h_float __fastcall operator/(const float value) const {
            return h_float(get() / value);
        }

        __forceinline h_float __fastcall operator*(const float value) const {
            return h_float(get() * value);
        }

        __forceinline h_float& __fastcall operator+=(const float value) {
            set(static_cast<float>(get() + value));
            return *this;
        }

        __forceinline h_float& __fastcall operator-=(const float value) {
            set(static_cast<float>(get() - value));
            return *this;
        }

        __forceinline h_float& __fastcall operator++() {
            operator+=(1.f);
            return *this;
        }

        __forceinline h_float& __fastcall operator--() {
            operator-=(1.f);
            return *this;
        }

        __forceinline h_float& __fastcall operator*=(const float value) {
            set(static_cast<float>(get() * value));
            return *this;
        }

        __forceinline h_float& __fastcall operator/=(const float value) {
            set(static_cast<float>(get() / value));
            return *this;
        }

        __forceinline h_float& __fastcall operator=(const float value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator float() const {
            return get();
        }

        __forceinline ~h_float() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#pragma endregion

#pragma region 64-bit

#ifdef _WIN64

    class h_double {

    private:

#pragma region Globals

        double _value;

        qmem::qwatcher watcher;

        qmemory_data memory_data{};

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall h_double(double value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_double(const h_double& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline double __fastcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __fastcall set(double value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

        __forceinline h_double __fastcall operator+(const double value) const {
            return h_double(get() + value);
        };

        __forceinline h_double __fastcall operator-(const double value) const {
            return h_double(get() - value);
        }

        __forceinline h_double __fastcall operator/(const double value) const {
            return h_double(get() / value);
        }

        __forceinline h_double __fastcall operator*(const double value) const {
            return h_double(get() * value);
        }

        __forceinline h_double& __fastcall operator+=(const double value) {
            set(static_cast<double>(get() + value));
            return *this;
        }

        __forceinline h_double& __fastcall operator-=(const double value) {
            set(static_cast<double>(get() - value));
            return *this;
        }

        __forceinline h_double& __fastcall operator++() {
            operator+=(1.f);
            return *this;
        }

        __forceinline h_double& __fastcall operator--() {
            operator-=(1.f);
            return *this;
        }

        __forceinline h_double& __fastcall operator*=(const double value) {
            set(static_cast<double>(get() * value));
            return *this;
        }

        __forceinline h_double& __fastcall operator/=(const double value) {
            set(static_cast<double>(get() / value));
            return *this;
        }

        __forceinline h_double& __fastcall operator=(const double value) {
            set(value);
            return *this;
        }

        __forceinline __fastcall operator double() const {
            return get();
        }

        __forceinline ~h_double() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#endif

#pragma endregion

#pragma endregion

#pragma region Extended Types

#pragma region SSE

#ifdef __SSE__

    class h_m128 {

    private:

#pragma region Globals

        __m128 _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m128(__m128 value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m128(const h_m128& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m128 __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m128 value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m128 __vectorcall operator+(const __m128 value) const {
            return h_m128(get() + value);
        };

        __forceinline h_m128 __vectorcall operator-(const __m128 value) const {
            return h_m128(get() - value);
        }

        __forceinline h_m128 __vectorcall operator/(const __m128 value) const {
            return h_m128(get() / value);
        }

        __forceinline h_m128 __vectorcall operator*(const __m128 value) const {
            return h_m128(get() * value);
        }

        __forceinline h_m128& __vectorcall operator+=(const __m128 value) {
            set(static_cast<__m128>(get() + value));
            return *this;
        }

        __forceinline h_m128& __vectorcall operator-=(const __m128 value) {
            set(static_cast<__m128>(get() - value));
            return *this;
        }

        __forceinline h_m128& __vectorcall operator*=(const __m128 value) {
            set(static_cast<__m128>(get() * value));
            return *this;
        }

        __forceinline h_m128& __vectorcall operator/=(const __m128 value) {
            set(static_cast<__m128>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m128& __vectorcall operator=(const __m128 value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m128() const {
            return get();
        }

        __forceinline ~h_m128() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#ifdef __SSE2__

    class h_m128i {

    private:

#pragma region Globals

        __m128i _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m128i(__m128i value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m128i(const h_m128i& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m128i __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m128i value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m128i __vectorcall operator+(const __m128i value) const {
            return h_m128i(get() + value);
        };

        __forceinline h_m128i __vectorcall operator-(const __m128i value) const {
            return h_m128i(get() - value);
        }

        __forceinline h_m128i __vectorcall operator/(const __m128i value) const {
            return h_m128i(get() / value);
        }

        __forceinline h_m128i __vectorcall operator*(const __m128i value) const {
            return h_m128i(get() * value);
        }

        __forceinline h_m128i& __vectorcall operator+=(const __m128i value) {
            set(static_cast<__m128i>(get() + value));
            return *this;
        }

        __forceinline h_m128i& __vectorcall operator-=(const __m128i value) {
            set(static_cast<__m128i>(get() - value));
            return *this;
        }

        __forceinline h_m128i& __vectorcall operator*=(const __m128i value) {
            set(static_cast<__m128i>(get() * value));
            return *this;
        }

        __forceinline h_m128i& __vectorcall operator/=(const __m128i value) {
            set(static_cast<__m128i>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m128i& __vectorcall operator=(const __m128i value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m128i() const {
            return get();
        }

        __forceinline ~h_m128i() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_m128d {

    private:

#pragma region Globals

        __m128d _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m128d(__m128d value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m128d(const h_m128d& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m128d __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m128d value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m128d __vectorcall operator+(const __m128d value) const {
            return h_m128d(get() + value);
        };

        __forceinline h_m128d __vectorcall operator-(const __m128d value) const {
            return h_m128d(get() - value);
        }

        __forceinline h_m128d __vectorcall operator/(const __m128d value) const {
            return h_m128d(get() / value);
        }

        __forceinline h_m128d __vectorcall operator*(const __m128d value) const {
            return h_m128d(get() * value);
        }

        __forceinline h_m128d& __vectorcall operator+=(const __m128d value) {
            set(static_cast<__m128d>(get() + value));
            return *this;
        }

        __forceinline h_m128d& __vectorcall operator-=(const __m128d value) {
            set(static_cast<__m128d>(get() - value));
            return *this;
        }

        __forceinline h_m128d& __vectorcall operator*=(const __m128d value) {
            set(static_cast<__m128d>(get() * value));
            return *this;
        }

        __forceinline h_m128d& __vectorcall operator/=(const __m128d value) {
            set(static_cast<__m128d>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m128d& __vectorcall operator=(const __m128d value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m128d() const {
            return get();
        }

        __forceinline ~h_m128d() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#endif

#endif

#pragma endregion

#pragma region AVX

#ifdef __AVX__

    class h_m256 {

    private:

#pragma region Globals

        __m256 _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m256(__m256 value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m256(const h_m256& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m256 __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m256 value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m256 __vectorcall operator+(const __m256 value) const {
            return h_m256(get() + value);
        };

        __forceinline h_m256 __vectorcall operator-(const __m256 value) const {
            return h_m256(get() - value);
        }

        __forceinline h_m256 __vectorcall operator/(const __m256 value) const {
            return h_m256(get() / value);
        }

        __forceinline h_m256 __vectorcall operator*(const __m256 value) const {
            return h_m256(get() * value);
        }

        __forceinline h_m256& __vectorcall operator+=(const __m256 value) {
            set(static_cast<__m256>(get() + value));
            return *this;
        }

        __forceinline h_m256& __vectorcall operator-=(const __m256 value) {
            set(static_cast<__m256>(get() - value));
            return *this;
        }

        __forceinline h_m256& __vectorcall operator*=(const __m256 value) {
            set(static_cast<__m256>(get() * value));
            return *this;
        }

        __forceinline h_m256& __vectorcall operator/=(const __m256 value) {
            set(static_cast<__m256>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m256& __vectorcall operator=(const __m256 value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m256() const {
            return get();
        }

        __forceinline ~h_m256() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_m256i {

    private:

#pragma region Globals

        __m256i _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m256i(__m256i value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m256i(const h_m256i& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m256i __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m256i value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m256i __vectorcall operator+(const __m256i value) const {
            return h_m256i(get() + value);
        };

        __forceinline h_m256i __vectorcall operator-(const __m256i value) const {
            return h_m256i(get() - value);
        }

        __forceinline h_m256i __vectorcall operator/(const __m256i value) const {
            return h_m256i(get() / value);
        }

        __forceinline h_m256i __vectorcall operator*(const __m256i value) const {
            return h_m256i(get() * value);
        }

        __forceinline h_m256i& __vectorcall operator+=(const __m256i value) {
            set(static_cast<__m256i>(get() + value));
            return *this;
        }

        __forceinline h_m256i& __vectorcall operator-=(const __m256i value) {
            set(static_cast<__m256i>(get() - value));
            return *this;
        }

        __forceinline h_m256i& __vectorcall operator*=(const __m256i value) {
            set(static_cast<__m256i>(get() * value));
            return *this;
        }

        __forceinline h_m256i& __vectorcall operator/=(const __m256i value) {
            set(static_cast<__m256i>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m256i& __vectorcall operator=(const __m256i value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m256i() const {
            return get();
        }

        __forceinline ~h_m256i() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

    class h_m256d {

    private:

#pragma region Globals

        __m256d _value;

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __vectorcall h_m256d(__m256d value) {
            memory_data = schedule_memory_work(&_value, sizeof(_value));
            set(value);
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_m256d(const h_m256d& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline __m256d __vectorcall get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __vectorcall set(__m256d value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

#pragma endregion

#pragma region Operators

#ifdef __clang__

        __forceinline h_m256d __vectorcall operator+(const __m256d value) const {
            return h_m256d(get() + value);
        };

        __forceinline h_m256d __vectorcall operator-(const __m256d value) const {
            return h_m256d(get() - value);
        }

        __forceinline h_m256d __vectorcall operator/(const __m256d value) const {
            return h_m256d(get() / value);
        }

        __forceinline h_m256d __vectorcall operator*(const __m256d value) const {
            return h_m256d(get() * value);
        }

        __forceinline h_m256d& __vectorcall operator+=(const __m256d value) {
            set(static_cast<__m256d>(get() + value));
            return *this;
        }

        __forceinline h_m256d& __vectorcall operator-=(const __m256d value) {
            set(static_cast<__m256d>(get() - value));
            return *this;
        }

        __forceinline h_m256d& __vectorcall operator*=(const __m256d value) {
            set(static_cast<__m256d>(get() * value));
            return *this;
        }

        __forceinline h_m256d& __vectorcall operator/=(const __m256d value) {
            set(static_cast<__m256d>(get() / value));
            return *this;
        }

#endif

        __forceinline h_m256d& __vectorcall operator=(const __m256d value) {
            set(value);
            return *this;
        }

        __forceinline __vectorcall operator __m256d() const {
            return get();
        }

        __forceinline ~h_m256d() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#endif

#pragma endregion

#pragma endregion

#pragma region String Types

#pragma region String

    class h_string {

    private:

#pragma region Encrypted value

        std::string _value = "";

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __cdecl h_string(const std::string& value) {
            memory_data = schedule_memory_work(&_value, sizeof(char) * value.size());

            set(value);
        }

        __forceinline __cdecl h_string(const char* value) {
            std::string temporary_string(value);

            memory_data = schedule_memory_work(&_value, sizeof(char) * temporary_string.size());

            set(std::string(value));
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline h_string(const h_string& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline std::string __cdecl get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __cdecl set(const std::string& value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

        __forceinline h_string __cdecl operator+(const std::string& value) const {
            return h_string(get() + value);
        }

        __forceinline h_string& __cdecl operator+=(const std::string& value) {
            set(static_cast<std::string>(get() + value));
            return *this;
        }

        __forceinline h_string& __cdecl operator=(const std::string& value) {
            set(value);
            return *this;
        }

        __forceinline __cdecl operator std::string() const {
            return get();
        }

        __forceinline __cdecl ~h_string() {
            //attempt to trigger memory release
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, sizeof(_value));
            }
            catch (std::exception& except) {}
        }

#pragma endregion

#pragma region Operators

#pragma endregion
    };

#pragma endregion

#pragma region Wide String

    class h_wstring {

    private:

#pragma region Encrypted value

        std::wstring _value = L"";

        qmemory_data memory_data{ };

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __cdecl h_wstring(const std::wstring& value) {
            memory_data = schedule_memory_work(&_value, sizeof(wchar_t) * value.size());

            set(value);
        }

        __forceinline __cdecl h_wstring(const wchar_t* value) {
            std::wstring temporary_wstring(value);

            memory_data = schedule_memory_work(&_value, sizeof(wchar_t) * temporary_wstring.size());

            set(std::wstring(value));
        }

#pragma region Thread-Safe Copy Ctor

        __forceinline __cdecl h_wstring(const h_wstring& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            set(other._value);
        }

#pragma endregion

#pragma endregion

#pragma region Accessors

        __forceinline std::wstring __cdecl get() const {
            /* pause memory scans and flag for re-hash */
            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).unlock_region_watch(memory_data.data_queue_index);

            auto _return = _value;

            (*const_cast<qmem::qwatcher*>(&qwatch_threads[memory_data.thread_index])).lock_region_watch(memory_data.data_queue_index, false);

            return _return;
        }

        __forceinline void __cdecl set(const std::wstring& value) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            _value = value;

            qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, true);
        }

        __forceinline h_wstring __cdecl operator+(const std::wstring& value) const {
            return h_wstring(get() + value);
        }

        __forceinline h_wstring& __cdecl operator+=(const std::wstring& value) {
            set(static_cast<std::wstring>(get() + value));
            return *this;
        }

        __forceinline h_wstring& __cdecl operator=(const std::wstring& value) {
            set(value);
            return *this;
        }

        __forceinline __cdecl operator std::wstring() const {
            return get();
        }

        __forceinline __cdecl ~h_wstring() {
            //attempt to trigger memory release
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(&_value, _value.size());
            }
            catch (std::exception& except) {}
        }

#pragma endregion

#pragma region Operators

#pragma endregion
    };

#pragma endregion

#pragma region Heap Allocation

    class h_malloc {

    private:

#pragma region Globals

        void* allocation;
        size_t allocation_size;

        qmemory_data memory_data{};

#pragma endregion

#pragma region Mutex-Safe accessors

        __forceinline void __fastcall access_m() {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);
        }

        __forceinline void __fastcall close_access_m(bool rehash) {
            /* pause memory scans and flag for re-hash */
            qwatch_threads[memory_data.thread_index].unlock_region_watch(memory_data.data_queue_index);

            if (rehash)
                qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, allocation, allocation_size);
            else
                qwatch_threads[memory_data.thread_index].lock_region_watch(memory_data.data_queue_index, false);
        }
#pragma endregion

#pragma region Subscript proxy

        // nested class to support subscript assignment 
        class index_proxy {

        private:

#pragma region Globals

            h_malloc& parent;
            size_t index;

#pragma endregion

        public:
#pragma region Ctor

            index_proxy(size_t index_, h_malloc& instance) : index(index_), parent(instance) { }

#pragma endregion

#pragma region Operator overrides

            byte& operator=(byte value) {
                parent.set(index, value);
                return value;   // return the passed assignment value rather than using up absurd resources to decrypt, re-encrypt everything using get()
            }

            __forceinline __fastcall operator byte() const {
                return parent.get(index);
            }

#pragma endregion

        };

#pragma endregion

    public:

        void illegal_set(byte b) {
            reinterpret_cast<byte*>(allocation)[0] = b;
        }

#pragma region Ctor

        __forceinline __fastcall h_malloc(size_t size) : allocation_size(size) {
            if (!size)
                return;

            allocation = malloc(size);

            memory_data = schedule_memory_work(allocation, allocation_size);

            if (!allocation)
                throw qexcept::q_badalloc();
        }

        __forceinline __fastcall h_malloc(const h_malloc& other) {
            if (this == &other)
                return;

            memory_data = other.memory_data;
            allocation_size = other.allocation_size;

            swap(other.allocation, other.allocation_size);
        }

#pragma endregion

#pragma region Get Accessors

        __forceinline void __fastcall get(uintptr_t source_position, void* destination, size_t length) {
            access_m();

            uintptr_t iteration = 0x0;
            auto data_p = reinterpret_cast<byte*>(destination);

            while (data_p < reinterpret_cast<byte*>(reinterpret_cast<uintptr_t>(destination) + length)) {

                *data_p = reinterpret_cast<byte*>(allocation)[source_position + iteration];

                ++iteration;
                ++data_p;
            }

            close_access_m(false);
        }

        __forceinline byte __fastcall get(uintptr_t position) {
            byte _return = NULL;

            get(position, &_return, sizeof(_return));

            return _return;
        }

        __forceinline void* __fastcall get() {
            auto _return = malloc(allocation_size);

            get(0, _return, allocation_size);

            return _return;
        }

#pragma endregion

#pragma region Set Accessors

        __forceinline void __fastcall set(uintptr_t position, void* data, size_t length) {
            access_m();

            uintptr_t iteration = 0x0;
            auto data_p = &reinterpret_cast<byte*>(allocation)[position];

            while (data_p < reinterpret_cast<byte*>(reinterpret_cast<uintptr_t>(allocation) + position + length)) {

                *data_p = reinterpret_cast<byte*>(data)[iteration];

                ++iteration;
                ++data_p;
            }

            close_access_m(true);
        }

        template<typename T>
        __forceinline void __fastcall set(uintptr_t position, T&& value) {
            set(position, &value, sizeof(decltype(value)));
        }

#pragma endregion

#pragma region Utility functions

        __forceinline void __fastcall reallocate(size_t size) {
            access_m();

            auto alloc_r = realloc(allocation, size);

            if (!alloc_r)
                throw qexcept::q_badalloc();

            allocation = std::move(alloc_r);
            allocation_size = size;

            close_access_m(true);

        }

        __forceinline void __fastcall swap(void* allocation_, size_t size) {
            access_m();

            allocation = allocation_;
            allocation_size = size;

            close_access_m(true);
        }

#pragma endregion

#pragma region Operators

        __forceinline index_proxy __fastcall operator[](size_t index) {
            return index_proxy(index, *this);
        }

#pragma endregion

#pragma region Destructor

        __forceinline ~h_malloc() {
            try {
                qwatch_threads[memory_data.thread_index].remove_memory_region(allocation, allocation_size);

                free(allocation);
                allocation = nullptr;
            }
            catch (std::exception& except) {}
        }

#pragma endregion

    };

#pragma endregion

}

#endif

