#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion

#pragma region Callback Functions

__singleton void __regcall violation_callback(qexcept::q_rogueaccess except, void* data) noexcept {

	if (except.id != qexcept::MEMORY_ALTERATION) // ensure this callback has been raised due to memory alteration
		return;

	std::cout << "Memory access violation occurred, original hash: " << std::hex << except.original_hash << std::endl; // display the original hash of the data when it was valid

	std::cout << "Altered hash: " << std::hex << except.altered_hash << std::endl; // display the hash of the data which was altered

	std::cout << "Memory address: " << std::hex << reinterpret_cast<uintptr_t>(data) << std::endl; //display the memory address of the data which was altered 

	//Here you would normally flag the user for a ban/violation of contract or force-quit the application as a security breach has obviously occured
}

#pragma endregion

#pragma region EP fn

__singleton std::int32_t __stackcall main() noexcept {

	qtype_enchash::init_qtype_hash( &violation_callback ); // assign our callback function to the namespace - all instances will refer to this callback if they detect a violation

	qtype_enchash::qeh_int32 MyInteger(999); // instance a hash-checked integer and set its value to 999
	
	*static_cast<std::uint32_t*>(const_cast<void*>(MyInteger.get_raw_memory_address())) = 998; // use the built-in illegal-accessor for this example to modify the value of the data and trigger our callback
	
	std::int32_t value = MyInteger.get(); // store the value held within MyInteger in a local primitive type to invoke get() and check data integrity

	std::cout << "Hacked value: " << value << std::endl; // print the new / hacked value to the screen (998)

	std::cin.get();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion