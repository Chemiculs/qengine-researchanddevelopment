#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

#pragma region Using Directives

using namespace qengine;

#pragma endregion

#pragma endregion

// This creates a decryption proxy which is compiled to entrypoint and executed once at startup
qimutexpr(std::uint32_t, const_1, 0xFEDCBA98ui32);

// This creates a caller-stackframe object representing the constant expression, which will decay upon return to caller
qimutexpr_stack(std::uint32_t, const_2, 666);

#pragma region EP fn

__symbolic std::int32_t __stackcall main() noexcept {	//	explicit declarators are used as the point of this project is explicit communication with the compiler, however these are not required

	// The QSTR macro, as qconstexpr, creates a decryption proxy for the raw CSTRING argument
	qtype_enc::qe_string my_string_e(QSTR("Hello World!"));

	qtype_hash::qh_string my_string_h(QSTR("Hello World!"));

	qtype_enchash::qeh_string my_string_eh(QSTR("Hello World!"));

	std::cout << my_string_e.get() << std::endl;

	std::cout << my_string_h.get() << std::endl;

	std::cout << my_string_eh.get() << std::endl;

	std::cout << const_1 << std::endl;

	std::cout << (std::uint32_t)const_2 << std::endl;

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion