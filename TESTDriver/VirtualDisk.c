#include "pch.h"
#include "VirtualDisk.h"

NTSTATUS CreateVirtualDisk(VIRTUALDISK virtualDisk)
{
	LARGE_INTEGER value;
	value.QuadPart = 20LL * 1024 * 1024;


	OBJECT_ATTRIBUTES objectAttributes;
	NTSTATUS status = STATUS_SUCCESS;
	HANDLE fileHandle;							// Дескриптор файла виртуального диска

			// Атрибуты объекта для открытия файла
	IO_STATUS_BLOCK ioStatusBlock;				// Блок состояния операции ввода-вывода

	UNICODE_STRING dirName;

	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);

	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	DbgBreakPoint();
	status = ZwCreateFile(&fileHandle,								// 
		FILE_GENERIC_READ | FILE_GENERIC_WRITE,					// доступ для чтения и записи
		&objectAttributes,										// 
		&ioStatusBlock,											// 
		&value,													// размер файла - ПРОДУМАТЬ ПОТОМ СОЗДАНИЕ ДИСКА ОПРЕДЕЛЕННОГО РАЗМЕРА
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,			// атрибуты файла (обычный файл и скрытый атрибут)
		0,														// это защита файла - ВОЗМОЖНО СЮДА ВПИХНУТЬ КАКОЕ-ТО ШИФРОВАНИЕ
		FILE_CREATE,											// если файл не существует, создаем
		FILE_NON_DIRECTORY_FILE,								// открыть файлы, не каталоги
		NULL,													// какие-либо параметры создаваемого файла
		0);

	if (status != STATUS_SUCCESS)
	{
		DbgBreakPoint();
		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
		return status;
	}

	return status;
}
