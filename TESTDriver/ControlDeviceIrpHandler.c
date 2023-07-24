#include "ControlDeviceIrpHandler.h"


NTSTATUS Mount_Disk(PVOID buff, ULONG input_buff_length, ULONG output_buff_length)
{
		//struct Mount_Request* request;
	UNREFERENCED_PARAMETER(buff);				//������� ����� ��� ������

	if (input_buff_length < sizeof(struct Mount_Request) || output_buff_length < sizeof(struct Mount_Request))
	{
		KdPrint((__FUNCTION__ " buffer size mismatch\n"));
		return STATUS_INVALID_PARAMETER;
	}

		//struct Mount_Request* request = (struct Mount_Request*)buff;

		//UINT64 totalLength = request->length;
		//struct Mount_Response* response = (struct Mount_Response*)buff;

		//response->DeviceID = gMountManager->Mount(totalLength);
	return STATUS_SUCCESS;
}

NTSTATUS Unmount_Disk(PVOID buff, ULONG input_buff_length, ULONG output_buff_length)
{
	UNREFERENCED_PARAMETER(buff);				//������� ����� ��� ������
	UNREFERENCED_PARAMETER(output_buff_length);				//������� ����� ��� ������
	
	if (input_buff_length < sizeof(struct Unmount_Request))
	{
		KdPrint((__FUNCTION__ " buffer size mismatch\n"));
		return STATUS_INVALID_PARAMETER;
	}
		//struct Unmount_Request* request = (struct Unmount_Request*)buff;
		//gMountManager->Unmount(request->deviceId);
	return STATUS_SUCCESS;
}

NTSTATUS Data_Exchange_User_Driver(PVOID buff, ULONG input_buff_length, ULONG output_buff_length)
{
	UNREFERENCED_PARAMETER(buff);				//������� ����� ��� ������

	if (input_buff_length < sizeof(struct Exchange_Request) || output_buff_length < sizeof(struct Exchange_Response))
	{
		KdPrint((__FUNCTION__ " buffer size mismatch\n"));
		return STATUS_UNSUCCESSFUL;
	}
		//struct Exchange_Request* request = (struct Exchange_Request*)buff;

	struct Exchange_Response response = { 0 };
		//gMountManager->RequestExchange(request->DeviceID,
		//	request->lastType,
		//	request->lastStatus,
		//	request->lastSize,
		//	request->data,
		//	request->dataSize,
		//	&response.type,
		//	&response.size,
		//	&response.offset);

	memcpy(buff, &response, sizeof(response));
	return STATUS_SUCCESS;
}

NTSTATUS ControlDeviceIrpHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(pIrp);			//�������� ��������� �� ������� ���� IRP
	NTSTATUS status = STATUS_SUCCESS;

	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
	{
		DbgPrint("IRP_MJ_CREATE successfully called!\n");
		return status;
		//return CompleteIrp(pIrp, STATUS_SUCCESS, 0);
	}
	case IRP_MJ_CLOSE:
	{
		DbgPrint("IRP_MJ_CLOSE successfully called!\n");
		return status;

		//return CompleteIrp(pIrp, STATUS_SUCCESS, 0);
	}
	case IRP_MJ_CLEANUP:
	{
		DbgPrint("IRP_MJ_CLEANUP successfully called!\n");
		return status;

		//return CompleteIrp(pIrp, STATUS_SUCCESS, 0);                 =============���������� CompleteIRP===============
	}
	case IRP_MJ_DEVICE_CONTROL:				//�� ������ ���������� �����������
	{
		DbgPrint("IRP_MJ_DEVICE_CONTROL successfully called!\n");

		ULONG code = irpStack->Parameters.DeviceIoControl.IoControlCode;
		PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;										//��������� �� ����� ������ �� ���� SystemBuffer ������� pIrp 
		ULONG outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
		ULONG inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

		switch (code)
		{
		//��� 3 ����� IOCTL ����� �������� ��� ����������� �� � usermode, ������� ��� IOCTL ������������ � IRP_MJ_DEVICE_CONTROL requests.
		case MOUNT_IOCTL:			
			status = Mount_Disk(buffer, inputBufferLength, outputBufferLength);
			break;
		case EXCHANGE_IOCTL:
			status = Unmount_Disk(buffer, inputBufferLength, outputBufferLength);
			break;
		case UNMOUNT_IOCTL:
			status = Data_Exchange_User_Driver(buffer, inputBufferLength, outputBufferLength);
			break;
		}

		//return CompleteIrp(pIrp, status, outputBufferLength);                //=============���������� CompleteIRP===============
		return status; // ��������� �������, ����� ��������� ������ ��������. ������� � �������� ������� ����
	}
	default:
	{
		DbgPrint("Unknown request!\n");
		
		status = STATUS_INVALID_DEVICE_REQUEST;
		pIrp->IoStatus.Status = status;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		
		return status;
	}
	}
}