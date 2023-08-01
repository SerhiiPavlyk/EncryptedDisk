#include "main/pch.h"
#include "MountedDisk/MountedDisk.h"

VOID InitMountDisk(PDRIVER_OBJECT DriverObject, UINT32 devId, UINT32 totalLength, PMOUNTEDDISK disk)
{
	IrpHandlerInit(devId, totalLength, DriverObject, disk);
	KernelCustomEventInit(FALSE, &disk->irpQueueNotEmpty_);
	initProtectedVectorAddEvent(&disk->irpQueue_, 1, &disk->irpQueueNotEmpty_);
	KernelCustomEventInit(FALSE, &disk->stopEvent_);
	//disk->pIrp = NULL;
	disk->fileSize.QuadPart = totalLength;

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
}

VOID MDCreateDisk(PMOUNTEDDISK disk, PIRP Pirp)
{
	OBJECT_ATTRIBUTES objectAttributes;

	FILE_END_OF_FILE_INFORMATION    file_eof;
	FILE_BASIC_INFORMATION          file_basic;
	FILE_STANDARD_INFORMATION       file_standard;
	FILE_ALIGNMENT_INFORMATION      file_alignment;
	IO_STATUS_BLOCK block;
	DbgBreakPoint();
	/*strcpy(OpenFileInformation->FileName, "\\??\\UNC");
	strcat(OpenFileInformation->FileName, FileName + 1);*/
	WCHAR* buff = L"\\??\\H:\\filedisk.txt";
	UNICODE_STRING name;
	
	RtlInitUnicodeString(&name, buff);
	InitializeObjectAttributes(&objectAttributes, &name, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, NULL);
	NTSTATUS status = ZwCreateFile(
		&disk->fileHandle,
		GENERIC_READ | GENERIC_WRITE |
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		&objectAttributes,
		&block, // ÕÇ שמ עמ 
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE |
		FILE_RANDOM_ACCESS |
		FILE_NO_INTERMEDIATE_BUFFERING |
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0
	);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "ZwCreateFile failed");
		return;
	}
	status = ZwQueryInformationFile(
		disk->fileHandle,
		&Pirp->IoStatus,
		&file_basic,
		sizeof(FILE_BASIC_INFORMATION),
		FileBasicInformation
	);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "ZwQueryInformationFile 1 failed");
		return;
	}
	status = ZwQueryInformationFile(
		disk->fileHandle,
		&Pirp->IoStatus,
		&file_standard,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation
	);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "ZwQueryInformationFile 2 failed");
		return;
	}
	disk->fileSize.QuadPart = file_standard.EndOfFile.QuadPart;
	status = ZwQueryInformationFile(
		disk->fileHandle,
		&Pirp->IoStatus,
		&file_alignment,
		sizeof(FILE_ALIGNMENT_INFORMATION),
		FileAlignmentInformation
	);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "ZwQueryInformationFile 3 failed");
		return;
	}
	disk->irpDispatcher.deviceObject_->AlignmentRequirement = file_alignment.AlignmentRequirement;
	ZwClose(disk->fileHandle);
}

NTSTATUS MountedDiskDispatchIrp(PIRP irp, PMOUNTEDDISK disk)
{
	IrpParam irpParam;
	irpParam.buffer = 0;
	irpParam.offset = 0;
	irpParam.size = 0;
	irpParam.type = 0;
	IrpHandlerGetIrpParam(irp, &irpParam);
	NTSTATUS status = STATUS_SUCCESS;
	if (irpParam.type == directOperationEmpty)
	{
		
		status = IrpHandlerdispatch(irp,disk);
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
	else if (irpParam.type == directOperationRead)
	{
		MDCreateDisk(disk, irp);
		PUCHAR              system_buffer;
		PUCHAR              buffer;
		system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		if (system_buffer == NULL)
		{
			irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
			irp->IoStatus.Information = 0;
			status = STATUS_UNSUCCESSFUL;
			return status;
		}
		buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, irpParam.size, 'XXX');
		if (buffer == NULL)
		{
			irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
			irp->IoStatus.Information = 0;
			status = STATUS_UNSUCCESSFUL;
			return status;
		}
		status = ZwReadFile(
			disk->fileHandle,
			NULL,
			NULL,
			NULL,
			&irp->IoStatus,
			buffer,
			irpParam.size,
			&irpParam.offset,
			NULL
		);
		RtlCopyMemory(system_buffer, buffer, irpParam.size);
		ExFreePool(buffer);
		return status;
	}
	else if (irpParam.type == directOperationWrite)
	{
		MDCreateDisk(disk, irp);
		PUCHAR              system_buffer;
		PUCHAR              buffer;
		/*if (irpParam.offset +
			irpParam.size) >
			device_extension->file_size.QuadPart)
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
			break;
		}*/
		status = ZwWriteFile(
			disk->fileHandle,
			NULL,
			NULL,
			NULL,
			&irp->IoStatus,
			MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority),
			irpParam.size,
			&irpParam.offset,
			NULL
		);
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