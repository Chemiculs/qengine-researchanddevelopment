#pragma region Imports

#pragma region std

#include <iostream>

#pragma endregion

#pragma region qengine

#include <qengine/qmorph/qdisasm.hpp>

#include <qengine/qhook/qcall_table.hpp>

using namespace qengine;

#pragma endregion

#pragma endregion

#pragma region EP fn

__singleton std::int32_t __stackcall main() noexcept {

	//You do not have to use all of the below functions, however analyze_executable_sections() must be called before morph_executable_sections(), and this must be called before manipulating headers as it depends on information from the headers to perform analyzation
	
	qmorph::qdisasm::qsection_assembler sec{ };	//	initialize qengine's PE manipulation object on stack

	sec.analyze_executable_sections();	//	perform initial analysis on executable section of compiler output in memory

	
	if (sec.morph_executable_sections(true)) // NOW we morph our stored sections and pass true to flag for memory clearance 
		std::cout << "Interrupt Padding morphed successfully! " << std::endl;
	else
		std::cout << "Interrupt Padding failed to be morphed! " << std::endl;

	if (sec.zero_information_sections())
		std::cout << "Garbage sections nulled" << std::endl;
	else
		std::cout << "Garbage section wipe failed" << std::endl;

	if (sec.scramble_dos_header(true))
		std::cout << "DOS headers wiped" << std::endl;
	else
		std::cout << "DOS headers not wiped" << std::endl;

	if (sec.scramble_nt_header())
		std::cout << "NT headers wiped" << std::endl;
	else
		std::cout << "NT headers not wiped" << std::endl;

	std::cout << ".text / header permutations complete!" << std::endl;

	

	std::cin.get();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion