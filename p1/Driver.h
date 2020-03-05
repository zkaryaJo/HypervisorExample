#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>

extern void inline Enable_VMX_Operation(void);
extern void inline Breakpoint(void);

//device type : user defined range
#define SIOCTL_TYPE 40000

//ioctl function code from 0x8000 to 0xfff are for customer use.
#define IOCTL_SIOCTL_METHOD_IN_DIRECT		CTL_CODE(SIOCTL_TYPE, 0x900, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_SIOCTL_METHOD_OUT_DIRECT		CTL_CODE(SIOCTL_TYPE, 0x901, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_SIOCTL_METHOD_BUFFERED		CTL_CODE(SIOCTL_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SIOCTL_METHOD_NEITHER			CTL_CODE(SIOCTL_TYPE, 0x903, METHOD_NEITHER, FILE_ANY_ACCESS)

//IOCTL Codes and Its meanings
#define IOCTL_TEST 0x1 // In case of testing 


//Driver 관련 함수
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegisterPath);
VOID DrvUnload(PDRIVER_OBJECT DriverObject);
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DrvUnload)

//Device 관련 함수
NTSTATUS DrvUnsupported(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvIOCTLDispatcher(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DrvWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
#pragma alloc_text(PAGE, DrvUnsupported)
#pragma alloc_text(PAGE, DrvCreate)
#pragma alloc_text(PAGE, DrvClose)
#pragma alloc_text(PAGE, DrvIOCTLDispatcher)
#pragma alloc_text(PAGE, DrvRead)
#pragma alloc_text(PAGE, DrvWrite)

VOID PrintIrpInfo(PIRP Irp);
VOID PrintChars(_In_reads_(CountChars) PCHAR BufferAddress, _In_ size_t CountChars);
#pragma alloc_text(PAGE, PrintIrpInfo)
#pragma alloc_text(PAGE, PrintChars)


