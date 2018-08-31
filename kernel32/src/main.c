#include "types.h"
#include "page.h"
#include "modeswitch.h"

void kPrintString(int x, int y, const char * pcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

void Main(void) {
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[13] = {0};

    kPrintString(0, 4, "[+] C Kernel Language");
    kPrintString(0, 5, "[*] Minimum Memory Size Check");
    if(kIsMemoryEnough() == FALSE) {
        kPrintString(0, 6, "[!] Minimum Memory Size Check Failed");
        kPrintString(0, 7, "[!] SimpleOS needs over 64MB Memory");
        while(1);
    }
    kPrintString(0, 6, "[+] Minimum Memory Size Check Complete");
    kPrintString(0, 7, "[*] IA-32e Kernel Area Initialization");
    if(kInitializeKernel64Area() == FALSE) {
        kPrintString(0, 8, "[!] IA-32e Kernel Area Initialization Failed");
        while(1);
    }
    kPrintString(0, 8, "[+] IA-32e Kernel Area Initialization Complete");
    kPrintString(0, 9, "[*] IA-32e Page Tables Initialization");
    kInitializePageTables();
    kPrintString(0, 10, "[+] IA-32e Page Tables Initialization Complete");

    kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    *(DWORD *) vcVendorString = dwEBX;
    *((DWORD*)vcVendorString + 1) = dwEDX;
    *((DWORD*)vcVendorString + 2) = dwECX;
    kPrintString(0, 11, "[*] Process Vendor String : ");
    kPrintString(28, 11, vcVendorString);
    kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    if(dwEDX & (1 << 29)) {
        kPrintString(0, 12, "[+] 64bit Mode Support Check Passed");
    }
    else {
        kPrintString(0, 12, "[!] 64bit Mode Support Check Failed");
        kPrintString(0, 13, "[!] This processor does not support 64bit mode");
        while(1);
    }
    kPrintString(0, 13, "[*] Switching to IA-32e Mode");
    

    while(1);
}
void kPrintString(int x, int y, const char * pcString) {
    CHARACTER * pstScreen = (CHARACTER * ) 0xB8000;
    int i;

    pstScreen += (y * 80) + x;
    for(i = 0; pcString[i] != 0; i++) {
        pstScreen[i].bCharacter = pcString[i];
    }
}
BOOL kInitializeKernel64Area(void) {
    DWORD * pdwCurrentAddress;
    pdwCurrentAddress = (DWORD *) 0x100000;

    while((DWORD) pdwCurrentAddress < 0x600000) {
        *pdwCurrentAddress = 0x00;
        if(*pdwCurrentAddress != 0) {
            return FALSE;
        }
        pdwCurrentAddress++;
    }
    return TRUE;
}
BOOL kIsMemoryEnough(void) {
    DWORD * pdwCurrentAddress;
    pdwCurrentAddress = (DWORD *) 0x100000;
    while((DWORD)pdwCurrentAddress < 0x4000000) {
        *pdwCurrentAddress = 0x12345678;
        if(*pdwCurrentAddress != 0x12345678) {
            return FALSE;
        }
        pdwCurrentAddress += (0x100000 / 4);
    }
    return TRUE;
}