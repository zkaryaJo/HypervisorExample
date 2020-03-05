#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>
#include "Driver.h"
#include "Common.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegisterPath)
{
	NTSTATUS NtStatus = STATUS_SUCCESS;
	UINT64 uiIndex = 0;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING usDriverName, usDosDeviceName;

	//커널 디버거에 메시지 보내기.
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "DriverEntry Called.");

	RtlInitUnicodeString(&usDriverName, L"\\Device\\MyHypervisorDevice");
	RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\MyHypervisorDevice");

	// Device 만들기(User 모드)
	NtStatus = IoCreateDevice(pDriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject); 

	//Device에 커널 Driver를 symboliclink로 연결
	NTSTATUS NtStatusSymLinkResult = IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);


	//Driver에 Major 함수 구성
	if (NtStatus == STATUS_SUCCESS && NtStatusSymLinkResult == STATUS_SUCCESS)
	{
		//드라이버 초기화
		for (uiIndex = 0; uiIndex<IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
			pDriverObject->MajorFunction[uiIndex] = DrvUnsupported;

		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] Setting Devices major functions.");
		
		pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DrvClose;
		pDriverObject->MajorFunction[IRP_MJ_CREATE] = DrvCreate;
		pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvIOCTLDispatcher;
		pDriverObject->MajorFunction[IRP_MJ_READ] = DrvRead;
		pDriverObject->MajorFunction[IRP_MJ_WRITE] = DrvWrite;


		pDriverObject->DriverUnload = DrvUnload;
		//pDriverObject->Flags != IO_TYPE_DEVICE;
		//pDriverObject->Flags &= (~DO_DEVICE_INITIALIZING);
		/*IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);*/
	}

	return NtStatus;
}

/*
함수명	: DrvUnload 
기능	: 커널 드라이버 객체를 Unload.
*/
VOID DrvUnload(PDRIVER_OBJECT DriverObject) 
{
	UNICODE_STRING usDosDeviceName;
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] DrvUnload Called.");
	RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\MyHypervisor");
	IoDeleteSymbolicLink(&usDosDeviceName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

/*
함수명	: DrvUnsupported
기능	: 디바이스 객체의 정의되지 않은 MJ 함수 오류처리.
*/
NTSTATUS DrvUnsupported(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] This function is not supported :( !");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS DrvCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {

	if (Initate_VMX()) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[+] VMX Initated Successfully!");
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS DrvClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"[+] DrvClose Called!");
	Terminate_VMX();
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
NTSTATUS DrvRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	DbgPrint("Not Implemented yet!");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
NTSTATUS DrvWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	DbgPrint("Not Implemented yet!");

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}


NTSTATUS DrvIOCTLDispatcher(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION	irpSp;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG inBufLength;
	ULONG outBufLength;
	PCHAR inBuf, outBuf;
	PCHAR data = "This String is from Device Driver !!!";
	size_t datalen = strlen(data) + 1;
	PMDL mdl = NULL;
	PCHAR buffer = NULL;

	UNREFERENCED_PARAMETER(DeviceObject);

	PAGED_CODE(); // IRQL

	irpSp = IoGetCurrentIrpStackLocation(Irp);
	inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
	outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;

	if (!inBufLength || !outBufLength)
	{
		ntStatus = STATUS_INVALID_PARAMETER;
		goto End;
	}

	switch (irpSp->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_SIOCTL_METHOD_BUFFERED:
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] data From User.");
			PrintIrpInfo(Irp);
			inBuf = Irp->AssociatedIrp.SystemBuffer;
			outBuf = Irp->AssociatedIrp.SystemBuffer;
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] data From User.");
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] data From User.");
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] data From User.");
			PrintChars(inBuf, inBufLength);
			RtlCopyBytes(outBuf, data, outBufLength);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[*] data to User.");
			PrintChars(outBuf, datalen);
			Irp->IoStatus.Information = (outBufLength < datalen ? outBufLength : datalen);

			break;
	}

End:
	Irp->IoStatus.Status = ntStatus;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntStatus;

}

VOID PrintIrpInfo(PIRP Irp)
{
	PIO_STACK_LOCATION irpSp;
	irpSp = IoGetCurrentIrpStackLocation(Irp);

	PAGED_CODE();
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "\tIrp->AssociateIrp.SystemBuffer = 0x%p\n", Irp->AssociatedIrp.SystemBuffer);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "\tIrp->UserBuffer= 0x%p\n", Irp->UserBuffer);	
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "\tirpSp->Parameters.DeviceIoControl.Type3InputBuffer= 0x%p\n", irpSp->Parameters.DeviceIoControl.Type3InputBuffer);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "\tirpSp->Parameters.DeviceIoControl.InputBufferLength= %d\n", irpSp->Parameters.DeviceIoControl.InputBufferLength);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "\tirpSp->Parameters.DeviceIoControl.OutputBufferLength= %d\n", irpSp->Parameters.DeviceIoControl.OutputBufferLength);

	return;
}

int ipow(int base, int exp) {
	int result = 1;
	for (;;) {
		if (exp & 1) {
			result *= base;
		}

		exp >>= 1;
		if (!exp) {
			break;
		}

		base *= base;
	}
	return result;
}

VOID PrintChars(_In_reads_(CountChars) PCHAR BufferAddress, _In_ size_t CountChars)
{
	PAGED_CODE();

	if (CountChars) {

		while (CountChars--) {

			if (*BufferAddress > 31 && *BufferAddress != 127) 
				KdPrint(("%c", *BufferAddress));
			else
				KdPrint(("."));

			BufferAddress++;
		}
		KdPrint(("\n"));
	}
	return;
}

