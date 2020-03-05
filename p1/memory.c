#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>
#include "MSR.h"
#include "CPU.h"
#include "Common.h"

#define ALIGNMENT_PAGE_SIZE 4096
#define VMXON_SIZE 4096
#define VCMS_SIZE 4096

/*
��� :VMXON Region �Ҵ�, �޸� �ʱ�ȭ, vmx on ����
*/
BOOLEAN Allocate_VMXON_Region(IN PVirtualMachineState vmState)
{
	//IRQL level�� DISPATCH_LEVEL�� ���� , �׷��� MmAllocateContiguousMemory �Լ��� ����ϸ� ���ص� ��.
	if (KeGetCurrentIrql() > DISPATCH_LEVEL)
		KeRaiseIrqlToDpcLevel();

	PHYSICAL_ADDRESS PhysicalMax = { 0 };
	PhysicalMax.QuadPart = MAXULONG64;

	//UC �޸� Ÿ������ �޸� �Ҵ��ϴ� Ŀ�� API
	int VMXONSize = 2 * VMXON_SIZE;
	BYTE* Buffer = MmAllocateContiguousMemory(VMXONSize + ALIGNMENT_PAGE_SIZE, PhysicalMax);

	PHYSICAL_ADDRESS Highest = { 0 }, Lowest = { 0 };
	Highest.QuadPart = ~0;

	if (Buffer == NULL) {
		//dbgPrint
		return FALSE;
	}
	UINT64 PhysicalBuffer = VirtualAddress_to_PhysicalAddress(Buffer);

	//�޸� ���� �ʱ�ȭ
	RtlSecureZeroMemory(Buffer, VMXONSize + ALIGNMENT_PAGE_SIZE);
	UINT64 alignedPhysicalBuffer = (BYTE*)( (ULONG_PTR)(PhysicalBuffer + ALIGNMENT_PAGE_SIZE-1) & ~(ALIGNMENT_PAGE_SIZE-1) );
	UINT64 alignedVirtualBuffer = (BYTE*)( (ULONG_PTR)(Buffer + ALIGNMENT_PAGE_SIZE - 1) & ~(ALIGNMENT_PAGE_SIZE - 1) );

	//DbgPrintEx();

	//vmx ���� 
	IA32_VMX_BASIC_MSR basic = { 0 };

	basic.All = __readmsr(MSR_IA32_VMX_BASIC);

	//DbgPrintEx();

	//Intel's Manual�� ���� VMXON ���� �� VMCS RevisionIdentifier�� write �ؾ� ��.
	*(UINT64*)alignedVirtualBuffer = basic.Fields.RevisionIdentifier;

	int status = __vmx_on(&alignedPhysicalBuffer); //status : 0(����), 1(region ��������), 2(vmcs ����)
	if (status) {
		//DbgPrintEx();
		return FALSE;
	}

	vmState->VMXON_REGION = alignedPhysicalBuffer;

	return TRUE;
	
}

/*
��� :VMXCS Region �Ҵ�. �޸� �ʱ�ȭ, vmptrld ����
*/
BOOLEAN Allocate_VMCS_Region(IN PVirtualMachineState vmState)
{
	if (KeGetCurrentIrql() > DISPATCH_LEVEL)
		KeRaiseIrqlToDpcLevel();

	PHYSICAL_ADDRESS PhysicalMax = { 0 };
	PhysicalMax.QuadPart = MAXULONG64;

	int VMCSSize = 2 * VCMS_SIZE;
	BYTE* Buffer = MmAllocateContiguousMemory(VMCSSize + ALIGNMENT_PAGE_SIZE, PhysicalMax);
	PHYSICAL_ADDRESS Highest = { 0 }, Lowest = { 0 };
	Highest.QuadPart = ~0;

	UINT64 PhysicalBuffer = VirtualAddress_to_PhysicalAddress(Buffer);
	if (Buffer == NULL) {
		//dbgPrintex("[*]error: couldn't allocate buffer for vmcs region.");
		return FALSE;
	}
	RtlSecureZeroMemory(Buffer, VMCSSize + ALIGNMENT_PAGE_SIZE);
	UINT64 alignedPhysicalBuffer = (BYTE*)((ULONG_PTR)(PhysicalBuffer + ALIGNMENT_PAGE_SIZE - 1) & ~(ALIGNMENT_PAGE_SIZE - 1));
	UINT64 alignedVirtualBuffer = (BYTE*)((ULONG_PTR)(Buffer + ALIGNMENT_PAGE_SIZE - 1) & ~(ALIGNMENT_PAGE_SIZE - 1));

	//DbgPrintEx();

	//vmcs ���� 
	IA32_VMX_BASIC_MSR basic = { 0 };

	basic.All = __readmsr(MSR_IA32_VMX_BASIC);

	//DbgPrintEx();

	//Intel's Manual�� ���� VMXON ���� �� VMCS RevisionIdentifier�� write �ؾ� ��.
	*(UINT64*)alignedVirtualBuffer = basic.Fields.RevisionIdentifier;

	int status = __vmx_vmptrld(&alignedPhysicalBuffer);
	if (status) {
		//DbgPrintEx("[*] VMCS failed with status %d\n", status);
		return FALSE;
	}

	vmState->VMCS_REGION = alignedPhysicalBuffer;

	return TRUE;

}


/*
��� : �ּҺ�ȯ
*/
UINT64 VirtualAddress_to_PhysicalAddress(void* va) {
	return MmGetPhysicalAddress(va).QuadPart;
}

UINT64 PhysicalAddress_to_VirtualAddress(UINT64 pa) {
	PHYSICAL_ADDRESS PhysicalAddr;
	PhysicalAddr.QuadPart = pa;

	return MmGetVirtualForPhysical(PhysicalAddr);
}