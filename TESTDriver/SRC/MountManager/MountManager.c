#include "main/pch.h"
#include "MountManager/MountManager.h"


NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp)
{
	PVIRTUALDISK disk;

	ExAcquireFastMutex(&diskMapLock_);

	for (size_t i = 0; i < gDiskCount; i++)
	{
		if (devId == DiskList[i].devID.deviceId)
		{
			devId = i;
		}
		if (devId + 1 == gDiskCount)
		{
			irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return STATUS_DEVICE_NOT_READY;
		}
	}
	disk = DiskList + devId;
	ExReleaseFastMutex(&diskMapLock_);
	return STATUS_SUCCESS;


	//return disk->DispatchIrp(irp);

}

NTSTATUS MountDisk(VIRTUALDISK disk)
{
	ExInitializeFastMutex(&diskMapLock_);
	NTSTATUS status = STATUS_SUCCESS;
	{
		ExAcquireFastMutex(&diskMapLock_);
		int i = gDiskCount + 1;

		if (i <= MAX_SIZE - 1)
		{
			DiskList[i] = disk; //учитывая то, что disk передается параметром, все данные о нем есть
			disk.devID.deviceId = gDiskCount++;

			DbgPrint("Disk data successfully ADDED!\n");
		}
		else
		{
			DbgPrint("DiskList is full!\n");
			ExReleaseFastMutex(&diskMapLock_);
			return STATUS_UNSUCCESSFUL;
		}

		ExReleaseFastMutex(&diskMapLock_);
	}
	return status;
}

NTSTATUS UnmountDisk(UINT32 deviceId)			//ввиду того, что буква Тома выбирается в любом удобном порядке
{

	if (deviceId < gDiskCount - 1)
	{

		for (int i = deviceId; i < gDiskCount - 1; ++i)
		{
			DiskList[deviceId] = DiskList[deviceId + 1];
		}

		gDiskCount--;
		DbgPrint("Disk successfully deleted!\n");
		return STATUS_SUCCESS;

	}
	else if (deviceId == gDiskCount)
	{
		gDiskCount--;				//просто уменьшаем число созданных дисков, чтобы
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


VOID RequestExchange(UINT32 devID,
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
		ExAcquireFastMutex(&diskMapLock_);
		for (int i = 0; i < gDiskCount; ++i)
		{
			if (devID == DiskList[i].devID.deviceId)
			{
				DbgPrintEx(0, 0, "RequestExchange() - disk FOUND\n");
				disk = &DiskList[i];
				break;
			}
		}
		ExReleaseFastMutex(&diskMapLock_);
	}
	if (disk == NULL)
	{
		DbgPrint("RequestExchange() - Disk NOT FOUND\n");
	}

	//а дальше то че????
}