#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>
#include "Driver.h"
#include "Common.h"
#include "CPU.h"

PVirtualMachineState vmState;
int ProcessorCounts;

PVirtualMachineState Initate_VMX(void) {

    if (!Is_VMX_Supported()) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "this is not supported.\n");
        return NULL;
    }

    ProcessorCounts = KeQueryActiveProcessorCount(0);
    vmState = ExAllocatePoolWithTag(NonPagedPool, sizeof(VirtualMachineState) * ProcessorCounts, POOLTAG);

    //DbgPrintEx("\n===============================\n");
    KAFFINITY kAffinityMask;
    for (size_t i = 0; i < ProcessorCounts; i++) {
        kAffinityMask = ipow(2, i);
        KeSetSystemAffinityThread(kAffinityMask);
        //DbgPrint("");
        //DbgPrintEx("[+] Current Thread is executing in %d th logical processor", i);
        Enable_VMX_Operation();
        //DbgPrintEx("[+] VMX Operation Enabled Successfully!");
        Allocate_VMXON_Region(&vmState[i]);
        Allocate_VMCS_Region(&vmState[i]);

        //DbgPrintEx("\n===============================\n");
        //DbgPrintEx("\n===============================\n");
        //DbgPrintEx("\n===============================\n");
    }

}


void Terminate_VMX(void) {
    //DbgPrint("");
    KAFFINITY kAffinityMask;
    for (size_t i = 0; i < ProcessorCounts; i++) {
        kAffinityMask = ipow(2, i);
        KeSetSystemAffinityThread(kAffinityMask);
        //DbgPrint("");

        __vmx_off();
        MmFreeContiguousMemory(PhysicalAddress_to_VirtualAddress(vmState[i].VMXON_REGION));
        MmFreeContiguousMemory(PhysicalAddress_to_VirtualAddress(vmState[i].VMCS_REGION));

    }
    //DbgPrintEx("[*] VMX Operation turned off successfully.\n");

}