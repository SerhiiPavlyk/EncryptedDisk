#include "main/pch.h"
#include "MountManager/MountManager.h"


void MountManagerInit(PDRIVER_OBJECT DriverObject)
{
	DataOfMountManager.DriverObject = DriverObject;
	DataOfMountManager.gMountedDiskCount = 0;
	//ExInitializeFastMutex(&DataOfMountManager.diskMapLock_);
	DataOfMountManager.isInitializied = TRUE;

}

//NTSTATUS MountManagerDispatchIrp(UINT32 devId, PIRP irp)
//{
//	PMOUNTEDDISK disk;
//	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
//
//	for (UINT32 i = 0; i < DataOfMountManager.gMountedDiskCount; i++)
//	{
//		if (devId == MountDiskList[i].irpDispatcher.devId_.deviceId)
//			devId = i;
//
//		/*if (devId == DataOfMountManager.gMountedDiskCount)
//		{
//
//			irp->IoStatus.Status = STATUS_DEVICE_NOT_READY;
//			IoCompleteRequest(irp, IO_NO_INCREMENT);
//			return STATUS_DEVICE_NOT_READY;
//		}*/
//	}
//	disk = MountDiskList + devId;
//	ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//	NTSTATUS status = MountedDiskDispatchIrp(irp, disk);
//
//	return status;
//
//}
//
//int Mount(UINT32 totalLength, const wchar_t* FileName)
//{
//
//	UINT32 devId = 0;
//	{
//		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
//		if (DataOfMountManager.gMountedDiskCount < MAX_SIZE - 2)
//			devId = DataOfMountManager.gMountedDiskCount++;
//		else
//			DbgPrintEx(0, 0, "FUNCTION - device ID already exist\n");
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//	}
//
//	PMOUNTEDDISK disk = (PMOUNTEDDISK)ExAllocatePoolWithTag(NonPagedPool, sizeof(MOUNTEDDISK), 'MYVC');
//	if (disk == NULL)
//	{
//		DbgPrintEx(0, 0, "Failed to mount disk\n");
//		return -1;
//	}
//	size_t fileNameSize = (wcslen(FileName) + 1) * sizeof(wchar_t);
//
//	// Allocate memory for Mdisk->FileName.Buffer
//	disk->FileName.Buffer = ExAllocatePool(PagedPool, fileNameSize);
//	if (disk->FileName.Buffer == NULL)
//	{
//		DbgPrintEx(0, 0, "Failed to allocate memory for Mdisk->FileName.");
//		return STATUS_INSUFFICIENT_RESOURCES;
//	}
//
//	// Copy the contents of FileName into Mdisk->FileName.Buffer
//	disk->FileName.Length = (USHORT)(fileNameSize - sizeof(wchar_t)); // Exclude null terminator
//	disk->FileName.MaximumLength = (USHORT)fileNameSize; // Include null terminator
//	wcscpy_s(disk->FileName.Buffer, disk->FileName.MaximumLength / sizeof(wchar_t), FileName);
//	//RtlInitUnicodeString(&disk->FileName, FileName);
//
//	InitMountDisk(DataOfMountManager.DriverObject, devId, totalLength, disk);
//	MDCreateDisk(disk, disk->pIrp);
//	{
//		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
//		int i = devId;
//
//		if (i <= MAX_SIZE - 1)
//		{
//			MountDiskList[i] = *disk;
//			disk->irpDispatcher.devId_.deviceId = devId;
//			DbgPrintEx(0, 0, "MountedDisk data successfully ADDED in array!\n");
//		}
//		else
//		{
//			DbgPrintEx(0, 0, "VirtDiskList is full!\n");
//			ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//			ExFreePoolWithTag(disk, 'MYVC');
//			return -1;
//		}
//
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//	}
//
//	ExFreePoolWithTag(disk, 'MYVC');
//	return devId;
//}
//
//VOID Unmount(UINT32 deviceId)			//ввиду того, что буква Тома выбирается в любом удобном порядке
//{
//
//	ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
//	if (deviceId < DataOfMountManager.gMountedDiskCount - 1)
//	{
//		DesctructorMountDisk(&MountDiskList[deviceId]);
//		for (UINT32 i = deviceId; i < DataOfMountManager.gMountedDiskCount - 1; ++i)
//			MountDiskList[deviceId] = MountDiskList[deviceId + 1];
//
//		DataOfMountManager.gMountedDiskCount--;
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//		DbgPrintEx(0, 0, "Disk successfully deleted!\n");
//	}
//	else if (deviceId == DataOfMountManager.gMountedDiskCount - 1)
//	{
//		DataOfMountManager.gMountedDiskCount--;				//просто уменьшаем число созданных дисков, чтобы
//									// 1. при показе всех дисков последний не показывался
//									// 2. при создании нового диска, данные перепишутся поверх последнего диска, который "удалили"
//									// P.S. возможно это не лучший вариант :)
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//		DbgPrintEx(0, 0, "Disk successfully deleted!\n");
//	}
//	else
//	{
//		DbgPrintEx(0, 0, "Invalid parameter - disk NOT FOUND");
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//	}
//}
//
//
//VOID MountManagerRequestExchange(UINT32 devID, UINT32 lastType, UINT32 lastStatus, UINT32 lastSize, char* buf,
//	UINT32* type, UINT32* length, UINT32* offset)
//{
//	PMOUNTEDDISK disk = NULL;
//	{
//		ExAcquireFastMutex(&DataOfMountManager.diskMapLock_);
//		for (UINT32 i = 0; i < DataOfMountManager.gMountedDiskCount; ++i)
//		{
//			if (devID == MountDiskList[i].irpDispatcher.devId_.deviceId)
//			{
//				//DbgPrintEx(0, 0, "RequestExchange() - disk FOUND\n");
//				disk = &MountDiskList[i];
//				break;
//			}
//		}
//		ExReleaseFastMutex(&DataOfMountManager.diskMapLock_);
//	}
//	if (disk == NULL)
//		DbgPrintEx(0, 0, "RequestExchange() - Disk NOT FOUND\n");
//
//	MountedDiskRequestExchange(lastType, lastStatus, lastSize, buf,
//		type, length, offset, disk);
//}




NTSTATUS MountManagerCreateDevice()
{
	UNICODE_STRING      device_name;
	NTSTATUS            status;
	PDEVICE_OBJECT      device_object;
	PDEVICE_EXTENSION   device_extension;
	HANDLE              thread_handle;
	UNICODE_STRING      sddl;

	ASSERT(DataOfMountManager.DriverObject != NULL);
    DbgBreakPoint();
	device_name.Buffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, MAXIMUM_FILENAME_LENGTH * 2, DISK_TAG);

	if (device_name.Buffer == NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	device_name.Length = 0;
	device_name.MaximumLength = MAXIMUM_FILENAME_LENGTH * 2;
	DataOfMountManager.gMountedDiskCount++;

	RtlUnicodeStringPrintf(&device_name, DIRECT_DISK_PREFIX L"%u", (DataOfMountManager.gMountedDiskCount - 1));
	RtlInitUnicodeString(&sddl, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;BU)");

	status = IoCreateDeviceSecure(
		DataOfMountManager.DriverObject,
		sizeof(DEVICE_EXTENSION),
		&device_name,
		FILE_DEVICE_DISK,
		0,
		FALSE,
		&sddl,
		NULL,
		&device_object
	);

	if (!NT_SUCCESS(status))
	{
		ExFreePool(device_name.Buffer);
		return status;
	}

	device_object->Flags |= DO_DIRECT_IO;

	device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
	device_extension->device_ID = DataOfMountManager.gMountedDiskCount - 1;
	device_extension->device_name.Length = device_name.Length;
	device_extension->device_name.MaximumLength = device_name.MaximumLength;
	device_extension->device_name.Buffer = device_name.Buffer;
	device_extension->media_in_device = FALSE;
	InitializeListHead(&device_extension->list_head);

	KeInitializeSpinLock(&device_extension->list_lock);

	KeInitializeEvent(
		&device_extension->request_event,
		SynchronizationEvent,
		FALSE
	);

	device_extension->terminate_thread = FALSE;

	status = PsCreateSystemThread(
		&thread_handle,
		(ACCESS_MASK)0L,
		NULL,
		NULL,
		NULL,
		IOCTLHandle,
		device_object
	);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(device_object);
		ExFreePool(device_name.Buffer);
		return status;
	}

	status = ObReferenceObjectByHandle(
		thread_handle,
		THREAD_ALL_ACCESS,
		NULL,
		KernelMode,
		&device_extension->thread_pointer,
		NULL
	);

	if (!NT_SUCCESS(status))
	{
		ZwClose(thread_handle);

		device_extension->terminate_thread = TRUE;

		KeSetEvent(
			&device_extension->request_event,
			(KPRIORITY)0,
			FALSE
		);

		IoDeleteDevice(device_object);

		ExFreePool(device_name.Buffer);

		return status;
	}

	ZwClose(thread_handle);
}

