#include "main/pch.h"
#include "IRP/IRP.h"

#include <ntstrsafe.h>

NTSTATUS IrpHandlerInit(UINT32 devId, UINT32 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk)
{
	IrpData.devId_.deviceId = devId;
	IrpData.totalLength_ = totalLength;
	PDEVICE_OBJECT deviceObject = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING deviceName;
	WCHAR device_name_buffer[MAXIMUM_FILENAME_LENGTH];

	//form device name

		RtlStringCbPrintfW(device_name_buffer,
			sizeof(device_name_buffer),
			L"\\Device\\disk" L"%u",
			IrpData.devId_.deviceId);
		RtlInitUnicodeString(&deviceName, device_name_buffer);

	ULONG size = sizeof(DeviceId);
	//create device
	status = IoCreateDevice(DriverObject,
		size,
		&deviceName,
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
	//DbgBreakPoint();
	DeviceId* devExt = (DeviceId*)deviceObject->DeviceExtension;
	memset(devExt, 0, sizeof(DeviceId));

	devExt->deviceId = devId;
	deviceObject->Flags |= DO_DIRECT_IO;
	deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	Mdisk->FileName.Buffer = (WCHAR*)ExAllocatePool(NonPagedPool, deviceName.Length);
	if (!Mdisk->FileName.Buffer)
	{
		// Handle allocation failure
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	// Copy the contents of deviceName to Mdisk->FileName
	Mdisk->FileName.Length = deviceName.Length;
	Mdisk->FileName.MaximumLength = deviceName.Length;
	RtlCopyUnicodeString(&Mdisk->FileName, &deviceName);

	Mdisk->irpDispatcher = IrpData;
	return status;
}

void IrpHandlerGetIrpParam(PIRP irp, IrpParam* irpParam)
{
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	irpParam->offset = 0;
	irpParam->type = directOperationEmpty;
	irpParam->buffer = (char*)getIrpBuffer(irp);
	if (ioStack->MajorFunction == IRP_MJ_READ)
	{
		irpParam->type = directOperationRead;
		irpParam->size = ioStack->Parameters.Read.Length;
		irpParam->offset = (UINT32)ioStack->Parameters.Read.ByteOffset.QuadPart;
	}
	else
		if (ioStack->MajorFunction == IRP_MJ_WRITE)
		{
			irpParam->type = directOperationWrite;
			irpParam->size = ioStack->Parameters.Write.Length;
			irpParam->offset = (UINT32)ioStack->Parameters.Write.ByteOffset.QuadPart;
		}
	return;
}

NTSTATUS IrpHandlerdispatch(PIRP irp)
{
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	NTSTATUS status = STATUS_SUCCESS;
	//DbgBreakPoint();
	switch (ioStack->MajorFunction)
	{
	case IRP_MJ_PNP:
		DbgPrintEx(0, 0, ("IRP_MJ_PNP\n"));
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	case IRP_MJ_CLEANUP:
	/*	return CompleteIrp(irp, STATUS_SUCCESS, 0);
		break;
	case IRP_MJ_QUERY_INFORMATION:
		DbgPrintEx(0, 0, (__FUNCTION__" IRP_MJ_QUERY_INFORMATION\n"));
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		status = STATUS_UNSUCCESSFUL;
		break;*/


	case IRP_MJ_CREATE:
	case IRP_MJ_CLOSE:
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = 0;
		break;
	case IRP_MJ_QUERY_VOLUME_INFORMATION:
		DbgPrintEx(0,0,("IRP_MJ_QUERY_VOLUME_INFORMATION\n"));
		irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		irp->IoStatus.Information = 0;
		status = STATUS_UNSUCCESSFUL;
		break;
	case IRP_MJ_DEVICE_CONTROL:
		dispatchIoctl(irp);
		//status = irp->IoStatus.Status;
		if (status != STATUS_SUCCESS)
		{
			DbgPrintEx(0, 0, "dispatchIoctl fail  IRP = 100 line");
		}
		
		break;
	default:
		DbgPrintEx(0, 0, "Unknown MJ fnc = 0x%02X\n", ioStack->MajorFunction);

	}
	return status;
}

NTSTATUS deleteDevice(PMOUNTEDDISK disk)
{
	IoDeleteDevice(disk->irpDispatcher.deviceObject_);
	DbgPrintEx(0, 0, "deleteDevice() - device successfully deleted!\n");
	return STATUS_SUCCESS;
}

NTSTATUS CompleteIrp(PIRP Irp, NTSTATUS status, ULONG info)
{
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

void dispatchIoctl(PIRP irp)
{
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	ULONG code = ioStack->Parameters.DeviceIoControl.IoControlCode;
	switch (code)
	{
	case IOCTL_DISK_GET_DRIVE_LAYOUT:
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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

		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		cdrom_toc->TrackData[0].Control = TOC_DATA_TRACK;
		irp->IoStatus.Status = STATUS_SUCCESS;
		irp->IoStatus.Information = sizeof(CDROM_TOC);
		break;
	}
	case IOCTL_DISK_SET_PARTITION_INFO:
	{
		if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_NAME))
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
			if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < outLength)
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MOUNTDEV_UNIQUE_ID))
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
			if (ioStack->Parameters.DeviceIoControl.OutputBufferLength < outLength)
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
		if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
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
		DbgPrintEx(0,0,"Unknown PNP minor function= 0x%x\n", ioStack->MinorFunction);
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
