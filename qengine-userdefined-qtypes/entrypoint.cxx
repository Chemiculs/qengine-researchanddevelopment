#include <iostream>

#include <qengine/engine/qengine.hpp>

typedef struct MYSTRUCT {

	float axis[16];
	std::int32_t values[0x1000]; // allocate 4 pages of memory for junk members
	std::uint16_t flags;
	std::uint8_t magic;
}_MYSTRUCT;

#define STDOUT_PRINTBLOCK_SEPERATOR() std::cout << "\n\n<-------------------------------------------------------------------------------------->\n\n";

__singleton void __regcall violation_callback(qengine::qexcept::q_rogueaccess except, void* data) noexcept {

	if (except.id != qengine::qexcept::MEMORY_ALTERATION) // ensure this callback has been raised due to memory alteration
		return;

	std::cout << "Memory access violation occurred, original hash: " << std::hex << except.original_hash << std::endl; // display the original hash of the data when it was valid

	std::cout << "Altered hash: " << std::hex << except.altered_hash << std::endl; // display the hash of the data which was altered

	std::cout << "Memory address: " << std::hex << reinterpret_cast<uintptr_t>(data) << std::endl; //display the memory address of the data which was altered 

	//Here you would normally flag the user for a ban/violation of contract or force-quit the application as a security breach has obviously occured
}

int main() nex {

	qengine::qtype_hash::init_hash_t(&violation_callback);

	std::cout << "[+] Allocating user-defined structure MYSTRUCT..." << std::endl;

	MYSTRUCT structure{};
	structure.flags = 0xFFFFui16;
	structure.magic = 0xAEui8;

	// Initialize qe_xxx (polymorphic encryption type) struct
	std::cout << "[+] Initializing qe_struct polymorphic instance of MYSTRUCT instance..." << std::endl;
	qengine::qtype_enc::qe_struct<MYSTRUCT> structe(structure); // Default constructure NULL initializes structure, but we will construct with a pre-defined instance of the structure

	// Initialize qh_xxx (hash-validated types) struct
	std::cout << "[+] Initializing qh_struct hash-validated instance of MYSTRUCT instance..." << std::endl;
	qengine::qtype_hash::qh_struct<MYSTRUCT> structh(structure);

	// Initialize qe_xxx (polymorphic encryption + hash-validated type) struct
	std::cout << "[+] Initializing qe_struct polymorphic + hash-validated instance of MYSTRUCT instance..." << std::endl;
	
	STDOUT_PRINTBLOCK_SEPERATOR();

	qengine::qtype_enchash::qeh_struct<MYSTRUCT> structeh(structure);

	std::cout << "[+] Flags wrote to qe_struct: \n" << 0xFFFFui16 << "\n[+] Flags retrieved from qe_struct: \n" << structe.get(&MYSTRUCT::flags) << std::endl;

	structh.set(&MYSTRUCT::flags, 0x0001ui16);

	STDOUT_PRINTBLOCK_SEPERATOR();

	std::cout << "[+] Flags wrote to qh_struct: \n" << 0x0001ui16 << "\n[+] Flags retrieved from qh_struct: \n" << structh.get(&MYSTRUCT::flags) << std::endl;

	structeh.set(&MYSTRUCT::flags, 0x0000ui16);

	STDOUT_PRINTBLOCK_SEPERATOR()

	std::cout << "[+] Flags wrote to qeh_struct: \n" << 0x0000ui16 << "\n[+] Flags retrieved from qeh_struct: \n" << structeh.get(&MYSTRUCT::flags) << std::endl;

	std::cout << "[+] To test hash-integrity of qh_struct and qeh_struct type after illegal alteration, press enter..." << std::endl;

	std::cin.get();

	// qtype_enchash initialization is overwriting the pointer to violation_callback.
	*imut_cast<MYSTRUCT*>(structh.get_raw_memory_address()) = structure;
	*imut_cast<MYSTRUCT*>((MYSTRUCT*)structeh.get_raw_memory_address()) = structure;

	std::cout << structh.get().flags << std::endl;
	std::cout << structeh.get().flags << std::endl;

	return 0;
}