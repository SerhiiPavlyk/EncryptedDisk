#include "main/pch.h"
#include "MountedDisk/MountedDisk.h"

NTSTATUS MountedDiskDispatchIrp(PIRP irp)
{
	IrpParam irpParam;
	irpParam.buffer = 0;
	irpParam.offset = 0;
	irpParam.size = 0;
	irpParam.type = 0;
	//DbgBreak()Point();
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
	//irpQueue_.push(irp);
	return STATUS_PENDING;
}
