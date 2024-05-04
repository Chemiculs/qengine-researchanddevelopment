#ifndef PHASH_H
#define PHASH_H

#include "../qbase/qdef.hpp"

namespace qhash {
	
	/*
	/**************************************************************************************************************
	*																											 *
	*  QHash is made to be a fast and performative 32 / 64-bit hashing algorithm for runtime data integrity      *
	*																											 *
	***************************************************************************************************************
	*/

#pragma region Type Definitions

#pragma region Hash Structure

	typedef struct qhash32_t {

		unsigned long hash;

	};

	typedef struct qhash64_t {
		
		unsigned long long hash;

	};

#pragma endregion

#pragma endregion

#pragma region Hash Constants

#define MOST_SIGNIFICANT_DEFAULT 0xAE

#pragma region 32-bit

#define QHBASE32 0xFAE9E8D7ul

#define QHEPILOGUE32 0xAEBDCCDBul

	extern bool initialized32;

	extern uint32_t qtable32[];

#pragma endregion

#pragma region 64-bit

#ifdef _WIN64

#define QHBASE64 0xFAE9E8D7C6B5A493ull

#define QHEPILOGUE64 0xAEBDCCDB0A192837ull

	extern bool initialized64;

	extern uint64_t qtable64[];

#endif

#pragma endregion

#pragma endregion

#pragma region Table Generation

	__forceinline static void qtable32_gen() {

		unsigned long seed = 0xFEEDDCCBul;

		unsigned short decrementor = 0xFFFFu;

		for (auto i = 0; i < 255; ++i) {

			qtable32[i] = ( ( ( seed ^ QHBASE32) ^ QHEPILOGUE32 ) ^ 0xFFFFFFFFul );

			seed -= decrementor;

		}
	}

#ifdef _WIN64

	__forceinline static void qtable64_gen() {

		unsigned long long seed = 0xFEEDDCCBBAA99887ull;

		unsigned long decrementor = 0xFFFFFFFF;

		for (auto i = 0; i < 255; ++i) {

			qtable64[i] = ( ( ( seed ^ QHBASE64) ^ QHEPILOGUE64 ) ^ 0xFFFFFFFFFFFFFFFFull );

			seed -= decrementor;

		}
	}

#endif

#pragma endregion

#pragma region Hashing
	// 0.0000000233% collision rate among 65535 unique 2-byte data sets ( 1 out of 4,294,770,690 possible collisions )
	__forceinline static qhash32_t __fastcall qhash32(void* data, uint32_t length) {
		/* check if our global variables have been initialized */
		if (!initialized32) {
			qtable32_gen();

			initialized32 = true;
		}

		/* initialize base hash32 value constant within structure */
		qhash32_t hash_r{
			QHBASE32
		};

		for (auto i = 0; i < length; ++i) {

			for (auto x = 0; x < sizeof(decltype(hash_r.hash)); ++x)
				reinterpret_cast<byte*>(&hash_r.hash)[x] ^= reinterpret_cast<byte*>(data)[i];

			hash_r.hash ^= qtable32[reinterpret_cast<byte*>(data)[i] ^ 0xFF];

		}

		auto preamble_result = hash_r.hash;

		/* initialize epilogue with base epilogue constant */
		auto epilogue = QHEPILOGUE32;

		byte least_significant = reinterpret_cast<byte*>(data)[0];
		byte most_significant = (length > 1 ? reinterpret_cast<byte*>(data)[length - 1] : MOST_SIGNIFICANT_DEFAULT);

		uint32 least_significant_length = length;
		uint32 most_significant_length = length;

		for (auto i = 0; i < sizeof(decltype(length)); ++i) {
			*reinterpret_cast<byte*>(&least_significant_length) ^= least_significant;
			*reinterpret_cast<byte*>(&most_significant_length) ^= most_significant;
		}

		bool significance_switch = false;

		for (auto i = 0; i < sizeof(hash_r.hash); ++i) {
			/* inverse significance flag */
			significance_switch = (significance_switch ? false : true);

			/* incrementally shift the bits left to ensure the entire output word has been marked by the sizeof(data) (this is more important for smaller datasets) */
			epilogue ^= static_cast<uint32_t>((QHEPILOGUE32 ^ (length << ((length * 8) % (((i == 0 ? 1 : i) * 8))))));
			reinterpret_cast<byte*>(&hash_r.hash)[i] ^= reinterpret_cast<byte*>(&epilogue)[i];

			/* shift length left i * (sizeof( byte in bits ), xor hash by results to make the result more unique */
			hash_r.hash ^= (((significance_switch ? most_significant_length : least_significant_length) << (32 - ((i * 8) > 0 ? (i * 8) : 8))));
		}

		auto epilogue_result = hash_r.hash;

		hash_r.hash = (preamble_result & epilogue_result) ^ ((0xFFFFFFFF - (most_significant * 255) - (least_significant * 128)));

		return hash_r;
	}

#ifdef _WIN64
	 //  0.00% collision rate among every possible 2-byte data set ( 0 out of 4,294,770,690 possible collisions )
	  __forceinline static qhash64_t __fastcall qhash64(void* data, size_t length) {

		/* check if our global variables have been initialized */
		if (!initialized64) {
			qtable64_gen();

			initialized64 = true;
		}

		/* initialize base hash64 value constant within structure */
		qhash64_t hash_r{
			QHBASE64
		};

		for (auto i = 0; i < length; ++i){

			for (auto x = 0; x < sizeof(decltype(hash_r.hash)); ++x) 
				reinterpret_cast<byte*>(&hash_r.hash)[x] ^= reinterpret_cast<byte*>(data)[i];

			hash_r.hash ^= qtable64[reinterpret_cast<byte*>(data)[i] ^ 0xFF];

		}
		auto preamble_result = hash_r.hash;

		/* initialize epilogue with base epilogue constant */
		auto epilogue = QHEPILOGUE64;

		byte least_significant = reinterpret_cast<byte*>(data)[0];
		byte most_significant = (length > 1 ? reinterpret_cast<byte*>(data)[length - 1] : MOST_SIGNIFICANT_DEFAULT);
		
		uint64 least_significant_length = length;
		uint64 most_significant_length = length;

		for (auto i = 0; i < sizeof(decltype(length)); ++i) {
			*reinterpret_cast<byte*>(&least_significant_length) ^= least_significant;
			*reinterpret_cast<byte*>(&most_significant_length) ^= most_significant;
		}
		
		bool significance_switch = false;

		for (auto i = 0; i < sizeof(hash_r.hash); ++i) {
			/* inverse significance flag */
			significance_switch = (significance_switch ? false : true);
			/* incrementally shift the bits left to ensure the entire output word has been marked by the sizeof(data) (this is more important for smaller datasets) */
			epilogue ^= static_cast<uint64_t>((QHEPILOGUE64 ^ (length << ((length * 8) % (((i == 0 ? 1 : i) * 8))))));
			reinterpret_cast<byte*>(&hash_r.hash)[i] ^= reinterpret_cast<byte*>(&epilogue)[i];
			/* shift length left i * (sizeof( byte in bits ), xor hash by results to make the result more unique */
			hash_r.hash ^= ((( significance_switch ? most_significant_length : least_significant_length ) << (64 - ((i * 8) > 0 ? (i * 8) : 8))) );
		}

		auto epilogue_result = hash_r.hash;

		hash_r.hash = (preamble_result & epilogue_result) ^ ( ( 0xFFFFFFFFFFFFFFFF - (most_significant * 255) - (least_significant * 128 ) ) );

		return hash_r;
	}

#endif

#pragma endregion

}

#endif