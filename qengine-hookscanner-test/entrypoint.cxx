#include <iostream>

#include <qengine/engine/qengine.hpp>

using namespace qengine;

#pragma region Placeholder Methods

static __singleton void __regcall myimportantmethod(std::uintptr_t val) noexcept { // add junk code to our dummy method to increase it's size in memory to be viable for hook placement

	auto j = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	auto k = j % val;

	std::cout << k << std::endl;
}

#pragma endregion

#pragma region Callback Methods

__singleton void __stackcall callback(qexcept::q_fn_alteration alteration) noexcept {	//	callbacks are never inlined nor inlineable, therefore in this example i am explicitly declaring these things

	if (alteration.id != qexcept::HOOK_DETECTED)
		return;

	if (!alteration.violation_object_)
		return;	//	duhh
	
	auto casted_arg = reinterpret_cast<qhook::qhook_detection_t*>(alteration.violation_object_);

	std::cout << "Function hook detected, address: " << std::hex << casted_arg->hook_address << std::endl;
	std::cout << "Hook size: " << casted_arg->hook_length << std::endl;
	std::cout << "Hook data: " << std::endl;

	for (auto i = 0; i < casted_arg->hook_length; ++i)
		std::cout << std::hex << (int)casted_arg->hook_data[i] << std::endl;

	delete casted_arg; // this was allocated with new, must be deleted inside callback to avoid memory leak
}

#pragma endregion

#pragma region EP fn

__singleton  std::int32_t __stackcall main() noexcept {

	std::cout << "initializing hook scanner..." << std::endl;

	qhook::qhook_t::set_client_callback_fn(&callback);

	qhook::qhook_t((void*)&myimportantmethod);

	// any of the below hooks will be detected - you could change the registers used etc. if you wanted to

	unsigned char hook1[12] = {
		0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xFF, 0xE0 // mov rax, 0x1111111111111111 ; jmp rax
	};

	unsigned char hook2[14] = { // this is a trash hook used to test features of the detection, push rax, pop rax is a NOP essentially
		0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x50, 0x58, // mov rax, 0x1111111111111111 ; push rax ; pop rax ; jmp rax
		0xFF, 0xE0
	};

	unsigned char hook3[12] = {
		0x48, 0xB8, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x50, 0xC3 // mov rax, 0x1111111111111111 ; push rax ; ret
	};

	myimportantmethod(4);

	std::cout << "emplacing hook..." << std::endl;

	auto* ptr = static_cast<void*>(&myimportantmethod);

	DWORD tmp{};

	VirtualProtect(ptr, sizeof(hook2), PAGE_EXECUTE_READWRITE, &tmp);

	memcpy(ptr, &hook2, sizeof(hook2));

	VirtualProtect(ptr, sizeof(hook2), tmp, &tmp);

	std::cin.get();

	return static_cast<std::int32_t>(NULL);
}

#pragma endregion