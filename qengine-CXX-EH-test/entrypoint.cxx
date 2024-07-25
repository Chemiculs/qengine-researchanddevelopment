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

__symbolic std::int32_t __stackcall main() {    //  Windows SEH is not used 

    CXX_EH_INIT();  //  initialize CXX exception handler

    std::cout << "Hello World!" << std::endl;   //  insert trash code so the compiler has something to generate inside the EH block

    std::cin.get(); 

    CXX_EH_END();   //  end CXX EH execution block

    return static_cast<std::int32_t>(NULL);
}

#pragma endregion