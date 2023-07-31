#include "main/pch.h"

#include "MountManager/MountManager.h"

//pointer on device object

PDEVICE_OBJECT gDeviceObject = NULL;

UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\DEVICE_TEST_NAME");

UNICODE_STRING gSymbolicLinkName = RTL_CONSTANT_STRING(L"\\Device\\Symbolic_Link_Name_TEST");


NTSTATUS dispatch_irp(_In_ PIRP Irp);



VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	IoDeleteSymbolicLink(&gSymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);

}



NTSTATUS IrpHandler(IN PDEVICE_OBJECT fdo, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	if (fdo == gDeviceObject)
	{
		return dispatch_irp(pIrp);
	}
	//DbgBreakPoint();
	PDeviceId devExt = (PDeviceId)fdo->DeviceExtension;
	UINT32 deviceIdValue = devExt->deviceId;
	status = MountManagerDispatchIrp(deviceIdValue, pIrp);
	//DbgBreakPoint();
	if (status != STATUS_SUCCESS)
	{
		status = CompleteIrp(pIrp, STATUS_NO_SUCH_DEVICE, 0);
	}

	return status;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = UnloadDriver;
	NTSTATUS status = STATUS_SUCCESS;

	status = IoCreateDevice(DriverObject,		// pointer on DriverObject
		0,					// additional size of memory
		&gDeviceName,		// pointer to UNICODE_STRING
		FILE_DEVICE_NULL,	// Device type
		0,					// Device characteristic
		FALSE,				// "Exclusive" device
		&gDeviceObject);	// pointer do device object

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "IoCreateDevice fail!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	status = IoCreateSymbolicLink(&gSymbolicLinkName, &gDeviceName);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "IoCreateSymbolicLink fail!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	for (size_t i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = IrpHandler;
	}

	MountManagerInit(DriverObject);
	return status;


	/* Driver initialization are done */
}



NTSTATUS DispatchMount(PVOID buffer, ULONG inputBufferLength, ULONG outputBufferLength)
{
	if (inputBufferLength < sizeof(CoreMNTMountRequest) ||
		outputBufferLength < sizeof(CoreMNTMountResponse))
	{
		DbgPrintEx(0, 0, "DispatchMount() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
	CoreMNTMountRequest* request = (CoreMNTMountRequest*)buffer;
	UINT32 totalLength = (UINT32)request->totalLength;
	CoreMNTMountResponse* response = (CoreMNTMountResponse*)buffer;
	DbgBreakPoint();
	response->deviceId = Mount(totalLength, request->FileName);
	return STATUS_SUCCESS;

}
NTSTATUS DispatchExchange(PVOID buffer, ULONG inputBufferLength, ULONG outputBufferLength)
{

	if (inputBufferLength < sizeof(CoreMNTExchangeRequest) ||
		outputBufferLength < sizeof(CoreMNTExchangeResponse))
	{
		DbgPrintEx(0, 0, "DispatchExchange() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
	CoreMNTExchangeRequest* request = (CoreMNTExchangeRequest*)buffer;

	CoreMNTExchangeResponse response = { 0 };
	MountManagerRequestExchange(request->deviceId,
		request->lastType,
		request->lastStatus,
		request->lastSize,
		request->data,
		&response.type,
		&response.size,
		&(UINT32)response.offset);
	memcpy(buffer, &response, sizeof(response));
	return STATUS_SUCCESS;
}
NTSTATUS DispatchUnmount(PVOID buffer, ULONG inputBufferLength)
{
	DbgBreakPoint();
	if (inputBufferLength < sizeof(CoreMNTUnmountRequest))
	{
		DbgPrintEx(0, 0, "DispatchUnmount() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
	CHAR writeData[256];
	RtlCopyMemory(writeData, buffer, inputBufferLength);
	CoreMNTUnmountRequest request;
	request.deviceId = writeData[0] - '0';
	Unmount(request.deviceId);
	return STATUS_SUCCESS;
}



NTSTATUS dispatch_irp(_In_ PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
	//DbgBreak()Point();
	//disk
	switch (ioStack->MajorFunction)
	{

	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
	case IRP_MJ_CLEANUP:
		return CompleteIrp(Irp, STATUS_SUCCESS, 0);
	case IRP_MJ_DEVICE_CONTROL:
	{
		ULONG code = ioStack->Parameters.DeviceIoControl.IoControlCode;
		PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
		ULONG outputBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
		ULONG inputBufferLength = ioStack->Parameters.DeviceIoControl.InputBufferLength;
		switch (code)
		{
		case CORE_MNT_MOUNT_IOCTL:
			status = DispatchMount(buffer, inputBufferLength, outputBufferLength);
			break;
		case CORE_MNT_EXCHANGE_IOCTL:
			status = DispatchExchange(buffer, inputBufferLength, outputBufferLength);
			break;
		case CORE_MNT_UNMOUNT_IOCTL:
			status = DispatchUnmount(buffer, inputBufferLength);
			break;
		}
		return CompleteIrp(Irp, status, outputBufferLength);
	}

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		DbgPrintEx(0, 0, "Unknown PNP minor function= 0x%x\n", ioStack->MinorFunction);
		break;
	}

	return status;
}
