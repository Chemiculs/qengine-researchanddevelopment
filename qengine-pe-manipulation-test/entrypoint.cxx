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

__singleton std::int32_t __stackcall main() noexcept {

	//You do not have to use all of the below functions, however analyze_executable_sections() must be called before morph_executable_sections(), and this must be called before manipulating headers as it depends on information from the headers to perform analyzation
	std::cout << "[+] Initializing section assembler object..." << std::endl;

	qmorph::qdisasm::qsection_assembler sec{ };	//	initialize qengine's PE manipulation object

	std::cout << "[+] Analyzing image for executable code..." << std::endl;

	sec.analyze_executable_sections();	//	perform initial analysis on executable section of compiler output in memory
	
	if (sec.morph_executable_sections(true)) // NOW we morph our stored sections and pass true to flag for memory clearance 
		std::cout << "[+] Executable Interrupt Padding morphed successfully! " << std::endl;
	else
		std::cout << "[!] Executable Interrupt Padding failed to be morphed! " << std::endl;

	if (sec.wipe_idata_iat_ilt())
		std::cout << "[+] .idata / ILT Wiped, IAT preserved!" << std::endl;
	else
		std::cout << "[!] .idata / ILT wipe failed!" << std::endl;

	if (sec.wipe_basereloc())
		std::cout << "[+] basereloc section wipe succeeded!" << std::endl;
	else
		std::cout << "[!] basereloc section wipe failed!" << std::endl;

	if (sec.wipe_section_headers())
		std::cout << "[+] Section headers wiped!" << std::endl;
	else
		std::cout << "[!] Section header wipe failed!" << std::endl;

	if (sec.scramble_dos_header())
		std::cout << "[+] DOS headers scrambled!" << std::endl;
	else
		std::cout << "[!] DOS header scramble failed!" << std::endl;

	if (sec.scramble_nt_header())
		std::cout << "[+] NT headers scrambled!" << std::endl;
	else
		std::cout << "[!] NT headers scramble failed!" << std::endl;

	std::cout << "[+] .text / PE header permutations complete!" << std::endl;

	std::cin.get();

	// Check IAT was preserved during wipe by calling an imported function which address must be retrieved via IAT 
	MessageBoxA(NULL, "", "", NULL);

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion