#pragma once

extern "C" {
#include <melon/core/vm.h>
}

extern "C" {
TRet sourceModuleInit(VM* vm, const char* path, const char* source);
}