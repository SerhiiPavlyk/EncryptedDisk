#include "main/pch.h"
#include "MountManager/MountManager.h"


NTSTATUS MountManagerInit(PDRIVER_OBJECT DriverObject)
{
	DataOfMountManager.DriverObject = DriverObject;
	DataOfMountManager.gMountedDiskCount = 0;
	ExInitializeFastMutex(&DataOfMountManager.diskMapLock_);
	DataOfMountManager.isInitializied = TRUE;
}

NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp)
{
	PMOUNTEDDISK disk;

	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);

	for (INT32 i = 0; i < DataOfMountManager.gMountedDiskCount; i++)
	{
		if (devId == MountDiskList[i].irpDispatcher.devId_.deviceId)
		{
			devId = i;
		}
		if (devId + 1 == DataOfMountManager.gMountedDiskCount)
		{
			irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_DEVICE_NOT_READY;
		}
	}
	disk = MountDiskList + devId;
	ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	return STATUS_SUCCESS;

	//return disk->DispatchIrp(irp);
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


int Mount(UINT64 totalLength)
{
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
	}

	PMOUNTEDDISK disk = (PMOUNTEDDISK)ExAllocatePoolWithTag(NonPagedPool, sizeof(MOUNTEDDISK), "mntDisk");
	if (disk == NULL)
	{
		DbgPrintEx(0, 0, "Failed to mount disk\n");
		return -1;
	}

	disk->irpDispatcher.totalLength_ = totalLength;
	disk->irpDispatcher.deviceObject_ = DataOfMountManager.DriverObject; // ?????
	disk->irpDispatcher.devId_.deviceId = devId;
	//disk->FileName=                   =================� ��� ������ ���-�� ������?=============

	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		int i = DataOfMountedDisk.gMountedDiskCount + 1;

		if (i <= MAX_SIZE - 1)
		{
			MountDiskList[i] = *disk;
			disk->irpDispatcher.devId_.deviceId = DataOfMountedDisk.gMountedDiskCount++;
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
	ExFreePoolWithTag(disk, "mntDisk");
	return devId;
}

VOID Unmount(UINT32 deviceId)			//����� ����, ��� ����� ���� ���������� � ����� ������� �������
{
	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
	if (deviceId < DataOfMountManager.gMountedDiskCount - 1)
	{

		for (INT32 i = deviceId; i < DataOfMountManager.gMountedDiskCount - 1; ++i)
		{
			MountDiskList[deviceId] = MountDiskList[deviceId + 1];
		}

		DataOfMountManager.gMountedDiskCount--;
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
		DbgPrint("Disk successfully deleted!\n");

	}
	else if (deviceId == DataOfMountManager.gMountedDiskCount)
	{
		DataOfMountManager.gMountedDiskCount--;				//������ ��������� ����� ��������� ������, �����
									// 1. ��� ������ ���� ������ ��������� �� �����������
									// 2. ��� �������� ������ �����, ������ ����������� ������ ���������� �����, ������� "�������"
									// P.S. �������� ��� �� ������ ������� :)

		DbgPrint("Disk successfully deleted!\n");
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrint("Invalid parameter - disk NOT FOUND");
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
	UINT64* offset)
{
	PMOUNTEDDISK disk = NULL;
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		for (INT32 i = 0; i < DataOfMountManager.gMountedDiskCount; ++i)
		{
			if (devID == MountDiskList[i].irpDispatcher.devId_.deviceId)
			{
				DbgPrintEx(0, 0, "RequestExchange() - disk FOUND\n");
				disk = &MountDiskList[i];
				break;
			}
		}
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	if (disk == NULL)
	{
		DbgPrint("RequestExchange() - Disk NOT FOUND\n");
	}

	//� ������ �� ��????
}