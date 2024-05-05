#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion

#pragma region EP fn

__nothrow __singleton std::int32_t __stackcall main() noexcept {	//	explicit declarators are used as the point of this project is explicit communication with the compiler, however these are not required

	qtype_enc::qe_string my_string_e("Hello World!");

	qtype_hash::qh_string my_string_h("Hello World!");

	qtype_enchash::qeh_string my_string_eh("Hello World!");

	std::cout << my_string_e.get() << std::endl;

	std::cout << my_string_h.get() << std::endl;

	std::cout << my_string_eh.get() << std::endl;

	std::cin.get();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion