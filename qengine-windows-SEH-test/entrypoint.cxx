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

__singleton std::int32_t __stackcall main() noexcept {	//	CXX exceptions are not used

	WINAPI_SEH_INIT();

	std::cout << "Hello World!" << std::endl;

	std::cin.get();

	WINAPI_SEH_END();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion