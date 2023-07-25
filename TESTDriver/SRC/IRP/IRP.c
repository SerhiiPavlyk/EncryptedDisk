#include "main/pch.h"
#include "IRP/IRP.h"


// disk as parametr
// system set a letter
// notify system

NTSTATUS handle_read_request(PDEVICE_OBJECT DeviceObject, PIRP Irp)
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

NTSTATUS handle_write_request(PDEVICE_OBJECT DeviceObject, PIRP Irp)
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

NTSTATUS handle_ioctl_request(PDEVICE_OBJECT DeviceObject, PIRP Irp)
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

NTSTATUS handle_cleanup_request(PDEVICE_OBJECT DeviceObject, PIRP Irp)
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
		NTSTATUS status = STATUS_SUCCESS;
		switch (code)
		{
		case CORE_MNT_MOUNT_IOCTL:
			//status = DispatchMount(buffer, inputBufferLength, outputBufferLength);
			DbgPrintEx(0, 0, "Dummy Driver: CORE_MNT_MOUNT_IOCTL\n");
			break;
		case CORE_MNT_EXCHANGE_IOCTL:
			//status = DispatchExchange(buffer, inputBufferLength, outputBufferLength);
			break;
		case CORE_MNT_UNMOUNT_IOCTL:
			//status = DispatchUnmount(buffer, inputBufferLength, outputBufferLength);
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
