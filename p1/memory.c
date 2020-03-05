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
기능 :VMXON Region 할당, 메모리 초기화, vmx on 실행
*/
BOOLEAN Allocate_VMXON_Region(IN PVirtualMachineState vmState)
{
	//IRQL level을 DISPATCH_LEVEL로 변경 , 그러나 MmAllocateContiguousMemory 함수를 사용하면 안해도 됨.
	if (KeGetCurrentIrql() > DISPATCH_LEVEL)
		KeRaiseIrqlToDpcLevel();

	PHYSICAL_ADDRESS PhysicalMax = { 0 };
	PhysicalMax.QuadPart = MAXULONG64;

	//UC 메모리 타입으로 메모리 할당하는 커널 API
	int VMXONSize = 2 * VMXON_SIZE;
	BYTE* Buffer = MmAllocateContiguousMemory(VMXONSize + ALIGNMENT_PAGE_SIZE, PhysicalMax);

	PHYSICAL_ADDRESS Highest = { 0 }, Lowest = { 0 };
	Highest.QuadPart = ~0;

	if (Buffer == NULL) {
		//dbgPrint
		return FALSE;
	}
	UINT64 PhysicalBuffer = VirtualAddress_to_PhysicalAddress(Buffer);

	//메모리 영역 초기화
	RtlSecureZeroMemory(Buffer, VMXONSize + ALIGNMENT_PAGE_SIZE);
	UINT64 alignedPhysicalBuffer = (BYTE*)( (ULONG_PTR)(PhysicalBuffer + ALIGNMENT_PAGE_SIZE-1) & ~(ALIGNMENT_PAGE_SIZE-1) );
	UINT64 alignedVirtualBuffer = (BYTE*)( (ULONG_PTR)(Buffer + ALIGNMENT_PAGE_SIZE - 1) & ~(ALIGNMENT_PAGE_SIZE - 1) );

	//DbgPrintEx();

	//vmx 관리 
	IA32_VMX_BASIC_MSR basic = { 0 };

	basic.All = __readmsr(MSR_IA32_VMX_BASIC);

	//DbgPrintEx();

	//Intel's Manual에 따라 VMXON 실행 전 VMCS RevisionIdentifier를 write 해야 함.
	*(UINT64*)alignedVirtualBuffer = basic.Fields.RevisionIdentifier;

	int status = __vmx_on(&alignedPhysicalBuffer); //status : 0(성공), 1(region 설정실패), 2(vmcs 없음)
	if (status) {
		//DbgPrintEx();
		return FALSE;
	}

	vmState->VMXON_REGION = alignedPhysicalBuffer;

	return TRUE;
	
}

/*
기능 :VMXCS Region 할당. 메모리 초기화, vmptrld 실행
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

	//vmcs 관리 
	IA32_VMX_BASIC_MSR basic = { 0 };

	basic.All = __readmsr(MSR_IA32_VMX_BASIC);

	//DbgPrintEx();

	//Intel's Manual에 따라 VMXON 실행 전 VMCS RevisionIdentifier를 write 해야 함.
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
기능 : 주소변환
*/
UINT64 VirtualAddress_to_PhysicalAddress(void* va) {
	return MmGetPhysicalAddress(va).QuadPart;
}

UINT64 PhysicalAddress_to_VirtualAddress(UINT64 pa) {
	PHYSICAL_ADDRESS PhysicalAddr;
	PhysicalAddr.QuadPart = pa;

	return MmGetVirtualForPhysical(PhysicalAddr);
}