#ifndef QENC_T_H
#define QENC_T_H

#include "../hashtypes/qhash_t.hpp"
#include "../enc_t.hpp"

namespace qenc_t {


#pragma region Types

#pragma region WORD

#pragma region 8-bit

    class q_int8 {

    private:

#pragma region Encrypted value

        qhash_t::h_int8 _value;

#pragma endregion

    public:

#pragma region CTOR

        __forceinline __fastcall q_int8(char value) : _value(algo_t_inst(value)) {
        }

#pragma endregion

#pragma region Accessors

        __forceinline char __fastcall get() const {
            return algo_t_inst(_value);
        }

        __forceinline void __fastcall set(char value) {
            _value = algo_t_inst(value);
        }

        __forceinline void* get_raw_memory_address() {
            return _value.get_raw_memory_address();
        }
#pragma endregion

        void illegal_set(char value) {
            _value.illegal_set(value);
        }

#pragma region Operators

        __forceinline q_int8 __fastcall operator+(const char value) const {
            return q_int8(algo_t_inst(_value) + value);
        };

        __forceinline q_int8 __fastcall operator-(const char value) const {
            return q_int8(algo_t_inst(_value) - value);
        }

        __forceinline q_int8 __fastcall operator/(const char value) const {
            return q_int8(algo_t_inst(_value) / value);
        }

        __forceinline q_int8 __fastcall operator*(const char value) const {
            return q_int8(algo_t_inst(_value) * value);
        }

        __forceinline q_int8 __fastcall operator&(const char value) const {
            return q_int8(algo_t_inst(_value) & value);
        }

        __forceinline q_int8 __fastcall operator|(const char value) const {
            return q_int8(algo_t_inst(_value) | value);
        }

        __forceinline q_int8 __fastcall operator%(const char value) const {
            return q_int8(algo_t_inst(_value) % value);
        }

        __forceinline q_int8 __fastcall operator^(const char value) const {
            return q_int8(algo_t_inst(_value) ^ value);
        }

        __forceinline q_int8 __fastcall operator<<(const char value) const {
            return q_int8(algo_t_inst(_value) << value);
        }

        __forceinline q_int8 __fastcall operator>>(const char value) const {
            return q_int8(algo_t_inst(_value) >> value);
        }

        __forceinline q_int8& __fastcall operator+=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) + value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator-=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) - value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator++() {
            operator+=(1i8);
            return *this;
        }

        __forceinline q_int8& __fastcall operator--() {
            operator-=(1i8);
            return *this;
        }

        __forceinline q_int8& __fastcall operator*=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) * value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator/=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) / value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator%=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) % value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator^=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) ^ value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator&=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) & value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator|=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) | value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator<<=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) << value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator>>=(const char value) {
            this->_value = algo_t_inst(static_cast<char>(algo_t_inst(_value) >> value));
            return *this;
        }

        __forceinline q_int8& __fastcall operator=(const char value) {
            _value = algo_t_inst(value);
            return *this;
        }

        __forceinline __fastcall operator char() const {
            return algo_t_inst(_value);
        }

        __forceinline ~q_int8() {
            try {
                _value.~h_int8();
            }
            catch (std::exception& except) {}
        }
#pragma endregion
    };

#pragma endregion

#pragma endregion

#pragma endregion

}

#endif