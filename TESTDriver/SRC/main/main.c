#include "main/pch.h"

#include "IRP/IRP.h"

//pointer on device object

PDEVICE_OBJECT gDeviceObject = NULL;

UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\DEVICE_TEST_NAME");

UNICODE_STRING gSymbolicLinkName = RTL_CONSTANT_STRING(L"\\Device\\Symbolic_Link_Name_TEST");


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
		status = dispatch_irp(fdo, pIrp);
	}

	DeviceId* devExt = (DeviceId*)fdo->DeviceExtension;
	status = MountManagerDispatchIrp(devExt->deviceId, pIrp);
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


	/*status = CreateVirtualDisk();
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("CreateVirtualDisk fail!\n");
		return STATUS_UNSUCCESSFUL;
	}*/

	status = IoCreateDevice(DriverObject,		// pointer on DriverObject
		0,					// additional size of memory
		&gDeviceName,		// pointer to UNICODE_STRING
		FILE_DEVICE_NULL,	// Device type
		0,					// Device characteristic
		FALSE,				// "Exclusive" device
		&gDeviceObject);	// pointer do device object

	if (status != STATUS_SUCCESS)
	{
		DbgPrint("IoCreateDevice fail!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	status = IoCreateSymbolicLink(&gSymbolicLinkName, &gDeviceName);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("IoCreateSymbolicLink fail!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	for (size_t i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = IrpHandler;
	}

	MountManagerInit(DriverObject);
	return status;
}



