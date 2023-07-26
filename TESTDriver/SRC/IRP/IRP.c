#include "main/pch.h"
#include "IRP/IRP.h"


#include <ntstrsafe.h>
// disk as parametr
// system set a letter
// notify system

NTSTATUS IrpHandlerInit(UINT32 devId, UINT64 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk)
{
	IrpData.devId_.deviceId = devId;
	IrpData.totalLength_ = totalLength;
	PDEVICE_OBJECT deviceObject = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	//UNICODE_STRING deviceName;
	//WCHAR device_name_buffer[MAXIMUM_FILENAME_LENGTH];

	//form device name
	UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\disk0");

		//RtlStringCbPrintfW(device_name_buffer,
		//	sizeof(device_name_buffer),
		//	DIRECT_DISK_PREFIX L"%u",
		//	IrpData.devId_.deviceId);
		//RtlInitUnicodeString(&deviceName, device_name_buffer);

	ULONG size = sizeof(DeviceId);
	//create device
	status = IoCreateDevice(DriverObject,
		size,
		&gDeviceName,
		FILE_DEVICE_DISK,
		0,
		FALSE,
		&deviceObject);

	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "FUNCTION can't create device.");
		return STATUS_UNSUCCESSFUL;
	}

	IrpData.deviceObject_ = deviceObject;

	DeviceId* devExt = (DeviceId*)deviceObject->DeviceExtension;
	memset(devExt, 0, sizeof(DeviceId));

	devExt->deviceId = devId;

	deviceObject->Flags |= DO_DIRECT_IO;
	deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	Mdisk->FileName = gDeviceName;
	Mdisk->irpDispatcher = IrpData;
	return status;
}

void IrpHandlergetIrpParam(PIRP irp, IrpParam* irpParam)
{
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	irpParam->offset = 0;
	irpParam->type = directOperationEmpty;
	irpParam->buffer = (char*)getIrpBuffer(irp);
	if (ioStack->MajorFunction == IRP_MJ_READ)
	{
		irpParam->type = directOperationRead;
		irpParam->size = ioStack->Parameters.Read.Length;
		irpParam->offset = ioStack->Parameters.Read.ByteOffset.QuadPart;
	}
	else
		if (ioStack->MajorFunction == IRP_MJ_WRITE)
		{
			irpParam->type = directOperationWrite;
			irpParam->size = ioStack->Parameters.Write.Length;
			irpParam->offset = ioStack->Parameters.Write.ByteOffset.QuadPart;
		}
	return;
}

NTSTATUS IrpHandlerdispatch(PIRP irp)
{
	PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	switch (io_stack->MajorFunction)
	{
	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	case IRP_MJ_QUERY_VOLUME_INFORMATION:
		DbgPrintEx(0,0,(__FUNCTION__" IRP_MJ_QUERY_VOLUME_INFORMATION\n"));
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		status = STATUS_UNSUCCESSFUL;
		break;
	case IRP_MJ_DEVICE_CONTROL:
		dispatchIoctl(irp);
		break;
	default:
		DbgPrintEx(0, 0, (__FUNCTION__"Unknown MJ fnc = 0x%x\n", io_stack->MajorFunction));
		status = STATUS_UNSUCCESSFUL;
	}
	return status;
}

NTSTATUS deleteDevice()
{
	IoDeleteDevice(IrpData.deviceObject_);
	DbgPrintEx(0, 0, "deleteDevice() - device successfully deleted!\n");
	return STATUS_SUCCESS;
}


NTSTATUS handle_read_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG bytesToRead = ioStack->Parameters.Read.Length;
	LARGE_INTEGER offset = ioStack->Parameters.Read.ByteOffset;

	NTSTATUS status = read_from_virtual_disk((char*)Irp->AssociatedIrp.SystemBuffer, bytesToRead, offset);
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = NT_SUCCESS(status) ? bytesToRead : 0;
	DbgPrint("Dummy Driver: handle_read_request\n");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handle_write_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
	ULONG bytesToWrite = ioStack->Parameters.Write.Length;
	LARGE_INTEGER offset = ioStack->Parameters.Write.ByteOffset;

	NTSTATUS status = write_request((const char*)Irp->AssociatedIrp.SystemBuffer, bytesToWrite, offset);
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = NT_SUCCESS(status) ? bytesToWrite : 0;
	DbgPrint("Dummy Driver: handle_write_request\n");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handle_ioctl_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS status = STATUS_SUCCESS;

	// createdisk
	// 

	/// <summary>
	/// /
	/// </summary>
	/// <param name="DeviceObject"></param>
	/// <param name="Irp"></param>
	/// <returns></returns>

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	DbgPrint("Dummy Driver: handle_ioctl_request\n");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS handle_cleanup_request(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	// Perform cleanup tasks (if needed) when a file handle to the virtual disk is closed.
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	DbgPrint("Dummy Driver: handle_cleanup_request\n");
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info)
{
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS dispatch_irp(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
	DbgBreakPoint();
	//disk
	switch (ioStack->MajorFunction)
	{

		//case IRP_MJ_READ:
		//	status = handle_read_request(DeviceObject, Irp);
		//	//xorEncrypt
		//	break;
		//case IRP_MJ_WRITE:
		//	status = handle_write_request(DeviceObject, Irp);
		//	//xorEncrypt
		//	break;
		//case IRP_MJ_DEVICE_CONTROL:
		//	status = handle_ioctl_request(DeviceObject, Irp);
		//	break;
		//	//case://autorth
		//		//create disk
		//		//mount 
		//		//unmount


	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
	case IRP_MJ_CLEANUP:
		return CompleteIrp(Irp, STATUS_SUCCESS, 0);
	case IRP_MJ_DEVICE_CONTROL:
	{
		ULONG code = ioStack->Parameters.DeviceIoControl.IoControlCode;
		PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
		ULONG outputBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
		ULONG inputBufferLength = ioStack->Parameters.DeviceIoControl.InputBufferLength;
		switch (code)
		{
		case CORE_MNT_MOUNT_IOCTL:
			status = DispatchMount(buffer, inputBufferLength, outputBufferLength);
			DbgPrintEx(0, 0, "Dummy Driver: CORE_MNT_MOUNT_IOCTL\n");
			break;
		case CORE_MNT_EXCHANGE_IOCTL:
			status = DispatchExchange(buffer, inputBufferLength, outputBufferLength);
			break;
		case CORE_MNT_UNMOUNT_IOCTL:
			status = DispatchUnmount(buffer, inputBufferLength, outputBufferLength);
			break;
		}
		return CompleteIrp(Irp, status, outputBufferLength);
	}

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		break;
	}

	return status;
}



NTSTATUS read_from_virtual_disk(char* buf, ULONG count, LARGE_INTEGER offset)
{
	UNREFERENCED_PARAMETER(buf);
	UNREFERENCED_PARAMETER(count);
	UNREFERENCED_PARAMETER(offset);
	return STATUS_SUCCESS;
}


NTSTATUS write_request(const char* data, ULONG bytesToWrite, LARGE_INTEGER offset)
{
	IO_STATUS_BLOCK ioStatusBlock;
	FILE_POSITION_INFORMATION positionInfo;
	HANDLE fileHandle;
	NTSTATUS status = STATUS_SUCCESS;
	OBJECT_ATTRIBUTES objectAttributes;

	UNICODE_STRING dirName;

	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);

	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ZwOpenFile(&fileHandle,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,
		&objectAttributes,
		&ioStatusBlock,
		0,
		FILE_NON_DIRECTORY_FILE
	);

	if (status != STATUS_SUCCESS)
	{
		DbgBreakPoint();
		DbgPrintEx(0, 0, "ZwOpenFile() FAILED!\n");
		return status;
	}

	// Set the file pointer to the specified offset
	positionInfo.CurrentByteOffset = offset;
	status = ZwSetInformationFile(fileHandle, &ioStatusBlock, &positionInfo, sizeof(positionInfo), FilePositionInformation);

	if (!NT_SUCCESS(status))
	{
		DbgPrintEx(0, 0, "ZwSetInformationFile() FAILED!\n");
		DbgPrint("Failed to set file pointer: 0x%X\n", status);
		return status;
	}

	// Write the data to the file
	status = ZwWriteFile(ROOT_DIR_NAME, NULL, NULL, NULL, &ioStatusBlock, (PVOID)data, bytesToWrite, &offset, NULL);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Failed to write to file: 0x%X\n", status);
		return status;
	}
	//ZwClose();
	return status;
}


