#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "types.h"

void kReadCPUID(DWORD dwEAX, DWORD * pdwEAX, DWORD * pdwEBX, DWORD * pdwECX, DWORD * pdwEDX);
void kSwitchAndExecuteKernel64(void);

#endif