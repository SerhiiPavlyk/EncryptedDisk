#include "main/pch.h"
#include "VirtualDisk/VirtualDisk.h"

NTSTATUS CreateVirtualDisk(/*struct Disk*/)
{
	VIRTUALDISK disk;
	OBJECT_ATTRIBUTES objectAttributes;

	/*LARGE_INTEGER value;
	value.QuadPart = 20LL * 1024 * 1024;*/

	NTSTATUS status = STATUS_SUCCESS;
	HANDLE fileHandle;              // ���������� ����� ������������ �����

	//OBJECT_ATTRIBUTES objectAttributes;      // �������� ������� ��� �������� �����
	IO_STATUS_BLOCK ioStatusBlock;        // ���� ��������� �������� �����-������

	UNICODE_STRING dirName;

	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);

	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	DbgBreakPoint();

	// �������� ����� ������������ �����
	status = ZwCreateFile(&fileHandle,              // 
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,          // ������ ��� ������ � ������
		&objectAttributes,                    // 
		&ioStatusBlock,                      // 
		&disk.size.QuadPart,                          // ������ ����� - ��������� ����� �������� ����� ������������� �������
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,      // �������� ����� (������� ���� � ������� �������)
		0,                            // ��� ������ ����� - �������� ���� �������� �����-�� ����������
		FILE_CREATE,                      // ���� ���� �� ����������, �������
		FILE_NON_DIRECTORY_FILE,                // ������� �����, �� �������� (����� � ���� ����� ����������� ��� kernel mode)
		NULL,                          // �����-���� ��������� ������������ �����
		0);                            // �����-���� �������� ������������ �����

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
		return status;
	}


	//// ������ ����������� ���������� � ���� ������������ �����
	//status = ZwWriteFile(fileHandle,        // ���������� �����
	//  NULL,                    // ����� �������
	//  NULL,                    // �����-�� APC-������ 
	//  NULL,                    // �������� ���� �� APC-������ 
	//  &ioStatusBlock,                // 
	//  directoryData.Buffer,            // ��������� �� ����� � ������� ��� ������ 
	//  directoryData.Length,            // ������ ������ 
	//  NULL,                    // ������� ��� ������
	//  NULL);                    // �� ����� ��� ���))

	//if (status!=STATUS_SUCCESS) {
	//  
	//  DbgPrintEx(0,0,"Write to disk FAILED!\n");
	//  ZwClose(fileHandle);                

	//  return status;
	//}

	ZwClose(fileHandle);
	return status;
}

NTSTATUS DispatchIrp(UINT32 devId, PIRP irp)
{
	NTSTATUS status = STATUS_SUCCESS;

	{
		ExAcquireFastMutex(&fastMtx);

		ExReleaseFastMutex(&fastMtx);
	}
	return status;

}




NTSTATUS IrpHandlerDisk(UINT32 devId, UINT64 totalLength, PDRIVER_OBJECT DriverObject, PMountManager mountManager)
{

	irpStruct.devId_ = devId;
	irpStruct.totalLength_ = totalLength;

	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING deviceName;
	WCHAR device_name_buffer[50];
	PDEVICE_OBJECT deviceObject;

	//form device name
	//swprintf(device_name_buffer, DIRECT_DISK_PREFIX L"%u", devId);
	RtlInitUnicodeString(&deviceName, device_name_buffer);

	//create device
	status = IoCreateDevice(DriverObject,
		sizeof(DeviceId),
		&deviceName,
		FILE_DEVICE_DISK,
		0,
		FALSE,
		&deviceObject);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "FUNCTION can't create device.");
		return STATUS_UNSUCCESSFUL;
	}

	irpStruct.deviceObject_ = deviceObject;

	DeviceId* devExt = (DeviceId*)deviceObject->DeviceExtension;
	memset(devExt, 0, sizeof(DeviceId));

	devExt->deviceId = devId;

	deviceObject->Flags |= DO_DIRECT_IO;
	deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return status;
}

NTSTATUS deleteDevice()
{
	IoDeleteDevice(irpStruct.deviceObject_);
	DbgPrintEx(0, 0, "deleteDevice() - device successfully deleted!\n");
	return STATUS_SUCCESS;
}







NTSTATUS PrintDisk(char Letter)
{
	for (int i = 0; i < MAX_SIZE; ++i)
	{
		if (Letter == DiskList[i].Letter)
		{
			DbgPrintEx(0, 0, "Disk successfully found!\n");
			DbgPrintEx(0, 0, "Disk LETTER: %c\n", DiskList[i].Letter);
			DbgPrintEx(0, 0, "Disk FileName: %wZ\n", DiskList[i].FileName);
			DbgPrintEx(0, 0, "Disk ID: %d\n", DiskList[i].devID.deviceId);
			DbgPrintEx(0, 0, "Disk SIZE: %lld\n", DiskList[i].size.QuadPart);

			return STATUS_SUCCESS;
		}
	}

	DbgPrintEx(0, 0, "Disk NOT FOUND!\n");
	return STATUS_INVALID_PARAMETER;
}