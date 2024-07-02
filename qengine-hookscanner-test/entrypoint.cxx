#include <iostream>

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma region Macros

#define PRINT_HEXADECIMAL std::hex << std::noshowbase

#define STDOUT_PRINTBLOCK_SEPERATOR() std::cout << "\n[--------------------------------------------------------------------------------------]" << std::endl;

#pragma endregion

#pragma region Placeholder Methods

static __singleton void __regcall myimportantmethod(std::uintptr_t val) noexcept { // add junk code to our dummy method to increase it's size in memory to be viable for hook placement

	auto j = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	auto k = j % val;

	std::cout << "\n[+] Placeholder function called, output: " << k << std::endl;
}

#pragma endregion

#pragma region Callback Methods

__singleton void __stackcall print_hook_details(qengine::qhook::qhook_detection_t* detection) noexcept {	//	callbacks are never inlined nor inlineable, therefore in this example i am explicitly declaring these things

	STDOUT_PRINTBLOCK_SEPERATOR();

	std::cout << "\n[+] Function hook detected, address: 0x" << std::hex << detection->hook_address << std::endl;
	std::cout << "\n[+] Hook size: 0x" << detection->hook_length << " bytes" << std::endl;
	std::cout << "\n[+] Hook data: \n\n" << std::endl;

	for (auto i = 0; i < detection->hook_length; ++i)
		std::cout << std::hex << "0x" << (std::uint32_t)detection->hook_data[i] << std::endl;

	STDOUT_PRINTBLOCK_SEPERATOR();
}

#pragma endregion

#pragma region EP fn

__singleton std::int32_t __stackcall main() noexcept {

	std::cout << "\n[+] Analyzing function length..." << std::endl;

	imut auto function_length = qengine::qhook::qhook_util::analyze_fn_length(&myimportantmethod);

	std::cout << "\n[+] Succeeded, function length is " << function_length << " bytes" << std::endl;

	// The most elementary x86_64 mov rax, jmp rax inline function hook
	unsigned char hook1[12] = {0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xFF, 0xE0 };

	// x86_32 equivalent of above hook - mov eax ADDRESS, jmp eax
	std::uint8_t hook1_x32[] = { 0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0 }; // mov eax, 0x00000000, jmp eax

	// This hook demonstrates the inability of junk instructions emplaced between CFT (control-flow-transfer) instructions to throw off the current algorithm
	unsigned char hook2[14] = { 
		0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x50, 0x58, // mov rax, ADDRESS; push rax; pop rax; jmp rax
		0xFF, 0xE0
	};

	// x86_32 equivalent of above hook
	unsigned char hook2_x32[] = {
		0xb8, 0x00, 0x00, 0x00, 0x00, 0x50, 0x58, 0xff, // mov eax, ADDRESS; push eax; pop eax; jmp eax
		0xe0,
	};

	// This hook demonstrates the ability of the algorithm to detect RETURN-induced control-flow transference
	unsigned char hook3[12] = {
		0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x50, 0xC3 // mov rax, ADDRESS ; push rax ; ret
	};

	unsigned char hook3_x32[] = {
		0xb8, 0x00, 0x00, 0x00, 0x00, 0x50, 0xc3, // mov eax, ADDRESS; push eax; ret
	};

	myimportantmethod(4);

	std::cout << "\n[+] Granting R/W/X permissions to function memory..." << std::endl;

	auto* ptr = static_cast<void*>(&myimportantmethod);

	DWORD tmp{};

	VirtualProtect(ptr, sizeof(hook1), PAGE_EXECUTE_READWRITE, &tmp);

	std::cout << "\n[+] Beginning x86_64 inline hook detection..." << std::endl;

	memcpy(ptr, &hook1, sizeof(hook1));

	qengine::qhook::qhook_detection_t* dtc;

	std::cout << "\n[+] Emplaced x86_64 hook #1: mov rax, ADDRESS; jmp rax ..." << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	//Emplace 2nd hook type
	memcpy(ptr, &hook2, sizeof(hook2));

	std::cout << "\n[+] Emplaced x86_64 hook #2: mov rax, ADDRESS; push rax; pop rax; jmp rax ..." << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	memcpy(ptr, &hook3, sizeof(hook3));

	std::cout << "\n[+] Emplaced x86_64 hook #3: // mov rax, ADDRESS ; push rax ; ret" << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	memcpy(ptr, &hook1_x32, sizeof(hook1_x32));

	std::cout << "\n[+] Emplaced x86_32 hook #1: mov eax, ADDRESS; jmp eax ..." << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	memcpy(ptr, &hook2_x32, sizeof(hook2_x32));

	// WARNING: This 32-bit hook format is improperly detected on 64-bit build targets (it recognizes the hooks presence, but returns invalid length)
	// This is considered precisely a non-issue as 32-bit hooks fail in 64-bit address spacing
	std::cout << "\n[+] Emplaced x86_32 hook #2: mov eax, ADDRESS; push eax; pop eax; jmp eax ..." << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	memcpy(ptr, &hook3_x32, sizeof(hook3_x32));

	std::cout << "\n[+] Emplaced x86_32 hook #3: mov eax, ADDRESS; push eax; ret ..." << std::endl;
	if (dtc = qengine::qhook::qhook_util::analyze_fn_hook_presence(&myimportantmethod, function_length))
		print_hook_details(dtc);

	//Restore page protections as we are done emplacing inline hooks
	VirtualProtect(ptr, sizeof(hook1), tmp, &tmp);

	std::cin.get();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion