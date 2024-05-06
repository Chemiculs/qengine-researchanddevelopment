#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion

#pragma region Static / Singleton Globals

/* First template argument specifies return type, subsequent template arguments specify argument type list in Left -> Right order for the fn being imported */
static const auto imp_MessageBoxA = qimport::qimp::get_fn_import_object<NTSTATUS, unsigned int, const char*, const char*, unsigned int>(L"user32.dll", "MessageBoxA");

#pragma endregion

#pragma region EP fn

__singleton std::int32_t __stackcall main() noexcept {

	auto status = imp_MessageBoxA(NULL, "Hello World!", "Hello World!", NULL); // call MessageBoxA and assign it's status return to a local 

	std::cout << status << std::endl; // output the return status to the console 

	std::cin.get();
}

#pragma endregion