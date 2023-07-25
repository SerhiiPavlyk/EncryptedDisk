#include "pch.h"
#include "MountManager.h"

void Init()
{
	ExInitializeFastMutex(&diskMapLock_);

}

NTSTATUS DispatchIrp(int devId, PIRP irp)
{
	PVIRTUALDISK disk;

	ExAcquireFastMutex(&diskMapLock_);

	for (size_t i = 0; i < AmountOfDisks; i++)
	{
		if (devId == DiskList[i].devID.deviceId)
		{
			devId = i;
		}
		if (devId + 1 == AmountOfDisks)
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
