#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion

#pragma region Callback Methods

static __nothrow __singleton void true_() noexcept {	//	callback functions shall never be inlined and should always be explicitly declared as a singleton point of execution, intentions are very important to know

	std::cout << "condition is true" << std::endl;

	std::cin.get();
}

static __nothrow __singleton void false_() noexcept {

	std::cout << "condition is false" << std::endl;

	std::cin.get();
}

#pragma endregion

#pragma region EP fn

__nothrow __singleton std::int32_t __stackcall main() noexcept {

	int x = 1;
	int y = 1;

	qcritical::SCRAMBLE_CRITICAL_CONDITION(
		&true_,				// callback if condition evaluates to TRUE
		&false_,			// callback if condition evaluates to FALSE
		std::tuple<>{},     // arguments (if any) for TRUE evaluated callback (our callback has no arguments)
		std::tuple<>{},		// arguments (if any) for FALSE evaluated callback (our callback has no arguments)
		x, y,				// our condition variables from left -> right order (can be of any primitive type or std::string / std::wstring type for now)
		qcritical::EQUALTO  // evaluation type (less than, greater than, equal to, greaterthanorequalto etc. )
	);

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion