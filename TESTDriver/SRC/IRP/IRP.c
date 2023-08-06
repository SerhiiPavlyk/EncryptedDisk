#include "main/pch.h"
#include "IRP/IRP.h"

#include <ntstrsafe.h>

//NTSTATUS IrpHandlerInit(UINT32 devId, UINT32 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk)
//{
//	IrpData.devId_.deviceId = devId;
//	IrpData.totalLength_ = totalLength;
//	PDEVICE_OBJECT deviceObject = NULL;
//	NTSTATUS status = STATUS_SUCCESS;
//	WCHAR device_name_buffer[MAXIMUM_FILENAME_LENGTH];
//	RtlStringCbPrintfW(device_name_buffer,
//		sizeof(device_name_buffer),
//		DIRECT_DISK_PREFIX 	L"%u",
//		IrpData.devId_.deviceId);
//	//form device name
//	size_t fileNameSize = (wcslen(device_name_buffer) + 1) * sizeof(wchar_t);
//
//	// Allocate memory for Mdisk->FileName.Buffer
//	Mdisk->DeviceName.Buffer = ExAllocatePool(PagedPool, fileNameSize);
//	if (Mdisk->DeviceName.Buffer == NULL)
//	{
//		DbgPrintEx(0, 0, "Failed to allocate memory for Mdisk->DeviceName.");
//		return STATUS_INSUFFICIENT_RESOURCES;
//	}
//
//	// Copy the contents of FileName into Mdisk->FileName.Buffer
//	Mdisk->DeviceName.Length = (USHORT)(fileNameSize - sizeof(wchar_t)); // Exclude null terminator
//	Mdisk->DeviceName.MaximumLength = (USHORT)fileNameSize; // Include null terminator
//	wcscpy_s(Mdisk->DeviceName.Buffer, Mdisk->DeviceName.MaximumLength / sizeof(wchar_t), device_name_buffer);
//
//
//	//RtlInitUnicodeString(&deviceName, device_name_buffer);
//
//	ULONG size = sizeof(MOUNTEDDISK);
//	//create device
//	status = IoCreateDevice(DriverObject,
//		size,
//		&Mdisk->DeviceName,
//		FILE_DEVICE_DISK,
//		0,
//		FALSE,
//		&deviceObject);
//
//	if (status != STATUS_SUCCESS)
//	{
//		DbgPrintEx(0, 0, "FUNCTION can't create device.");
//		return STATUS_UNSUCCESSFUL;
//	}
//
//	IrpData.deviceObject_ = deviceObject;
//	//DbgBreakPoint();
//	DeviceId* devExt = (DeviceId*)deviceObject->DeviceExtension;
//	memset(devExt, 0, sizeof(DeviceId));
//
//	devExt->deviceId = devId;
//	deviceObject->Flags |= DO_DIRECT_IO;
//	deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
//	Mdisk->irpDispatcher = IrpData;
//	return status;
//}
//
//void IrpHandlerGetIrpParam(PIRP irp, IrpParam* irpParam)
//{
//	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
//	irpParam->offset = 0;
//	irpParam->type = directOperationEmpty;
//	irpParam->buffer = (char*)getIrpBuffer(irp);
//	if (ioStack->MajorFunction == IRP_MJ_READ)
//	{
//		irpParam->type = directOperationRead;
//		irpParam->size = ioStack->Parameters.Read.Length;
//		irpParam->offset = (UINT32)ioStack->Parameters.Read.ByteOffset.QuadPart;
//	}
//	else
//		if (ioStack->MajorFunction == IRP_MJ_WRITE)
//		{
//			irpParam->type = directOperationWrite;
//			irpParam->size = ioStack->Parameters.Write.Length;
//			irpParam->offset = (UINT32)ioStack->Parameters.Write.ByteOffset.QuadPart;
//		}
//	return;
//}
//
//NTSTATUS IrpHandlerdispatch(PIRP irp, PMOUNTEDDISK disk)
//{
//	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
//	NTSTATUS status = STATUS_SUCCESS;
//	//DbgBreakPoint();
//	switch (ioStack->MajorFunction)
//	{
//		/*case IRP_MJ_PNP:
//			DbgPrintEx(0, 0, ("IRP_MJ_PNP\n"));
//			irp->IoStatus.Status = STATUS_SUCCESS;
//			irp->IoStatus.Information = 0;
//			break;
//		case IRP_MJ_CLEANUP:*/
//		/*	return CompleteIrp(irp, STATUS_SUCCESS, 0);
//			break;
//		case IRP_MJ_QUERY_INFORMATION:
//			DbgPrintEx(0, 0, (__FUNCTION__" IRP_MJ_QUERY_INFORMATION\n"));
//			irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
//			irp->IoStatus.Information = 0;
//			status = STATUS_UNSUCCESSFUL;
//			break;*/
//
//
//	case IRP_MJ_CREATE:
//	case IRP_MJ_CLOSE:
//		irp->IoStatus.Status = STATUS_SUCCESS;
//		irp->IoStatus.Information = 0;
//		break;
//	case IRP_MJ_QUERY_VOLUME_INFORMATION:
//		DbgPrintEx(0, 0, ("IRP_MJ_QUERY_VOLUME_INFORMATION\n"));
//		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
//		irp->IoStatus.Information = 0;
//		status = STATUS_UNSUCCESSFUL;
//		break;
//	case IRP_MJ_DEVICE_CONTROL:
//		dispatchIoctl(irp, disk);
//		//status = irp->IoStatus.Status;
//		if (status != STATUS_SUCCESS)
//		{
//			DbgPrintEx(0, 0, "dispatchIoctl fail  IRP = 100 line");
//		}
//
//		break;
//	default:
//		DbgPrintEx(0, 0, "Unknown MJ fnc = 0x%02X\n", ioStack->MajorFunction);
//
//	}
//	return status;
//}
//
//NTSTATUS deleteDevice(PMOUNTEDDISK disk)
//{
//	IoDeleteDevice(disk->irpDispatcher.deviceObject_);
//	DbgPrintEx(0, 0, "deleteDevice() - device successfully deleted!\n");
//	return STATUS_SUCCESS;
//}
//
//NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info)
//{
//	Irp->IoStatus.Status = status;
//	Irp->IoStatus.Information = info;
//	IoCompleteRequest(Irp, IO_NO_INCREMENT);
//	return status;
//}