NTSTATUS DispatchMount(PVOID buffer,
	ULONG inputBufferLength,
	ULONG outputBufferLength)
{

	if (inputBufferLength >= sizeof(CoreMNTMountRequest) ||
		outputBufferLength >= sizeof(CoreMNTMountResponse))
	{

		CoreMNTMountRequest* request = (CoreMNTMountRequest*)buffer;
		UINT64 totalLength = request->totalLength;
		CoreMNTMountResponse* response = (CoreMNTMountResponse*)buffer;
		//CreateVirtualDisk();
		response->deviceId = Mount(totalLength);
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0, 0, "DispatchMount() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS DispatchExchange(PVOID buffer, ULONG inputBufferLength, ULONG outputBufferLength)
{

	if (inputBufferLength >= sizeof(CoreMNTExchangeRequest) ||
		outputBufferLength >= sizeof(CoreMNTExchangeResponse))
	{
		CoreMNTExchangeRequest* request = (CoreMNTExchangeRequest*)buffer;

		CoreMNTExchangeResponse response = { 0 };
		MountManagerRequestExchange(request->deviceId,
			request->lastType,
			request->lastStatus,
			request->lastSize,
			request->data,
			request->dataSize,
			&response.type,
			&response.size,
			&response.offset);
		memcpy(buffer, &response, sizeof(response));
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0, 0, "DispatchExchange() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
}
NTSTATUS DispatchUnmount(PVOID buffer, ULONG inputBufferLength, ULONG outputBufferLength)
{
	if (inputBufferLength >= sizeof(CoreMNTUnmountRequest))
	{
		CoreMNTUnmountRequest* request = (CoreMNTUnmountRequest*)buffer;
		Unmount(request->deviceId);
		return STATUS_SUCCESS;
	}
	else
	{
		DbgPrintEx(0, 0, "DispatchUnmount() - buffer size mismatch");
		return STATUS_UNSUCCESSFUL;
	}
}


void dispatchIoctl(PIRP irp)
{
	PIO_STACK_LOCATION io_stack = IoGetCurrentIrpStackLocation(irp);
	ULONG code = io_stack->Parameters.DeviceIoControl.IoControlCode;
	switch (code)
	{
	case IOCTL_DISK_GET_DRIVE_LAYOUT:
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(DRIVE_LAYOUT_INFORMATION))
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
		}
		else
		{
			PDRIVE_LAYOUT_INFORMATION outputBuffer = (PDRIVE_LAYOUT_INFORMATION)
				irp->AssociatedIrp.SystemBuffer;

			outputBuffer->PartitionCount = 1;
			outputBuffer->Signature = 0;

			outputBuffer->PartitionEntry->PartitionType = PARTITION_ENTRY_UNUSED;
			outputBuffer->PartitionEntry->BootIndicator = FALSE;
			outputBuffer->PartitionEntry->RecognizedPartition = TRUE;
			outputBuffer->PartitionEntry->RewritePartition = FALSE;
			outputBuffer->PartitionEntry->StartingOffset = RtlConvertUlongToLargeInteger(0);
			outputBuffer->PartitionEntry->PartitionLength.QuadPart = IrpData.totalLength_;
			outputBuffer->PartitionEntry->HiddenSectors = 1L;

			irp->IoStatus.Status = STATUS_SUCCESS;
			irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
		}
		break;
	case IOCTL_DISK_CHECK_VERIFY:
	case IOCTL_CDROM_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY:
	case IOCTL_STORAGE_CHECK_VERIFY2:
	{
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_DISK_GET_DRIVE_GEOMETRY:
	case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
	{
		PDISK_GEOMETRY  disk_geometry;
		ULONGLONG       length;

		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(DISK_GEOMETRY))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Information = 0;
			break;
		}
		disk_geometry = (PDISK_GEOMETRY)irp->AssociatedIrp.SystemBuffer;
		length = IrpData.totalLength_;
		disk_geometry->Cylinders.QuadPart = length / SECTOR_SIZE / 0x20 / 0x80;
		disk_geometry->MediaType = FixedMedia;
		disk_geometry->TracksPerCylinder = 0x80;
		disk_geometry->SectorsPerTrack = 0x20;
		disk_geometry->BytesPerSector = SECTOR_SIZE;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(DISK_GEOMETRY);
		break;
	}
	case IOCTL_DISK_GET_LENGTH_INFO:
	{
		PGET_LENGTH_INFORMATION get_length_information;
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(GET_LENGTH_INFORMATION))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Information = 0;
			break;
		}
		get_length_information = (PGET_LENGTH_INFORMATION)irp->AssociatedIrp.SystemBuffer;
		get_length_information->Length.QuadPart = IrpData.totalLength_;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);
		break;
	}

	case IOCTL_DISK_GET_PARTITION_INFO:
	{
		PPARTITION_INFORMATION  partition_information;
		ULONGLONG               length;
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(PARTITION_INFORMATION))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Information = 0;
			break;
		}
		partition_information = (PPARTITION_INFORMATION)irp->AssociatedIrp.SystemBuffer;
		length = IrpData.totalLength_;
		partition_information->StartingOffset.QuadPart = 0;
		partition_information->PartitionLength.QuadPart = length;
		partition_information->HiddenSectors = 0;
		partition_information->PartitionNumber = 0;
		partition_information->PartitionType = 0;
		partition_information->BootIndicator = FALSE;
		partition_information->RecognizedPartition = TRUE;
		partition_information->RewritePartition = FALSE;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);
		break;
	}

	case IOCTL_DISK_GET_PARTITION_INFO_EX:
	{
		PPARTITION_INFORMATION_EX   partition_information_ex;
		ULONGLONG                   length;
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(PARTITION_INFORMATION_EX))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Information = 0;
			break;
		}
		partition_information_ex = (PPARTITION_INFORMATION_EX)irp->AssociatedIrp.SystemBuffer;
		length = IrpData.totalLength_;
		partition_information_ex->PartitionStyle = PARTITION_STYLE_MBR;
		partition_information_ex->StartingOffset.QuadPart = 0;
		partition_information_ex->PartitionLength.QuadPart = length;
		partition_information_ex->PartitionNumber = 0;
		partition_information_ex->RewritePartition = FALSE;
		partition_information_ex->Mbr.PartitionType = 0;
		partition_information_ex->Mbr.BootIndicator = FALSE;
		partition_information_ex->Mbr.RecognizedPartition = TRUE;
		partition_information_ex->Mbr.HiddenSectors = 0;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);
		break;
	}

	case IOCTL_DISK_IS_WRITABLE:
	{
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_DISK_MEDIA_REMOVAL:
	case IOCTL_STORAGE_MEDIA_REMOVAL:
	{
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_CDROM_READ_TOC:
	{
		PCDROM_TOC cdrom_toc;
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(CDROM_TOC))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			irp->IoStatus.Information = 0;
			break;
		}
		cdrom_toc = (PCDROM_TOC)irp->AssociatedIrp.SystemBuffer;
		RtlZeroMemory(cdrom_toc, sizeof(CDROM_TOC));
		cdrom_toc->FirstTrack = 1;
		cdrom_toc->LastTrack = 1;
		cdrom_toc->TrackData[0].Control = IOCTL_TOC_DATA_TRACK;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(CDROM_TOC);
		break;
	}
	case IOCTL_DISK_SET_PARTITION_INFO:
	{
		if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(SET_PARTITION_INFORMATION))
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
			break;
		}
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_DISK_VERIFY:
	{
		PVERIFY_INFORMATION verify_information;
		if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
			sizeof(VERIFY_INFORMATION))
		{
			irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			irp->IoStatus.Information = 0;
			break;
		}
		verify_information = (PVERIFY_INFORMATION)irp->AssociatedIrp.SystemBuffer;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = verify_information->Length;
		break;
	}
	case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
	{
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_NAME))
		{
			irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
			irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
		}
		else
		{
			PMOUNTDEV_NAME devName = (PMOUNTDEV_NAME)irp->AssociatedIrp.SystemBuffer;

			WCHAR device_name_buffer[MAXIMUM_FILENAME_LENGTH];
			RtlStringCbPrintfW(device_name_buffer,
				sizeof(device_name_buffer),
				DIRECT_DISK_PREFIX L"%u",
				IrpData.devId_.deviceId);

			UNICODE_STRING deviceName;
			RtlInitUnicodeString(&deviceName, device_name_buffer);

			devName->NameLength = deviceName.Length;
			USHORT outLength = sizeof(USHORT) + deviceName.Length;
			if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < outLength)
			{
				irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
				irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
				break;
			}

			RtlCopyMemory(devName->Name, deviceName.Buffer, deviceName.Length);

			irp->IoStatus.Status = STATUS_SUCCESS;
			irp->IoStatus.Information = outLength;
		}
	}
	break;
	case IOCTL_MOUNTDEV_QUERY_UNIQUE_ID:
	{
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_UNIQUE_ID))
		{
			irp->IoStatus.Information = sizeof(MOUNTDEV_UNIQUE_ID);
			irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
		}
		else
		{
#define UNIQUE_ID_PREFIX L"coreMntMountedDrive"
			PMOUNTDEV_UNIQUE_ID mountDevId = (PMOUNTDEV_UNIQUE_ID)irp->AssociatedIrp.SystemBuffer;

			WCHAR unique_id_buffer[MAXIMUM_FILENAME_LENGTH];
			USHORT unique_id_length;

			RtlStringCbPrintfW(unique_id_buffer,
				sizeof(unique_id_buffer),
				DIRECT_DISK_PREFIX L"%u",
				IrpData.devId_.deviceId);

			UNICODE_STRING uniqueId;
			RtlInitUnicodeString(&uniqueId, unique_id_buffer);
			unique_id_length = uniqueId.Length;

			mountDevId->UniqueIdLength = uniqueId.Length;
			USHORT outLength = sizeof(USHORT) + uniqueId.Length;
			if (io_stack->Parameters.DeviceIoControl.OutputBufferLength < outLength)
			{
				irp->IoStatus.Status = STATUS_BUFFER_OVERFLOW;
				irp->IoStatus.Information = sizeof(MOUNTDEV_UNIQUE_ID);
				break;
			}

			RtlCopyMemory(mountDevId->UniqueId, uniqueId.Buffer, uniqueId.Length);

			irp->IoStatus.Status = STATUS_SUCCESS;
			irp->IoStatus.Information = outLength;
		}
	}
	break;
	case IOCTL_MOUNTDEV_QUERY_STABLE_GUID:
	{
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_MOUNTDEV_UNIQUE_ID_CHANGE_NOTIFY:
	{
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		break;
	}
	case IOCTL_STORAGE_GET_HOTPLUG_INFO:
	{
		if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
			sizeof(STORAGE_HOTPLUG_INFO))
		{
			irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PSTORAGE_HOTPLUG_INFO hotplug =
			(PSTORAGE_HOTPLUG_INFO)irp->AssociatedIrp.SystemBuffer;

		RtlZeroMemory(hotplug, sizeof(STORAGE_HOTPLUG_INFO));

		hotplug->Size = sizeof(STORAGE_HOTPLUG_INFO);
		hotplug->MediaRemovable = 1;

		irp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);
		irp->IoStatus.Status = STATUS_SUCCESS;
	}
	break;
	case IOCTL_MOUNTDEV_UNIQUE_ID_CHANGE_NOTIFY_READWRITE:
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		break;
	default:
		KdPrint((__FUNCTION__"Unknown PNP minor function= 0x%x\n", io_stack->MinorFunction));
	}
}

PVOID getIrpBuffer(PIRP irp)
{
	PVOID systemBuffer = 0;
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	if (ioStack->MajorFunction == IRP_MJ_READ || ioStack->MajorFunction == IRP_MJ_WRITE)
		systemBuffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
	else
		systemBuffer = irp->AssociatedIrp.SystemBuffer;
	return systemBuffer;
}

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
