#pragma once
#include <ntddk.h>

//VM 상태 관련 구조체. (VMX, VMCS Region)
typedef struct _VirtualMachineState {
	UINT64 VMXON_REGION; //VMXON region
	UINT64 VMCS_REGION;  //VMCS region
}VirtualMachineState, * PVirtualMachineState;

extern PVirtualMachineState vmState;

#define POOLTAG 0x48564653

UINT64 VirtualAddress_to_PhysicalAddress(void* va);
UINT64 PhysicalAddress_to_VirtualAddress(UINT64 pa);
void Run_On_Each_Logical_Processor(void* (*FunctionPtr)());
int ipow(int base, int exp);

BOOLEAN Allocate_VMCS_Region(IN PVirtualMachineState vmState);
BOOLEAN Allocate_VMXON_Region(IN PVirtualMachineState vmState);

PVirtualMachineState Initate_VMX(void);
void Terminate_VMX(void);
