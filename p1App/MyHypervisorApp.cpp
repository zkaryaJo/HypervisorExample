// p1App.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <Windows.h>
#include <conio.h>
#include <iostream>  
#include <vector>  
#include <bitset>  
#include <array>  
#include <string>  
#include <intrin.h>  

using namespace std;

//part2
string GetCpuID() {

    char SysType[13];
    string CpuID;

    _asm
    {
        //eax =0 으로 초기화 , CPUID 
        XOR EAX, EAX
        CPUID

        //EBX 값 확인.
        MOV EAX, EBX
        MOV SysType[0], al
        MOV SysType[1], ah
        SHR EAX, 16
        MOV SysType[2], al
        MOV SysType[3], ah

        //EDX 값 확인.
        MOV EAX, EDX
        MOV SysType[4], al
        MOV SysType[5], ah
        SHR EAX, 16
        MOV SysType[6], al
        MOV SysType[7], ah

        //ECX 값 확인
        MOV EAX, ECX
        MOV SysType[8], al
        MOV SysType[9], ah
        SHR EAX, 16
        MOV SysType[10], al
        MOV SysType[11], ah
        MOV SysType[12], 00
    }

    CpuID.assign(SysType, 12);
    return CpuID;
}

//part2
bool VMX_Support_Detection()
{
    bool vmx = false;
    __asm {
        //eax = 1
        xor eax, eax
        inc     eax

        cpuid
        bt      ecx, 0x5     //5번째 비트 carry 플래그에 복사
        jc      vmxsupport
        vmxnotsupport :     //carry 플래그 != 1
        jmp     nopinstr
            vmxsupport :        //carry 플래그  = 1
        mov     vmx, 0x1
            nopinstr :
            nop
    }
    return vmx;
}


int main()
{
    string CpuID;
    CpuID = GetCpuID();


    cout << "[+] The CPU Vendor is : "<< CpuID << endl;
    if (CpuID == "GenuineIntel") {
        cout << "[+] The Processor virtualization technology is VT-x\n";
    }
    else {
        cout << "[+] This program is not designed to run in a non-VT-x environment!";
    }

    if (VMX_Support_Detection()) {
        cout << "[+] VMX Operation is supported by your processor \n";

    }
    else {
        cout << "[+] VMX Operation is not supported by your processor \n";
        return 1;
    }

    HANDLE handle = CreateFile(L"\\\\.\\MyHypervisorDevice",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, //lpSecurityAttributes
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL); //lpTemplateFile

    /*char OutputBuffer[1000];
    char InputBuffer[1000];
    ULONG bytesReturned;
    BOOL Result;

    StringCbCopy(InputBuffer, sizeof(InputBuffer), "This String is from user Application; using METHOD_BUFFERED");
    printf("\nCalling DeviceIoControl METHOD_BUFFERED:\n");
    memset(OutputBuffer, 0, sizeof(OutputBuffer));

    Result = DeviceIoControl(handle,
        (DWORD)IOCTL_SIOCTL_METHOD_BUFFERED,
        &InputBuffer,
        (DWORD)strlen(InputBuffer) + 1,
        &OutputBuffer,
        sizeof(OutputBuffer),
        &bytesReturned,
        NULL
    );*/




    _getch();

    CloseHandle(handle);
    return 0;

}



// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