void xorEncrypt(PUCHAR message, ULONG size, const char key[])
{
	int keyLen = strlen(key);
	int i;

	for (i = 0; i < size; ++i)
	{
		// XOR each character with the corresponding character in the key
		message[i] = message[i] ^ key[i % keyLen];
	}
}

VOID IOCTLHandle(IN PVOID Context)

{
	PDEVICE_OBJECT      device_object;
	PDEVICE_EXTENSION   device_extension;
	PLIST_ENTRY         request;
	PIRP                irp;
	PIO_STACK_LOCATION  io_stack;
	PUCHAR              system_buffer;
	PUCHAR              buffer;

	PAGED_CODE();

	ASSERT(Context != NULL);

	device_object = (PDEVICE_OBJECT)Context;

	device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

	KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

	FileDiskAdjustPrivilege(SE_IMPERSONATE_PRIVILEGE, TRUE);

	for (;;)
	{
		KeWaitForSingleObject(
			&device_extension->request_event,
			Executive,
			KernelMode,
			FALSE,
			NULL
		);

		if (device_extension->terminate_thread)
		{
			PsTerminateSystemThread(STATUS_SUCCESS);
		}

		while ((request = ExInterlockedRemoveHeadList(
			&device_extension->list_head,
			&device_extension->list_lock
		)) != NULL)
		{
			irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

			io_stack = IoGetCurrentIrpStackLocation(irp);
			//DbgBreakPoint();
			switch (io_stack->MajorFunction)
			{
			case IRP_MJ_READ:
			{
				/*
				std::cin >> password 
					if (password != device_extension.password)
					{
						irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
						irp->IoStatus.Information = 0;
						return;
					}*/
				ULONG size = io_stack->Parameters.Read.Length;
				system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				if (system_buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, size, DISK_TAG);
				if (buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				ZwReadFile(
					device_extension->file_handle,
					NULL,
					NULL,
					NULL,
					&irp->IoStatus,
					buffer,
					size,
					&io_stack->Parameters.Read.ByteOffset,
					NULL
				);
				xorEncrypt(buffer, size, "disk");
				RtlCopyMemory(system_buffer, buffer, size);
				ExFreePool(buffer);
				break;
			}
			case IRP_MJ_WRITE:
			{
				ULONG size = io_stack->Parameters.Write.Length;
				if ((io_stack->Parameters.Write.ByteOffset.QuadPart +
					size) >
					device_extension->file_size.QuadPart)
				{
					irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
					irp->IoStatus.Information = 0;
					break;
				}
				system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				if (system_buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, io_stack->Parameters.Write.Length, DISK_TAG);
				if (buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				RtlCopyMemory(buffer, system_buffer, size);
				xorEncrypt(buffer, size, "disk");
				
				ZwWriteFile(
					device_extension->file_handle,
					NULL,
					NULL,
					NULL,
					&irp->IoStatus,
					buffer,
					io_stack->Parameters.Write.Length,
					&io_stack->Parameters.Write.ByteOffset,
					NULL
				);
				ExFreePool(buffer);
				break;
			}
			case IRP_MJ_DEVICE_CONTROL:
				switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
				{
				case IOCTL_FILE_DISK_OPEN_FILE:

					SeImpersonateClient(device_extension->security_client_context, NULL);

					irp->IoStatus.Status = FileDiskOpenFile(device_object, irp);

					PsRevertToSelf();

					break;

				case IOCTL_FILE_DISK_CLOSE_FILE:
					irp->IoStatus.Status = FileDiskCloseFile(device_object, irp);
					break;

				default:
					irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
				}
				break;

			default:
				irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
			}

			IoCompleteRequest(
				irp,
				(CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
					IO_DISK_INCREMENT : IO_NO_INCREMENT)
			);
		}
	}
}

NTSTATUS FileDiskAdjustPrivilege(IN ULONG Privilege, IN BOOLEAN Enable)
{
	NTSTATUS            status;
	HANDLE              token_handle;
	TOKEN_PRIVILEGES    token_privileges;

	PAGED_CODE();

	status = ZwOpenProcessToken(
		NtCurrentProcess(),
		TOKEN_ALL_ACCESS,
		&token_handle
	);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	token_privileges.PrivilegeCount = 1;
	token_privileges.Privileges[0].Luid = RtlConvertUlongToLuid(Privilege);
	token_privileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

	status = ZwAdjustPrivilegesToken(
		token_handle,
		FALSE,
		&token_privileges,
		sizeof(token_privileges),
		NULL,
		NULL
	);

	ZwClose(token_handle);

	return status;
}

NTSTATUS FileDiskReadWrite(IN PDEVICE_OBJECT   DeviceObject, IN PIRP Irp)
{

	PDEVICE_EXTENSION   device_extension;
	PIO_STACK_LOCATION  io_stack;

	device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

	if (!device_extension->media_in_device)
	{
		Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_NO_MEDIA_IN_DEVICE;
	}

	io_stack = IoGetCurrentIrpStackLocation(Irp);

	if (io_stack->Parameters.Read.Length == 0)
	{
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_SUCCESS;
	}

	IoMarkIrpPending(Irp);

	ExInterlockedInsertTailList(
		&device_extension->list_head,
		&Irp->Tail.Overlay.ListEntry,
		&device_extension->list_lock
	);

	KeSetEvent(
		&device_extension->request_event,
		(KPRIORITY)0,
		FALSE
	);

	return STATUS_PENDING;
}

//NTSTATUS dispatchIoctl(PIRP Irp, PDEVICE_OBJECT object)
//{
//	PDEVICE_EXTENSION   device_extension = (PDEVICE_EXTENSION)object->DeviceExtension;
//	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
//	NTSTATUS            status = STATUS_SUCCESS;
//
//
//	ULONG code = ioStack->Parameters.DeviceIoControl.IoControlCode;
//	switch (ioStack->Parameters.DeviceIoControl.IoControlCode)
//	{
//
//	case IOCTL_DISK_CHECK_VERIFY:
//	case IOCTL_CDROM_CHECK_VERIFY:
//	case IOCTL_STORAGE_CHECK_VERIFY:
//	case IOCTL_STORAGE_CHECK_VERIFY2:
//	{
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//
//	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
//	case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
//	{
//		PDISK_GEOMETRY  disk_geometry;
//		ULONGLONG       length;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(DISK_GEOMETRY))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		disk_geometry = (PDISK_GEOMETRY)Irp->AssociatedIrp.SystemBuffer;
//
//		length = device_extension->file_size.QuadPart;
//		disk_geometry->Cylinders.QuadPart = length / SECTOR_SIZE / 32 / 2;
//		disk_geometry->MediaType = FixedMedia;
//		disk_geometry->TracksPerCylinder = 2;
//		disk_geometry->SectorsPerTrack = 32;
//		disk_geometry->BytesPerSector = SECTOR_SIZE;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
//
//		break;
//	}
//
//	case IOCTL_DISK_GET_LENGTH_INFO:
//	{
//		PGET_LENGTH_INFORMATION get_length_information;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(GET_LENGTH_INFORMATION))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		get_length_information = (PGET_LENGTH_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
//
//		get_length_information->Length.QuadPart = device_extension->file_size.QuadPart;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);
//
//		break;
//	}
//
//	case IOCTL_DISK_GET_PARTITION_INFO:
//	{
//		PPARTITION_INFORMATION  partition_information;
//		ULONGLONG               length;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(PARTITION_INFORMATION))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		partition_information = (PPARTITION_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
//
//		length = device_extension->file_size.QuadPart;
//
//		partition_information->StartingOffset.QuadPart = 0;
//		partition_information->PartitionLength.QuadPart = length;
//		partition_information->HiddenSectors = 1;
//		partition_information->PartitionNumber = 0;
//		partition_information->PartitionType = 0;
//		partition_information->BootIndicator = FALSE;
//		partition_information->RecognizedPartition = FALSE;
//		partition_information->RewritePartition = FALSE;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
//
//		break;
//	}
//
//	case IOCTL_DISK_GET_PARTITION_INFO_EX:
//	{
//		PPARTITION_INFORMATION_EX   partition_information_ex;
//		ULONGLONG                   length;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(PARTITION_INFORMATION_EX))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		partition_information_ex = (PPARTITION_INFORMATION_EX)Irp->AssociatedIrp.SystemBuffer;
//
//		length = device_extension->file_size.QuadPart;
//
//		partition_information_ex->PartitionStyle = PARTITION_STYLE_MBR;
//		partition_information_ex->StartingOffset.QuadPart = 0;
//		partition_information_ex->PartitionLength.QuadPart = length;
//		partition_information_ex->PartitionNumber = 0;
//		partition_information_ex->RewritePartition = FALSE;
//		partition_information_ex->Mbr.PartitionType = 0;
//		partition_information_ex->Mbr.BootIndicator = FALSE;
//		partition_information_ex->Mbr.RecognizedPartition = FALSE;
//		partition_information_ex->Mbr.HiddenSectors = 1;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);
//
//		break;
//	}
//
//	case IOCTL_DISK_IS_WRITABLE:
//	{
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//
//	case IOCTL_DISK_MEDIA_REMOVAL:
//	case IOCTL_STORAGE_MEDIA_REMOVAL:
//	{
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//
//	case IOCTL_CDROM_READ_TOC:
//	{
//		PCDROM_TOC cdrom_toc;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(CDROM_TOC))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		cdrom_toc = (PCDROM_TOC)Irp->AssociatedIrp.SystemBuffer;
//
//		RtlZeroMemory(cdrom_toc, sizeof(CDROM_TOC));
//
//		cdrom_toc->FirstTrack = 1;
//		cdrom_toc->LastTrack = 1;
//		cdrom_toc->TrackData[0].Control = TOC_DATA_TRACK;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(CDROM_TOC);
//
//		break;
//	}
//
//	case IOCTL_CDROM_GET_LAST_SESSION:
//	{
//		PCDROM_TOC_SESSION_DATA cdrom_toc_s_d;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(CDROM_TOC_SESSION_DATA))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		cdrom_toc_s_d = (PCDROM_TOC_SESSION_DATA)Irp->AssociatedIrp.SystemBuffer;
//
//		RtlZeroMemory(cdrom_toc_s_d, sizeof(CDROM_TOC_SESSION_DATA));
//
//		cdrom_toc_s_d->FirstCompleteSession = 1;
//		cdrom_toc_s_d->LastCompleteSession = 1;
//		cdrom_toc_s_d->TrackData[0].Control = TOC_DATA_TRACK;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(CDROM_TOC_SESSION_DATA);
//
//		break;
//	}
//
//	case IOCTL_DISK_SET_PARTITION_INFO:
//	{
//		if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
//			sizeof(SET_PARTITION_INFORMATION))
//		{
//			status = STATUS_INVALID_PARAMETER;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = 0;
//
//		break;
//	}
//
//	case IOCTL_DISK_VERIFY:
//	{
//		PVERIFY_INFORMATION verify_information;
//
//		if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
//			sizeof(VERIFY_INFORMATION))
//		{
//			status = STATUS_INVALID_PARAMETER;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		verify_information = (PVERIFY_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = verify_information->Length;
//
//		break;
//	}
//
//	case IOCTL_STORAGE_GET_DEVICE_NUMBER:
//	{
//		PSTORAGE_DEVICE_NUMBER number;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(STORAGE_DEVICE_NUMBER))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		number = (PSTORAGE_DEVICE_NUMBER)Irp->AssociatedIrp.SystemBuffer;
//
//		number->DeviceType = FILE_DEVICE_DISK;
//		number->DeviceNumber = device_extension->device_ID;
//		number->PartitionNumber = (ULONG)-1;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(STORAGE_DEVICE_NUMBER);
//
//		break;
//	}
//
//	case IOCTL_STORAGE_GET_HOTPLUG_INFO:
//	{
//		PSTORAGE_HOTPLUG_INFO info;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(STORAGE_HOTPLUG_INFO))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		info = (PSTORAGE_HOTPLUG_INFO)Irp->AssociatedIrp.SystemBuffer;
//
//		info->Size = sizeof(STORAGE_HOTPLUG_INFO);
//		info->MediaRemovable = 0;
//		info->MediaHotplug = 0;
//		info->DeviceHotplug = 0;
//		info->WriteCacheEnableOverride = 0;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);
//
//		break;
//	}
//
//	case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
//	{
//		PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION attr;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		attr = (PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION)Irp->AssociatedIrp.SystemBuffer;
//
//		attr->GptAttributes = 0;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);
//
//		break;
//	}
//
//	case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
//	{
//		PVOLUME_DISK_EXTENTS ext;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(VOLUME_DISK_EXTENTS))
//		{
//			status = STATUS_INVALID_PARAMETER;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//		/*
//					// not needed since there is only one disk extent to return
//					if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//						sizeof(VOLUME_DISK_EXTENTS) + ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT)))
//					{
//						status = STATUS_BUFFER_OVERFLOW;
//						Irp->IoStatus.Information = 0;
//						break;
//					}
//		*/
//		ext = (PVOLUME_DISK_EXTENTS)Irp->AssociatedIrp.SystemBuffer;
//
//		ext->NumberOfDiskExtents = 1;
//		ext->Extents[0].DiskNumber = device_extension->device_ID;
//		ext->Extents[0].StartingOffset.QuadPart = 0;
//		ext->Extents[0].ExtentLength.QuadPart = device_extension->file_size.QuadPart;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS) /*+ ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT))*/;
//
//		break;
//	}
//
//#if (NTDDI_VERSION < NTDDI_VISTA)
//#define IOCTL_DISK_IS_CLUSTERED CTL_CODE(IOCTL_DISK_BASE, 0x003e, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#endif  // NTDDI_VERSION < NTDDI_VISTA
//
//	case IOCTL_DISK_IS_CLUSTERED:
//	{
//		PBOOLEAN clus;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(BOOLEAN))
//		{
//			status = STATUS_BUFFER_TOO_SMALL;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		clus = (PBOOLEAN)Irp->AssociatedIrp.SystemBuffer;
//
//		*clus = FALSE;
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = sizeof(BOOLEAN);
//
//		break;
//	}
//
//	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
//	{
//		PMOUNTDEV_NAME name;
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			sizeof(MOUNTDEV_NAME))
//		{
//			status = STATUS_INVALID_PARAMETER;
//			Irp->IoStatus.Information = 0;
//			break;
//		}
//
//		name = (PMOUNTDEV_NAME)Irp->AssociatedIrp.SystemBuffer;
//		name->NameLength = device_extension->device_name.Length * sizeof(WCHAR);
//
//		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
//			name->NameLength + sizeof(USHORT))
//		{
//			status = STATUS_BUFFER_OVERFLOW;
//			Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
//			break;
//		}
//
//		RtlCopyMemory(name->Name, device_extension->device_name.Buffer, name->NameLength);
//
//		status = STATUS_SUCCESS;
//		Irp->IoStatus.Information = name->NameLength + sizeof(USHORT);
//
//		break;
//	}
//
//	case IOCTL_CDROM_READ_TOC_EX:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl IOCTL_CDROM_READ_TOC_EX.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//	case IOCTL_DISK_GET_MEDIA_TYPES:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl IOCTL_DISK_GET_MEDIA_TYPES.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//	case 0x66001b:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl FT_BALANCED_READ_MODE.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//	case IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//	case IOCTL_STORAGE_QUERY_PROPERTY:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl IOCTL_STORAGE_QUERY_PROPERTY.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//
//#if (NTDDI_VERSION < NTDDI_VISTA)
//#define IOCTL_VOLUME_QUERY_ALLOCATION_HINT CTL_CODE(IOCTL_VOLUME_BASE, 20, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
//#endif  // NTDDI_VERSION < NTDDI_VISTA
//
//	case IOCTL_VOLUME_QUERY_ALLOCATION_HINT:
//	{
//		KdPrint(("FileDisk: Unhandled ioctl IOCTL_VOLUME_QUERY_ALLOCATION_HINT.\n"));
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		break;
//	}
//	default:
//	{
//		DbgPrintEx(0, 0, (
//			"FileDisk: Unknown IoControlCode %#x\n",
//			ioStack->Parameters.DeviceIoControl.IoControlCode
//			));
//
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//	}
//	if (status != STATUS_PENDING)
//	{
//		Irp->IoStatus.Status = status;
//
//		IoCompleteRequest(Irp, IO_NO_INCREMENT);
//	}
//
//}
//	return status;
//
//}

//PVOID getIrpBuffer(PIRP irp)
//{
//	PVOID systemBuffer = 0;
//	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
//	if (ioStack->MajorFunction == IRP_MJ_READ || ioStack->MajorFunction == IRP_MJ_WRITE)
//		systemBuffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
//	else
//		systemBuffer = irp->AssociatedIrp.SystemBuffer;
//	return systemBuffer;
//}

//void xorEncrypt(char message[], const char key[])
//{
//	int msgLen = strlen(message);
//	int keyLen = strlen(key);
//	int i;

//	for (i = 0; i < msgLen; ++i)
//	{
//		// XOR each character with the corresponding character in the key
//		message[i] = message[i] ^ key[i % keyLen];
//	}
//}

// To decrypt the message, simply call the xorEncrypt function again
// with the same key: xorEncrypt(message, key);

//char message[100];
//const char key[] = "my_secret_key"; // Replace with your custom key
//printf("Enter the message to encrypt: ");
//fgets(message, sizeof(message), stdin);

//xorEncrypt(message, key);

//printf("Encrypted message: %s\n", message);
