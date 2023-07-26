#include "main/pch.h"
#include "VirtualDisk/VirtualDisk.h"

//NTSTATUS CreateVirtualDisk(/*struct Disk*/)
//{
//	VIRTUALDISK disk;
//	OBJECT_ATTRIBUTES objectAttributes;
//
//	/*LARGE_INTEGER value;
//	value.QuadPart = 20LL * 1024 * 1024;*/
//
//	NTSTATUS status = STATUS_SUCCESS;
//	HANDLE fileHandle;              // ���������� ����� ������������ �����
//
//	//OBJECT_ATTRIBUTES objectAttributes;      // �������� ������� ��� �������� �����
//	IO_STATUS_BLOCK ioStatusBlock;        // ���� ��������� �������� �����-������
//
//	UNICODE_STRING dirName;
//
//	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);
//
//	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//	//DbgBreak()Point();
//
//	// �������� ����� ������������ �����
//	status = ZwCreateFile(&fileHandle,              // 
//		FILE_GENERIC_READ | FILE_GENERIC_WRITE,          // ������ ��� ������ � ������
//		&objectAttributes,                    // 
//		&ioStatusBlock,                      // 
//		&disk.size.QuadPart,                          // ������ ����� - ��������� ����� �������� ����� ������������� �������
//		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,      // �������� ����� (������� ���� � ������� �������)
//		0,                            // ��� ������ ����� - �������� ���� �������� �����-�� ����������
//		FILE_CREATE,                      // ���� ���� �� ����������, �������
//		FILE_NON_DIRECTORY_FILE,                // ������� �����, �� �������� (����� � ���� ����� ����������� ��� kernel mode)
//		NULL,                          // �����-���� ��������� ������������ �����
//		0);                            // �����-���� �������� ������������ �����
//
//	if (status != STATUS_SUCCESS)
//	{
//		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
//		return status;
//	}
//
//
//	//// ������ ����������� ���������� � ���� ������������ �����
//	//status = ZwWriteFile(fileHandle,        // ���������� �����
//	//  NULL,                    // ����� �������
//	//  NULL,                    // �����-�� APC-������ 
//	//  NULL,                    // �������� ���� �� APC-������ 
//	//  &ioStatusBlock,                // 
//	//  directoryData.Buffer,            // ��������� �� ����� � ������� ��� ������ 
//	//  directoryData.Length,            // ������ ������ 
//	//  NULL,                    // ������� ��� ������
//	//  NULL);                    // �� ����� ��� ���))
//
//	//if (status!=STATUS_SUCCESS) {
//	//  
//	//  DbgPrintEx(0,0,"Write to disk FAILED!\n");
//	//  ZwClose(fileHandle);                
//
//	//  return status;
//	//}
//
//	ZwClose(fileHandle);
//	return status;
//}
//
//NTSTATUS DispatchIrp(UINT32 devId, PIRP irp)
//{
//	NTSTATUS status = STATUS_SUCCESS;
//
//	{
//		ExAcquireFastMutex(&fastMtx);
//
//		ExReleaseFastMutex(&fastMtx);
//	}
//	return status;
//
//}
//
//
//
//
//
//
//
//NTSTATUS PrintDisk(char Letter)
//{
//	for (int i = 0; i < MAX_SIZE; ++i)
//	{
//		if (Letter == DiskList[i].Letter)
//		{
//			DbgPrintEx(0, 0, "Disk successfully found!\n");
//			DbgPrintEx(0, 0, "Disk LETTER: %c\n", DiskList[i].Letter);
//			DbgPrintEx(0, 0, "Disk FileName: %wZ\n", DiskList[i].FileName);
//			DbgPrintEx(0, 0, "Disk ID: %d\n", DiskList[i].);
//			DbgPrintEx(0, 0, "Disk SIZE: %lld\n", DiskList[i].size.QuadPart);
//
//			return STATUS_SUCCESS;
//		}
//	}
//
//	DbgPrintEx(0, 0, "Disk NOT FOUND!\n");
//	return STATUS_INVALID_PARAMETER;
//}