#include "main/pch.h"
#include "MountedDisk/MountedDisk.h"

void InitMountDisk(PDRIVER_OBJECT DriverObject,
	UINT32 devId,
	UINT32 totalLength,
	PMOUNTEDDISK disk)
{
	IrpHandlerInit(devId, totalLength, DriverObject, disk);
	initProtectedVectorAddEvent(&disk->irpQueue_, 1, &disk->irpQueueNotEmpty_);
	disk->pIrp = NULL;
}

void DesctructorMountDisk(PMOUNTEDDISK disk)
{
	//set(&disk->stopEvent_);
	if (disk->pIrp)
		MountedDiskCompleteLastIrp(STATUS_DEVICE_NOT_READY, 0, disk);

	while (pop(&disk->irpQueue_))
		MountedDiskCompleteLastIrp(STATUS_DEVICE_NOT_READY, 0, disk);
}

NTSTATUS MountedDiskDispatchIrp(PIRP irp, PMOUNTEDDISK disk)
{
	IrpParam irpParam;
	irpParam.buffer = 0;
	irpParam.offset = 0;
	irpParam.size = 0;
	irpParam.type = 0;

	IrpHandlerGetIrpParam(irp, &irpParam);
	if (irpParam.type == directOperationEmpty)
	{
		NTSTATUS status = STATUS_SUCCESS;
		status = IrpHandlerdispatch(irp);
		if (status != STATUS_SUCCESS)
		{
			DbgPrintEx(0, 0, "IrpHandlerdispatch failed");
			return status;
		}
		status = irp->IoStatus.Status;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
	}
	IoMarkIrpPending(irp);
	push_back(&disk->irpQueue_, irp);
	return STATUS_PENDING;
}

void MountedDiskCompleteLastIrp(NTSTATUS status, ULONG information, PMOUNTEDDISK disk)
{
	disk->pIrp->IoStatus.Status = status;
	disk->pIrp->IoStatus.Information = information;
	IoCompleteRequest(disk->pIrp, IO_NO_INCREMENT);
	disk->pIrp = 0;
}