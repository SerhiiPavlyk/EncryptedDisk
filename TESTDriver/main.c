#pragma warning (disable:28251)

#include "IRP.h"
//#include "ControlDeviceIrpHandler.h"
#include <winsmcrd.h>
#include <handleapi.h>

#include <wdf.h>

//pointer on device object
PDEVICE_OBJECT gDeviceObject = NULL;

UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\DEVICE_TEST_NAME");

UNICODE_STRING gSymbolicLinkName = RTL_CONSTANT_STRING(L"\\Device\\Symbolic_Link_Name_TEST");

OBJECT_ATTRIBUTES objectAttributes;

//
//NTSTATUS IrpHandler(IN PDRIVER_OBJECT DriverObject, IN PIRP pIrp)
//{
//	//проверяем, является ли объект устройства равным gDeviceObject
//	try
//	{
//		if (DriverObject == gDeviceObject) {
//
//		}
//
//	}
//	catch (const std::exception& ex)
//	{
//
//	}
//}

//NTSTATUS dispatch(PIRP irp)
//{
//	PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(irp);
//	switch (io_stack->MajorFunction)
//	{
//	case IRP_MJ_CREATE:
//	case IRP_MJ_CLOSE:
//		irp->IoStatus.Status = STATUS_SUCCESS;
//		irp->IoStatus.Information = 0;
//		break;
//	case IRP_MJ_QUERY_VOLUME_INFORMATION:
//		KdPrint((__FUNCTION__" IRP_MJ_QUERY_VOLUME_INFORMATION\n"));
//		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
//		irp->IoStatus.Information = 0;
//		break;
//	case IRP_MJ_DEVICE_CONTROL:
//		dispatchIoctl(irp);
//		break;
//	default:
//		KdPrint((__FUNCTION__"Unknown MJ fnc = 0x%x\n", io_stack->MajorFunction));
//	}
//}

//NTSTATUS IrpHandler(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
//{
//
//
//	NTSTATUS status = STATUS_SUCCESS;
//	if (pDeviceObject == gDeviceObject)
//	{
//		return ControlDeviceIrpHandler(pDeviceObject, pIrp);
//	}
//
//	//=================нужно доделать=================
//		//DiskDevExt* devExt = (DiskDevExt*) pDeviceObject->DeviceExtension;
//
//		//NTSTATUS status = gMountManager->DispatchIrp(devExt->deviceId, pIrp);
//
//	if (status != STATUS_SUCCESS)
//	{
//		KdPrint((__FUNCTION__ " Error: 0x%X\n", status));
//		//return CompleteIrp(pIrp, STATUS_NO_SUCH_DEVICE, 0);
//	}
//
//	return status;
//}

VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	IoDeleteSymbolicLink(&gSymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);

	ZwDeleteFile(&objectAttributes);

	DbgPrint("[DriverTest]: DriverEntry unload\n");
}

NTSTATUS CreateVirtualDisk(PUNICODE_STRING DiskName, PUNICODE_STRING DirectoryName);



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = UnloadDriver;
	NTSTATUS status = STATUS_SUCCESS;
	DbgPrintEx(0, 0, "[DriverTest]: DriverEntry load\n");
	DbgPrintEx(0, 0, "[DriverTest]: Breakpoint\n");

	DbgBreakPoint();

	UNICODE_STRING diskName = RTL_CONSTANT_STRING(L"FuckingDisk");

	UNICODE_STRING dir = RTL_CONSTANT_STRING(L"C:\\NewFolder\\");

	status = CreateVirtualDisk(&diskName, &dir);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("CreateVirtualDisk fail!\n");
		return STATUS_UNSUCCESSFUL;
	}

	status = IoCreateDevice(DriverObject,		// pointer on DriverObject
		0,					// additional size of memory
		&gDeviceName,		// pointer to UNICODE_STRING
		FILE_DEVICE_DISK,	// Device type
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

	//Register IRP handlers
	PDRIVER_DISPATCH* mj_func;
	mj_func = DriverObject->MajorFunction;

	for (size_t i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i)
	{
		DriverObject->MajorFunction[i] = dispatch_irp;
	}

	return status;

	/* Driver initialization are done */
	//================================ПРОДОЛЖИТЬ в Барде================================
	//gMountManager = new MountManager(DriverObject);
	//================================ПРОДОЛЖИТЬ в Барде================================

}

#define ROOT_DIR_NAME        L"\\??\\H:\\DISK"


NTSTATUS CreateVirtualDisk(PUNICODE_STRING DiskName, PUNICODE_STRING DirectoryName)
{
	UNREFERENCED_PARAMETER(DirectoryName);
	UNREFERENCED_PARAMETER(DiskName);
	LARGE_INTEGER value;
	value.QuadPart = 20LL * 1024 * 1024;


	NTSTATUS status = STATUS_SUCCESS;
	HANDLE fileHandle;							// Дескриптор файла виртуального диска

			// Атрибуты объекта для открытия файла
	IO_STATUS_BLOCK ioStatusBlock;				// Блок состояния операции ввода-вывода

	UNICODE_STRING dirName;

	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);

	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	DWORD   BytesReturned;
	DbgBreakPoint();
	status = ZwCreateFile(&fileHandle,								// 
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,					// доступ для чтения и записи
		&objectAttributes,										// 
		&ioStatusBlock,											// 
		&value,													// размер файла - ПРОДУМАТЬ ПОТОМ СОЗДАНИЕ ДИСКА ОПРЕДЕЛЕННОГО РАЗМЕРА
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,			// атрибуты файла (обычный файл и скрытый атрибут)
		0,														// это защита файла - ВОЗМОЖНО СЮДА ВПИХНУТЬ КАКОЕ-ТО ШИФРОВАНИЕ
		FILE_CREATE,											// если файл не существует, создаем
		FILE_NON_DIRECTORY_FILE,								// открыть файлы, не каталоги
		NULL,													// какие-либо параметры создаваемого файла
		0);

	if (status != STATUS_SUCCESS)
	{
		DbgBreakPoint();
		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
		return status;
	}

	return status;
}