//NTSTATUS MountManagerMount(PIRP Irp, PDEVICE_EXTENSION device_extension)
//{
//	NTSTATUS status = STATUS_SUCCESS;
//	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
//	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
//	ULONG outputBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
//	ULONG inputBufferLength = ioStack->Parameters.DeviceIoControl.InputBufferLength;
//
//	SECURITY_QUALITY_OF_SERVICE security_quality_of_service;
//
//	if (device_extension->media_in_device)
//	{
//		DbgPrintEx(0, 0, "FileDisk: IOCTL_FILE_DISK_OPEN_FILE: Media already opened.\n");
//
//		status = STATUS_INVALID_DEVICE_REQUEST;
//		Irp->IoStatus.Information = 0;
//		return STATUS_NO_MEDIA_IN_DEVICE;
//	}
//
//	if (inputBufferLength < sizeof(DISK_PARAMETERS))
//	{
//		status = STATUS_INVALID_PARAMETER;
//		Irp->IoStatus.Information = 0;
//		return STATUS_UNSUCCESSFUL;
//	}
//
//	if (inputBufferLength < sizeof(DISK_PARAMETERS) +
//		((PDISK_PARAMETERS)Irp->AssociatedIrp.SystemBuffer)->FileNameLength -
//		sizeof(UCHAR))
//	{
//		status = STATUS_INVALID_PARAMETER;
//		Irp->IoStatus.Information = 0;
//		return STATUS_UNSUCCESSFUL;
//	}
//
//	if (device_extension->security_client_context != NULL)
//	{
//		SeDeleteClientSecurity(device_extension->security_client_context);
//	}
//	else
//	{
//		device_extension->security_client_context =
//			ExAllocatePoolWithTag(NonPagedPool, sizeof(SECURITY_CLIENT_CONTEXT), DISK_TAG);
//	}
//
//	RtlZeroMemory(&security_quality_of_service, sizeof(SECURITY_QUALITY_OF_SERVICE));
//
//	security_quality_of_service.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
//	security_quality_of_service.ImpersonationLevel = SecurityImpersonation;
//	security_quality_of_service.ContextTrackingMode = SECURITY_STATIC_TRACKING;
//	security_quality_of_service.EffectiveOnly = FALSE;
//
//	if ((SeCreateClientSecurity(
//		PsGetCurrentThread(),
//		&security_quality_of_service,
//		FALSE,
//		device_extension->security_client_context
//	)) != STATUS_SUCCESS)
//	{
//		DbgPrintEx(0, 0, ("SeCreateClientSecurity. Error\n"));
//		return STATUS_UNSUCCESSFUL;
//	}
//
//	IoMarkIrpPending(Irp);
//
//	ExInterlockedInsertTailList(
//		&device_extension->list_head,
//		&Irp->Tail.Overlay.ListEntry,
//		&device_extension->list_lock
//	);
//
//	KeSetEvent(
//		&device_extension->request_event,
//		(KPRIORITY)0,
//		FALSE
//	);
//
//	status = STATUS_PENDING;
//
//	return status;
//}
//



