#include "Common.h"
#include "CPU.h"
#include "MSR.h"

/*
part3
��� : Ŀ�ο��� �����۹����� �����Ǵ��� ����.
*/
BOOLEAN Is_VMX_Supported() {

    CPUID data = { 0 };

    //vmx bit
    __cpuid((int*)&data, 1);
    if ((data.ecx & (1 >> 5)) == 0)
        return FALSE;

    IA32_FEATURE_CONTROL_MSR Control = { 0 };
    Control.All = __readmsr(MSR_IA32_FEATURE_CONTROL);

    //bios lock check
    if (Control.Fields.Lock == 0) {
        Control.Fields.Lock = TRUE;
        Control.Fields.EnableVmxon = TRUE;
        __writemsr(MSR_IA32_FEATURE_CONTROL, Control.All);

    }
    else if (Control.Fields.EnableVmxon == FALSE) {
        return FALSE;
    }

    return TRUE;
}

/*
���μ����� AFFINITY ����
*/
void Run_On_Each_Logical_Processor(void* (*FunctionPtr)()) {
    KAFFINITY kAffinityMask;
    for (size_t i = 0; i < KeQueryActiveProcessorCount(0); i++) {
        kAffinityMask = ipow(2, i);
        KeSetSystemAffinityThread(kAffinityMask);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "======================");
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Current thread is executing in %d th logical processor", i);
    }
}

