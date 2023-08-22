#include "main/pch.h"
#include "IRP/IRP.h"
#include <ntstrsafe.h>

void xorEncrypt(PUCHAR message, ULONG size, UNICODE_STRING key)
{
	ULONG keyLen = key.Length;

	for (ULONG i = 0; i < size; ++i)
	{
		// XOR each character with the corresponding character in the key
		message[i] = (UCHAR)(message[i] ^ key.Buffer[i % keyLen]);
	}
}

VOID IOCTLHandle(IN PVOID Context)
{
	PDEVICE_OBJECT      device_object;
	PDEVICE_EXTENSION   device_extension;
	PLIST_ENTRY         request;
	PIRP                irp;
	PIO_STACK_LOCATION  ioStack;
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

			ioStack = IoGetCurrentIrpStackLocation(irp);

			switch (ioStack->MajorFunction)
			{
			case IRP_MJ_READ:
			{
				ULONG size = ioStack->Parameters.Read.Length;
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
					&ioStack->Parameters.Read.ByteOffset,
					NULL
				);

				xorEncrypt(buffer, size, device_extension->password);
				RtlCopyMemory(system_buffer, buffer, size);
				ExFreePool(buffer);
				break;
			}
			case IRP_MJ_WRITE:
			{
				ULONG size = ioStack->Parameters.Write.Length;
				if ((ioStack->Parameters.Write.ByteOffset.QuadPart +
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
				buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, ioStack->Parameters.Write.Length, DISK_TAG);
				if (buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				RtlCopyMemory(buffer, system_buffer, size);
				xorEncrypt(buffer, size, device_extension->password);

				ZwWriteFile(
					device_extension->file_handle,
					NULL,
					NULL,
					NULL,
					&irp->IoStatus,
					buffer,
					ioStack->Parameters.Write.Length,
					&ioStack->Parameters.Write.ByteOffset,
					NULL
				);
				ExFreePool(buffer);
				break;
			}

			case IRP_MJ_DEVICE_CONTROL:
				switch (ioStack->Parameters.DeviceIoControl.IoControlCode)
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

			IoCompleteRequest(irp, (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ? IO_DISK_INCREMENT : IO_NO_INCREMENT));
		}
	}
}

NTSTATUS FileDiskAdjustPrivilege(IN ULONG Privilege, IN BOOLEAN Enable)
{
	NTSTATUS            status;
	HANDLE              token_handle;
	TOKEN_PRIVILEGES    token_privileges;

	PAGED_CODE();

	status = ZwOpenProcessToken(NtCurrentProcess(), TOKEN_ALL_ACCESS, &token_handle);

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

NTSTATUS FileDiskReadWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PDEVICE_EXTENSION   device_extension;
	PIO_STACK_LOCATION  ioStack;

	device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

	if (!device_extension->media_in_device)
	{
		Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_NO_MEDIA_IN_DEVICE;
	}

	ioStack = IoGetCurrentIrpStackLocation(Irp);

	if (ioStack->Parameters.Read.Length == 0)
	{
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return STATUS_SUCCESS;
	}

	IoMarkIrpPending(Irp);

	ExInterlockedInsertTailList(&device_extension->list_head, &Irp->Tail.Overlay.ListEntry, &device_extension->list_lock);

	KeSetEvent(&device_extension->request_event, (KPRIORITY)0, FALSE);

	return STATUS_PENDING;
}
