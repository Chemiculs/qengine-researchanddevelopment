#ifndef QEXCEPT_H
#define QEXCEPT_H

#include "qdef.hpp"

#include <string>

namespace qexcept {

#pragma region Interface Prototype

	struct qexcept_t {

		std::string message;
		
		unsigned long id;

		bool iserror;

	};

#pragma endregion

#pragma region Memory Exceptions

#pragma region Preset Exceptions

	enum qexcept_mem_e {

		BAD_ALLOC,

		BAD_ACCESS,

		BAD_PTR,

		MEMORY_ALTERATION,

		THREAD_VIOLATION,

		ACCESS_VIOLATION

	};

	static const char* qexcept_mem_str[6]{ 

		"BAD_ALLOC",

		"BAD_ACCESS",

		"BAD_PTR",

		"MEMORY_ALTERATION",

		"THREAD_VIOLATION",

		"ACCESS_VIOLATION"
	};

#pragma endregion

#pragma region Memory Exception Prototype

	struct qexcept_mem : qexcept_t {

		qexcept_mem(qexcept_mem_e except_t) {

			message = std::string(qexcept_mem_str[except_t]);

			id = except_t;
			
			switch (except_t) {
				case BAD_ALLOC: {
					iserror = true;
					break;
				}
				case BAD_ACCESS: {
					iserror = true;
					break;
				}
				case ACCESS_VIOLATION: {
					iserror = true;
					break;
				}
				case THREAD_VIOLATION: {
					iserror = true;
					break;
				}
				default: {
					iserror = false;
					break;
				}
			}
		}



	};

#pragma endregion

#pragma region Memory Exception Presets

	struct q_badalloc : qexcept_mem {
		q_badalloc() : qexcept_mem(qexcept_mem_e::BAD_ALLOC){  }
	};

	struct q_rogueaccess : qexcept_mem {
		hash_t original_hash;
		hash_t altered_hash;
		q_rogueaccess(hash_t oldhash, hash_t newhash) : qexcept_mem(qexcept_mem_e::MEMORY_ALTERATION) { original_hash = oldhash; altered_hash = newhash; };
	};

#pragma endregion

#pragma endregion

}

#endif