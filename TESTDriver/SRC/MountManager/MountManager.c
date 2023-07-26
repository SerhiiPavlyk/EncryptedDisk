#include "main/pch.h"
#include "MountManager/MountManager.h"


NTSTATUS MountManagerInit(PDRIVER_OBJECT DriverObject)
{
	DataOfMountManager.DriverObject = DriverObject;
	DataOfMountManager.gDiskCount = 0;
	ExInitializeFastMutex(&DataOfMountManager.diskMapLock_);
	DataOfMountManager.isInitializied = TRUE;
}

NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp)
{
	PVIRTUALDISK disk;

	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);

	for (size_t i = 0; i < DataOfMountManager.gDiskCount; i++)
	{
		if (devId == DiskList[i].devID.deviceId)
		{
			devId = i;
		}
		if (devId + 1 == DataOfMountManager.gDiskCount)
		{
			irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_DEVICE_NOT_READY;
		}
	}
	disk = DiskList + devId;
	ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	return STATUS_SUCCESS;

	//return disk->DispatchIrp(irp);
}

int Mount(UINT64 totalLength)
{
	// generate id
	int devId = 0;
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		if (DataOfMountManager.gDiskCount < 25)
		{
			devId = DataOfMountManager.gDiskCount++;
		}
		else
		{
			DbgPrintEx(0,0, "__FUNCTION__ - device ID already exist.");
		}
	}
	/*boost::shared_ptr<MountedDisk> disk(
		new MountedDisk(driverObject_, this, devId, totalLength));
	AutoMutex guard(diskMapLock_);
	MountedDiskMapPairIB pairIb =
		diskMap_.insert(std::make_pair(devId, disk));
	if (!pairIb.second)
		throw std::exception(__FUNCTION__" - device ID already exist.");
	ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);*/
	return devId;
}

NTSTATUS MountDisk()
{
	VIRTUALDISK disk;
	disk.devID.deviceId = 0;

	NTSTATUS status = STATUS_SUCCESS;

	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		int i = DataOfMountManager.gDiskCount + 1;

		if (i <= MAX_SIZE - 1)
		{
			DiskList[i] = disk; //учитывая то, что disk передается параметром, все данные о нем есть
			disk.devID.deviceId = DataOfMountManager.gDiskCount++;

			DbgPrint("Disk data successfully ADDED!\n");
		}
		else
		{
			DbgPrint("DiskList is full!\n");
			ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
			return STATUS_UNSUCCESSFUL;
		}

		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	return status;
}

NTSTATUS UnmountDisk(UINT32 deviceId)			//ввиду того, что буква Тома выбирается в любом удобном порядке
{

	if (deviceId < DataOfMountManager.gDiskCount - 1)
	{

		for (int i = deviceId; i < DataOfMountManager.gDiskCount - 1; ++i)
		{
			DiskList[deviceId] = DiskList[deviceId + 1];
		}

		DataOfMountManager.gDiskCount--;
		DbgPrint("Disk successfully deleted!\n");
		return STATUS_SUCCESS;

	}
	else if (deviceId == DataOfMountManager.gDiskCount)
	{
		DataOfMountManager.gDiskCount--;				//просто уменьшаем число созданных дисков, чтобы
									// 1. при показе всех дисков последний не показывался
									// 2. при создании нового диска, данные перепишутся поверх последнего диска, который "удалили"
									// P.S. возможно это не лучший вариант :)

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
	PVIRTUALDISK disk = NULL;
	{
		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
		for (int i = 0; i < DataOfMountManager.gDiskCount; ++i)
		{
			if (devID == DiskList[i].devID.deviceId)
			{
				DbgPrintEx(0, 0, "RequestExchange() - disk FOUND\n");
				disk = &DiskList[i];
				break;
			}
		}
		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
	}
	if (disk == NULL)
	{
		DbgPrint("RequestExchange() - Disk NOT FOUND\n");
	}

	//а дальше то че????
}