NTSTATUS FileDiskOpenFile(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION               device_extension;
    PDISK_PARAMETERS          open_file_information;
    NTSTATUS                        status;
    OBJECT_ATTRIBUTES               object_attributes;
    FILE_END_OF_FILE_INFORMATION    file_eof;
    FILE_BASIC_INFORMATION          file_basic;
    FILE_STANDARD_INFORMATION       file_standard;
    FILE_ALIGNMENT_INFORMATION      file_alignment;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    DbgBreakPoint();
    open_file_information = (PDISK_PARAMETERS)Irp->AssociatedIrp.SystemBuffer;

    device_extension->file_name.Length = open_file_information->FileNameLength * sizeof(wchar_t);
    device_extension->file_name.MaximumLength = device_extension->file_name.Length + 1;
    device_extension->file_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, device_extension->file_name.Length, DISK_TAG);

    if (device_extension->file_name.Buffer == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(
        device_extension->file_name.Buffer,
        open_file_information->FileName,
        device_extension->file_name.Length
    );

    InitializeObjectAttributes(
        &object_attributes,
        &device_extension->file_name,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    status = ZwCreateFile(
        &device_extension->file_handle,
        GENERIC_READ | GENERIC_WRITE,
        &object_attributes,
        &Irp->IoStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
         0,
        FILE_OPEN,
        FILE_NON_DIRECTORY_FILE |
        FILE_RANDOM_ACCESS |
        FILE_NO_INTERMEDIATE_BUFFERING |
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
    );

    if (NT_SUCCESS(status))
    {
        KdPrint(("FileDisk: File %.*S opened.\n", device_extension->file_name.Length / 2, device_extension->file_name.Buffer));
    }

    if (status == STATUS_OBJECT_NAME_NOT_FOUND || status == STATUS_NO_SUCH_FILE)
    {
        if ( open_file_information->Size.QuadPart == 0)
        {
            DbgPrint("FileDisk: File %.*S not found.\n", device_extension->file_name.Length / 2, device_extension->file_name.Buffer);
            ExFreePool(device_extension->file_name.Buffer);

            Irp->IoStatus.Status = STATUS_NO_SUCH_FILE;
            Irp->IoStatus.Information = 0;

            return STATUS_NO_SUCH_FILE;
        }
        else
        {
            status = ZwCreateFile(
                &device_extension->file_handle,
                GENERIC_READ | GENERIC_WRITE,
                &object_attributes,
                &Irp->IoStatus,
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

            if (!NT_SUCCESS(status))
            {
                DbgPrint("FileDisk: File %.*S could not be created.\n", device_extension->file_name.Length / 2, device_extension->file_name.Buffer);
                ExFreePool(device_extension->file_name.Buffer);
                return status;
            }

            if (Irp->IoStatus.Information == FILE_CREATED)
            {
                KdPrint(("FileDisk: File %.*S created.\n", device_extension->file_name.Length / 2, device_extension->file_name.Buffer));
                status = ZwFsControlFile(
                    device_extension->file_handle,
                    NULL,
                    NULL,
                    NULL,
                    &Irp->IoStatus,
                    FSCTL_SET_SPARSE,
                    NULL,
                    0,
                    NULL,
                    0
                );

                if (NT_SUCCESS(status))
                {
                    KdPrint(("FileDisk: File attributes set to sparse.\n"));
                }

                file_eof.EndOfFile.QuadPart = open_file_information->Size.QuadPart;

                status = ZwSetInformationFile(
                    device_extension->file_handle,
                    &Irp->IoStatus,
                    &file_eof,
                    sizeof(FILE_END_OF_FILE_INFORMATION),
                    FileEndOfFileInformation
                );

                if (!NT_SUCCESS(status))
                {
                    DbgPrint("FileDisk: eof could not be set.\n");
                    ExFreePool(device_extension->file_name.Buffer);
                    ZwClose(device_extension->file_handle);
                    return status;
                }
                KdPrint(("FileDisk: eof set to %I64u.\n", file_eof.EndOfFile.QuadPart));
            }
        }
    }
    else if (!NT_SUCCESS(status))
    {
        DbgPrint("FileDisk: File %.*S could not be opened.\n", device_extension->file_name.Length / 2, device_extension->file_name.Buffer);
        ExFreePool(device_extension->file_name.Buffer);
        return status;
    }

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_basic,
        sizeof(FILE_BASIC_INFORMATION),
        FileBasicInformation
    );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    //
    // The NT cache manager can deadlock if a filesystem that is using the cache
    // manager is used in a virtual disk that stores its file on a filesystem
    // that is also using the cache manager, this is why we open the file with
    // FILE_NO_INTERMEDIATE_BUFFERING above, however if the file is compressed
    // or encrypted NT will not honor this request and cache it anyway since it
    // need to store the decompressed/unencrypted data somewhere, therefor we put
    // an extra check here and don't alow disk images to be compressed/encrypted.
    //
    if (file_basic.FileAttributes & (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED))
    {
        DbgPrint("FileDisk: Warning: File is compressed or encrypted. File attributes: %#x.\n", file_basic.FileAttributes);
        /*
                ExFreePool(device_extension->file_name.Buffer);
                ZwClose(device_extension->file_handle);
                Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
                Irp->IoStatus.Information = 0;
                return STATUS_ACCESS_DENIED;
        */
    }

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_standard,
        sizeof(FILE_STANDARD_INFORMATION),
        FileStandardInformation
    );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    device_extension->file_size.QuadPart = file_standard.EndOfFile.QuadPart;

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_alignment,
        sizeof(FILE_ALIGNMENT_INFORMATION),
        FileAlignmentInformation
    );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    DeviceObject->AlignmentRequirement = file_alignment.AlignmentRequirement;

        DeviceObject->Characteristics &= ~FILE_READ_ONLY_DEVICE;

    device_extension->media_in_device = TRUE;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS FileDiskCloseFile(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)

{
	PDEVICE_EXTENSION device_extension;

	PAGED_CODE();

	ASSERT(DeviceObject != NULL);
	ASSERT(Irp != NULL);

	device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

	ExFreePool(device_extension->file_name.Buffer);

	ZwClose(device_extension->file_handle);

	device_extension->media_in_device = FALSE;

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	return STATUS_SUCCESS;
}