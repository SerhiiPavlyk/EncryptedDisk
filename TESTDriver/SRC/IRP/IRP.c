#include "main/pch.h"
#include "IRP/IRP.h"

#include <ntstrsafe.h>

NTSTATUS IrpHandlerInit(UINT32 devId, UINT32 totalLength, PDRIVER_OBJECT DriverObject, PMOUNTEDDISK Mdisk)
{
	IrpData.devId_.deviceId = devId;
	IrpData.totalLength_ = 1024 * 1024 * 40LL;
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

NTSTATUS IrpHandlerdispatch(PIRP irp, PMOUNTEDDISK disk)
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
		dispatchIoctl(irp, disk);
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

NTSTATUS dispatchIoctl(PIRP irp, PMOUNTEDDISK disk)
{
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(irp);
	ULONG code = ioStack->Parameters.DeviceIoControl.IoControlCode;
    NTSTATUS status = STATUS_SUCCESS;
    switch (ioStack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_CDROM_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY2:
    {
        status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
    {
        PDISK_GEOMETRY  disk_geometry;
        ULONGLONG       length;
        ULONG           sector_size;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_GEOMETRY))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        disk_geometry = (PDISK_GEOMETRY)irp->AssociatedIrp.SystemBuffer;

        length = disk->fileSize.QuadPart;
        sector_size = 512;
       /* if (device_extension->device_type != FILE_DEVICE_CD_ROM)
        {
            
        }
        else
        {
            sector_size = 2048;
        }*/

        disk_geometry->Cylinders.QuadPart = length / sector_size / 32 / 2;
        disk_geometry->MediaType = FixedMedia;
        disk_geometry->TracksPerCylinder = 2;
        disk_geometry->SectorsPerTrack = 32;
        disk_geometry->BytesPerSector = sector_size;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

        break;
    }

    case IOCTL_DISK_GET_LENGTH_INFO:
    {
        PGET_LENGTH_INFORMATION get_length_information;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GET_LENGTH_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        get_length_information = (PGET_LENGTH_INFORMATION)irp->AssociatedIrp.SystemBuffer;

        get_length_information->Length.QuadPart = disk->fileSize.QuadPart;

        status = STATUS_SUCCESS;
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
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        partition_information = (PPARTITION_INFORMATION)irp->AssociatedIrp.SystemBuffer;

        length = disk->fileSize.QuadPart;

        partition_information->StartingOffset.QuadPart = 0;
        partition_information->PartitionLength.QuadPart = length;
        partition_information->HiddenSectors = 1;
        partition_information->PartitionNumber = 0;
        partition_information->PartitionType = 0;
        partition_information->BootIndicator = FALSE;
        partition_information->RecognizedPartition = FALSE;
        partition_information->RewritePartition = FALSE;

        status = STATUS_SUCCESS;
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
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        partition_information_ex = (PPARTITION_INFORMATION_EX)irp->AssociatedIrp.SystemBuffer;

        length = disk->fileSize.QuadPart;

        partition_information_ex->PartitionStyle = PARTITION_STYLE_MBR;
        partition_information_ex->StartingOffset.QuadPart = 0;
        partition_information_ex->PartitionLength.QuadPart = length;
        partition_information_ex->PartitionNumber = 0;
        partition_information_ex->RewritePartition = FALSE;
        partition_information_ex->Mbr.PartitionType = 0;
        partition_information_ex->Mbr.BootIndicator = FALSE;
        partition_information_ex->Mbr.RecognizedPartition = FALSE;
        partition_information_ex->Mbr.HiddenSectors = 1;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);

        break;
    }

    case IOCTL_DISK_IS_WRITABLE:
    {
        /*if (!device_extension->read_only)
        {
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_MEDIA_WRITE_PROTECTED;
        }*/
        irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
    {
        status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_CDROM_READ_TOC:
    {
        PCDROM_TOC cdrom_toc;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(CDROM_TOC))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        cdrom_toc = (PCDROM_TOC)irp->AssociatedIrp.SystemBuffer;

        RtlZeroMemory(cdrom_toc, sizeof(CDROM_TOC));

        cdrom_toc->FirstTrack = 1;
        cdrom_toc->LastTrack = 1;
        cdrom_toc->TrackData[0].Control = TOC_DATA_TRACK;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(CDROM_TOC);

        break;
    }

    case IOCTL_CDROM_GET_LAST_SESSION:
    {
        PCDROM_TOC_SESSION_DATA cdrom_toc_s_d;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(CDROM_TOC_SESSION_DATA))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        cdrom_toc_s_d = (PCDROM_TOC_SESSION_DATA)irp->AssociatedIrp.SystemBuffer;

        RtlZeroMemory(cdrom_toc_s_d, sizeof(CDROM_TOC_SESSION_DATA));

        cdrom_toc_s_d->FirstCompleteSession = 1;
        cdrom_toc_s_d->LastCompleteSession = 1;
        cdrom_toc_s_d->TrackData[0].Control = TOC_DATA_TRACK;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(CDROM_TOC_SESSION_DATA);

        break;
    }

    case IOCTL_DISK_SET_PARTITION_INFO:
    {
       /* if (device_extension->read_only)
        {
            status = STATUS_MEDIA_WRITE_PROTECTED;
            irp->IoStatus.Information = 0;
            break;
        }*/

        if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(SET_PARTITION_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;

        break;
    }

    case IOCTL_DISK_VERIFY:
    {
        PVERIFY_INFORMATION verify_information;

        if (ioStack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(VERIFY_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        verify_information = (PVERIFY_INFORMATION)irp->AssociatedIrp.SystemBuffer;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = verify_information->Length;

        break;
    }

    case IOCTL_STORAGE_GET_DEVICE_NUMBER:
    {
        PSTORAGE_DEVICE_NUMBER number;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(STORAGE_DEVICE_NUMBER))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        number = (PSTORAGE_DEVICE_NUMBER)irp->AssociatedIrp.SystemBuffer;

        number->DeviceType = FILE_DEVICE_CD_ROM;

        number->DeviceNumber = disk->irpDispatcher.devId_.deviceId;
        number->PartitionNumber = (ULONG)-1;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(STORAGE_DEVICE_NUMBER);

        break;
    }

    case IOCTL_STORAGE_GET_HOTPLUG_INFO:
    {
        PSTORAGE_HOTPLUG_INFO info;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(STORAGE_HOTPLUG_INFO))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        info = (PSTORAGE_HOTPLUG_INFO)irp->AssociatedIrp.SystemBuffer;

        info->Size = sizeof(STORAGE_HOTPLUG_INFO);
        info->MediaRemovable = 0;
        info->MediaHotplug = 0;
        info->DeviceHotplug = 0;
        info->WriteCacheEnableOverride = 0;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);

        break;
    }

    case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
    {
        PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION attr;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        attr = (PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION)irp->AssociatedIrp.SystemBuffer;

        attr->GptAttributes = 0;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);

        break;
    }

    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
    {
        PVOLUME_DISK_EXTENTS ext;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(VOLUME_DISK_EXTENTS))
        {
            status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }
        /*
                    // not needed since there is only one disk extent to return
                    if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
                        sizeof(VOLUME_DISK_EXTENTS) + ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT)))
                    {
                        status = STATUS_BUFFER_OVERFLOW;
                        irp->IoStatus.Information = 0;
                        break;
                    }
        */
        ext = (PVOLUME_DISK_EXTENTS)irp->AssociatedIrp.SystemBuffer;

        ext->NumberOfDiskExtents = 1;
        ext->Extents[0].DiskNumber = disk->irpDispatcher.devId_.deviceId;
        ext->Extents[0].StartingOffset.QuadPart = 0;
        ext->Extents[0].ExtentLength.QuadPart = disk->fileSize.QuadPart;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS) /*+ ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT))*/;

        break;
    }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_DISK_IS_CLUSTERED CTL_CODE(IOCTL_DISK_BASE, 0x003e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_DISK_IS_CLUSTERED:
    {
        PBOOLEAN clus;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(BOOLEAN))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            irp->IoStatus.Information = 0;
            break;
        }

        clus = (PBOOLEAN)irp->AssociatedIrp.SystemBuffer;

        *clus = FALSE;

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = sizeof(BOOLEAN);

        break;
    }

    case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
    {
        PMOUNTDEV_NAME name;

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(MOUNTDEV_NAME))
        {
            status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        name = (PMOUNTDEV_NAME)irp->AssociatedIrp.SystemBuffer;
        name->NameLength = disk->FileName.Length * sizeof(WCHAR);

        if (ioStack->Parameters.DeviceIoControl.OutputBufferLength <
            name->NameLength + sizeof(USHORT))
        {
            status = STATUS_BUFFER_OVERFLOW;
            irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
            break;
        }

        RtlCopyMemory(name->Name, disk->FileName.Buffer, name->NameLength);

        status = STATUS_SUCCESS;
        irp->IoStatus.Information = name->NameLength + sizeof(USHORT);

        break;
    }

    case IOCTL_CDROM_READ_TOC_EX:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_CDROM_READ_TOC_EX.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_DISK_GET_MEDIA_TYPES:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_DISK_GET_MEDIA_TYPES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
    case 0x66001b:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl FT_BALANCED_READ_MODE.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
   /* case IOCTL_SCSI_GET_CAPABILITIES:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_SCSI_GET_CAPABILITIES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_SCSI_PASS_THROUGH:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_SCSI_PASS_THROUGH.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }*/
    case IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_STORAGE_QUERY_PROPERTY:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_STORAGE_QUERY_PROPERTY.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_VOLUME_QUERY_ALLOCATION_HINT CTL_CODE(IOCTL_VOLUME_BASE, 20, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_VOLUME_QUERY_ALLOCATION_HINT:
    {
        DbgPrintEx(0, 0,("FileDisk: Unhandled ioctl IOCTL_VOLUME_QUERY_ALLOCATION_HINT.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
        break;
    }
    default:
    {
        DbgPrintEx(0, 0,(
            "FileDisk: Unknown IoControlCode %#x\n",
            ioStack->Parameters.DeviceIoControl.IoControlCode
            ));

        status = STATUS_INVALID_DEVICE_REQUEST;
        irp->IoStatus.Information = 0;
    }
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
