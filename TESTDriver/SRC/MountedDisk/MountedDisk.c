#include "main/pch.h"
#include "MountedDisk/MountedDisk.h"

VOID InitMountDisk(PDRIVER_OBJECT DriverObject, UINT32 devId, UINT32 totalLength, PMOUNTEDDISK disk)
{
	IrpHandlerInit(devId, totalLength, DriverObject, disk);
	KernelCustomEventInit(FALSE, &disk->irpQueueNotEmpty_);
	initProtectedVectorAddEvent(&disk->irpQueue_, 1, &disk->irpQueueNotEmpty_);
	KernelCustomEventInit(FALSE, &disk->stopEvent_);
	disk->pIrp = NULL;
}

VOID DesctructorMountDisk(PMOUNTEDDISK disk)
{
	set(&disk->stopEvent_);
	if (disk->pIrp)
		MountedDiskCompleteLastIrp(STATUS_DEVICE_NOT_READY, 0, disk);

	while (pop(&disk->irpQueue_, disk->pIrp))
		MountedDiskCompleteLastIrp(STATUS_DEVICE_NOT_READY, 0, disk);

	deleteDevice(disk);
	destroy(&disk->irpQueue_);
	ExFreePoolWithTag(disk->FileName.Buffer, 'MYVC');
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
			ASSERT(FALSE);
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
	ASSERT(disk->pIrp);
	disk->pIrp->IoStatus.Status = status;
	disk->pIrp->IoStatus.Information = information;
	IoCompleteRequest(disk->pIrp, IO_NO_INCREMENT);
	disk->pIrp = 0;
}

VOID MountedDiskRequestExchange(UINT32 lastType, UINT32 lastStatus, UINT32 lastSize, char* buf, UINT32* type, UINT32* length, UINT32* offset, PMOUNTEDDISK disk)
{
	if (lastType != directOperationEmpty)
	{
		ASSERT(disk->pIrp);
		disk->pIrp->IoStatus.Status = lastStatus;
		if (lastStatus == STATUS_SUCCESS && (DiskOperationType)lastType == directOperationRead)
		{
			IrpParam irpParam;
			irpParam.buffer = 0;
			irpParam.offset = 0;
			irpParam.size = 0;
			irpParam.type = 0;
			IrpHandlerGetIrpParam(disk->pIrp, &irpParam);
			if (irpParam.buffer)
				memcpy(irpParam.buffer, buf, lastSize);
			else
				disk->pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		}
		MountedDiskCompleteLastIrp(disk->pIrp->IoStatus.Status, lastSize, disk);
	}
	ASSERT(!disk->pIrp);
	*type = directOperationEmpty;
	if (&disk->irpQueueNotEmpty_ == NULL || &disk->stopEvent_ == NULL ||
		!KeReadStateEvent(&disk->irpQueueNotEmpty_) || !KeReadStateEvent(&disk->stopEvent_))
	{
		return;
	}
	PVOID eventsArray[] = { &disk->irpQueueNotEmpty_, &disk->stopEvent_ };
	NTSTATUS status = KeWaitForMultipleObjects(sizeof(eventsArray) / sizeof(PVOID), eventsArray, WaitAny,
		Executive, KernelMode, FALSE, NULL, 0);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "KeWaitForMultipleObjects failed");
		return;
	}
	while (pop(&disk->irpQueue_, disk->pIrp))
	{
		IrpParam irpParam;
		irpParam.buffer = 0;
		irpParam.offset = 0;
		irpParam.size = 0;
		irpParam.type = 0;
		IrpHandlerGetIrpParam(disk->pIrp, &irpParam);
		*type = irpParam.type;
		*length = irpParam.size;
		*offset = irpParam.offset;
		if (*type != directOperationEmpty)
			break;
		ASSERT(FALSE);
	}
	if (*type != directOperationEmpty && (DiskOperationType)*type == directOperationWrite)
	{
		IrpParam irpParam;
		irpParam.buffer = 0;
		irpParam.offset = 0;
		irpParam.size = 0;
		irpParam.type = 0;
		IrpHandlerGetIrpParam(disk->pIrp, &irpParam);

		if (irpParam.buffer)
			memcpy(buf, irpParam.buffer, *length);
		else
		{
			MountedDiskCompleteLastIrp(STATUS_INSUFFICIENT_RESOURCES, 0, disk);
			*type = directOperationEmpty;
		}
	}
}