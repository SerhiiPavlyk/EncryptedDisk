#include "main/pch.h"
#include "MountManager/MountManager.h"


NTSTATUS MountManagerInit(PDRIVER_OBJECT DriverObject)
{
	DataOfMountManager.DriverObject = DriverObject;
	DataOfMountManager.gMountedDiskCount = 0;
	ExInitializeFastMutex(&DataOfMountManager.diskMapLock_);
	DataOfMountManager.isInitializied = TRUE;

	//NTSTATUS          status;
	//UNICODE_STRING    device_dir_name;
	//OBJECT_ATTRIBUTES object_attributes;

	//RtlInitUnicodeString(&device_dir_name, ROOT_DIR_NAME);

	//InitializeObjectAttributes(
	//	&object_attributes,
	//	&device_dir_name,
	//	OBJ_OPENIF,
	//	NULL,
	//	NULL
	//);

	//HANDLE dir_handle;
	//status = ZwCreateDirectoryObject(
	//	&dir_handle,
	//	DIRECTORY_ALL_ACCESS,
	//	&object_attributes
	//);

	//if (!NT_SUCCESS(status))
	//	ASSERT(FALSE);
}

NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp)
{
	PMOUNTEDDISK disk;
	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);

	for (UINT32 i = 0; i < DataOfMountManager.gMountedDiskCount; i++)
	{
		if (devId == MountDiskList[i].irpDispatcher.devId_.deviceId)
		{
			devId = i;
		}
		/*if (devId == DataOfMountManager.gMountedDiskCount)
		{
			
			irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_DEVICE_NOT_READY;
		}*/
	}
	disk = MountDiskList + devId;
	ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	//initProtectedVector(&disk->irpQueue_, 1);
	NTSTATUS status = MountedDiskDispatchIrp(irp, disk);
	//destroy(&disk->irpQueue_);
	//DesctructorMountDisk(disk);
	return status;

}


/*NTSTATUS AddVirtualDisk()
{
	VIRTUALDISK disk;
	disk.devID.deviceId = 0;

	NTSTATUS status = STATUS_SUCCESS;

	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		int i = DataOfMountManager.gVirtualDiskCount + 1;

		if (i <= MAX_SIZE - 1)
		{
			VirtDiskList[i] = disk; //�������� ��, ��� disk ���������� ����������, ��� ������ � ��� ����
			disk.devID.deviceId = DataOfMountManager.gVirtualDiskCount++;

			DbgPrintEx(0,0,"VirtualDisk data successfully ADDED in array!\n");
		}
		else
		{
			DbgPrintEx(0,0,"VirtDiskList is full!\n");
			ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
			return STATUS_UNSUCCESSFUL;
		}

		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	return status;
}*/

/*NTSTATUS RemoveVirtualDisk(UINT32 deviceId)      //����� ����, ��� ����� ���� ���������� � ����� ������� �������
{

	if (deviceId < DataOfMountManager.gVirtualDiskCount - 1)
	{

		for (int i = deviceId; i < DataOfMountManager.gVirtualDiskCount - 1; ++i)
		{
			VirtDiskList[deviceId] = VirtDiskList[deviceId + 1];
		}

		DataOfMountManager.gVirtualDiskCount--;
		DbgPrintEx(0,0,"Disk successfully deleted!\n");
		return STATUS_SUCCESS;

	}
	else if (deviceId == DataOfMountManager.gVirtualDiskCount)
	{
		DataOfMountManager.gVirtualDiskCount--;        //������ ��������� ����� ��������� ������, �����
					  // 1. ��� ������ ���� ������ ��������� �� �����������
					  // 2. ��� �������� ������ �����, ������ ����������� ������ ���������� �����, ������� "�������"
					  // P.S. �������� ��� �� ������ ������� :)

		DbgPrintEx(0,0,"Disk successfully deleted!\n");
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0,0,"Invalid parameter - disk NOT FOUND");
		return STATUS_INVALID_PARAMETER;
	}
}*/


