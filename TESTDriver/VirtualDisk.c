#include "pch.h"
#include "VirtualDisk.h"

NTSTATUS CreateVirtualDisk(VIRTUALDISK virtualDisk)
{
	LARGE_INTEGER value;
	value.QuadPart = 20LL * 1024 * 1024;


	OBJECT_ATTRIBUTES objectAttributes;
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE fileHandle;							// ���������� ����� ������������ �����

			// �������� ������� ��� �������� �����
	IO_STATUS_BLOCK ioStatusBlock;				// ���� ��������� �������� �����-������

	UNICODE_STRING dirName;

	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);

	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	DbgBreakPoint();
	status = ZwCreateFile(&fileHandle,								// 
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,					// ������ ��� ������ � ������
		&objectAttributes,										// 
		&ioStatusBlock,											// 
		&value,													// ������ ����� - ��������� ����� �������� ����� ������������� �������
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,			// �������� ����� (������� ���� � ������� �������)
		0,														// ��� ������ ����� - �������� ���� �������� �����-�� ����������
		FILE_CREATE,											// ���� ���� �� ����������, �������
		FILE_NON_DIRECTORY_FILE,								// ������� �����, �� ��������
		NULL,													// �����-���� ��������� ������������ �����
		0);

	if (status != STATUS_SUCCESS)
	{
		DbgBreakPoint();
		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
		return status;
	}

	return status;
}
