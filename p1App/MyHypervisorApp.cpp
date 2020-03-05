// p1App.cpp : �� ���Ͽ��� 'main' �Լ��� ���Ե˴ϴ�. �ű⼭ ���α׷� ������ ���۵ǰ� ����˴ϴ�.
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
        //eax =0 ���� �ʱ�ȭ , CPUID 
        XOR EAX, EAX
        CPUID

        //EBX �� Ȯ��.
        MOV EAX, EBX
        MOV SysType[0], al
        MOV SysType[1], ah
        SHR EAX, 16
        MOV SysType[2], al
        MOV SysType[3], ah

        //EDX �� Ȯ��.
        MOV EAX, EDX
        MOV SysType[4], al
        MOV SysType[5], ah
        SHR EAX, 16
        MOV SysType[6], al
        MOV SysType[7], ah

        //ECX �� Ȯ��
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
        bt      ecx, 0x5     //5��° ��Ʈ carry �÷��׿� ����
        jc      vmxsupport
        vmxnotsupport :     //carry �÷��� != 1
        jmp     nopinstr
            vmxsupport :        //carry �÷���  = 1
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



// ���α׷� ����: <Ctrl+F5> �Ǵ� [�����] > [��������� �ʰ� ����] �޴�
// ���α׷� �����: <F5> Ű �Ǵ� [�����] > [����� ����] �޴�

// ������ ���� ��: 
//   1. [�ַ�� Ž����] â�� ����Ͽ� ������ �߰�/�����մϴ�.
//   2. [�� Ž����] â�� ����Ͽ� �ҽ� ��� �����մϴ�.
//   3. [���] â�� ����Ͽ� ���� ��� �� ��Ÿ �޽����� Ȯ���մϴ�.
//   4. [���� ���] â�� ����Ͽ� ������ ���ϴ�.
//   5. [������Ʈ] > [�� �׸� �߰�]�� �̵��Ͽ� �� �ڵ� ������ ����ų�, [������Ʈ] > [���� �׸� �߰�]�� �̵��Ͽ� ���� �ڵ� ������ ������Ʈ�� �߰��մϴ�.
//   6. ���߿� �� ������Ʈ�� �ٽ� ������ [����] > [����] > [������Ʈ]�� �̵��ϰ� .sln ������ �����մϴ�.