int Mount(UINT32 totalLength)
{
	//DbgBreakPoint();
	//generate id
	UINT32 devId = 0;
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		if (DataOfMountManager.gMountedDiskCount < MAX_SIZE - 2)
		{
			devId = DataOfMountManager.gMountedDiskCount++;
		}
		else
		{
			DbgPrintEx(0, 0, "FUNCTION - device ID already exist\n");
		}
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}

	PMOUNTEDDISK disk = (PMOUNTEDDISK)ExAllocatePoolWithTag(NonPagedPool, sizeof(MOUNTEDDISK), "mntDisk");
	if (disk == NULL)
	{
		DbgPrintEx(0, 0, "Failed to mount disk\n");
		return -1;
	}
	InitMountDisk(DataOfMountManager.DriverObject, devId, totalLength, disk);

	//DbgBreak()Point();

	/*UNICODE_STRING gSymbolicLinkName = RTL_CONSTANT_STRING(L"\\Device\\disk0");

	
	NTSTATUS status = IoCreateSymbolicLink(&gSymbolicLinkName, &disk->FileName);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0,0,"IoCreateSymbolicLink fail!\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}*/

	//disk->irpDispatcher.deviceObject_->Size = 2000LL * 1024;
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		int i = devId;

		if (i <= MAX_SIZE - 1)
		{
			MountDiskList[i] = *disk;
			disk->irpDispatcher.devId_.deviceId = devId;
			DbgPrintEx(0, 0, "MountedDisk data successfully ADDED in array!\n");
		}
		else
		{
			DbgPrintEx(0, 0, "VirtDiskList is full!\n");
			ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
			ExFreePoolWithTag(disk, "mntDisk");
			return -1;
		}

		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	//DbgBreakPoint();
	//DesctructorMountDisk(disk);
	ExFreePoolWithTag(disk, "mntDisk");
	return devId;
}

VOID Unmount(UINT32 deviceId)			//����� ����, ��� ����� ���� ���������� � ����� ������� �������
{
	DbgBreakPoint();
	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
	if (deviceId < DataOfMountManager.gMountedDiskCount - 1)
	{
		DesctructorMountDisk(&MountDiskList[deviceId]);
		for (UINT32 i = deviceId; i < DataOfMountManager.gMountedDiskCount - 1; ++i)
		{
			MountDiskList[deviceId] = MountDiskList[deviceId + 1];
		}

		DataOfMountManager.gMountedDiskCount--;
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
		DbgPrintEx(0,0,"Disk successfully deleted!\n");

	}
	else if (deviceId == DataOfMountManager.gMountedDiskCount)
	{

		DataOfMountManager.gMountedDiskCount--;				//������ ��������� ����� ��������� ������, �����
									// 1. ��� ������ ���� ������ ��������� �� �����������
									// 2. ��� �������� ������ �����, ������ ����������� ������ ���������� �����, ������� "�������"
									// P.S. �������� ��� �� ������ ������� :)

		DbgPrintEx(0,0,"Disk successfully deleted!\n");
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0,0,"Invalid parameter - disk NOT FOUND");
		return STATUS_INVALID_PARAMETER;
	}
}


VOID MountManagerRequestExchange(UINT32 devID,
	UINT32 lastType,
	UINT32 lastStatus,
	UINT32 lastSize,
	char* buf,
	UINT32 bufSize,
	UINT32* type,
	UINT32* length,
	UINT32* offset)
{
	PMOUNTEDDISK disk = NULL;
	//DbgPrintEx(0, 0, "MountManagerRequestExchange\n");
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		for (UINT32 i = 0; i < DataOfMountManager.gMountedDiskCount; ++i)
		{
			if (devID == MountDiskList[i].irpDispatcher.devId_.deviceId)
			{
				//DbgPrintEx(0, 0, "RequestExchange() - disk FOUND\n");
				disk = &MountDiskList[i];
				break;
			}
		}
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	if (disk == NULL)
	{
		DbgPrintEx(0,0,"RequestExchange() - Disk NOT FOUND\n");
	}

	MountedDiskRequestExchange(lastType, lastStatus, lastSize, buf, bufSize,
		type, length, offset, disk);
}


