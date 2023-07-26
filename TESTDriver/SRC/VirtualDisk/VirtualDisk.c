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
//	HANDLE fileHandle;              // Дескриптор файла виртуального диска
//
//	//OBJECT_ATTRIBUTES objectAttributes;      // Атрибуты объекта для открытия файла
//	IO_STATUS_BLOCK ioStatusBlock;        // Блок состояния операции ввода-вывода
//
//	UNICODE_STRING dirName;
//
//	RtlInitUnicodeString(&dirName, ROOT_DIR_NAME);
//
//	InitializeObjectAttributes(&objectAttributes, &dirName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//	//DbgBreak()Point();
//
//	// Создание файла виртуального диска
//	status = ZwCreateFile(&fileHandle,              // 
//		FILE_GENERIC_READ | FILE_GENERIC_WRITE,          // доступ для чтения и записи
//		&objectAttributes,                    // 
//		&ioStatusBlock,                      // 
//		&disk.size.QuadPart,                          // размер файла - ПРОДУМАТЬ ПОТОМ СОЗДАНИЕ ДИСКА ОПРЕДЕЛЕННОГО РАЗМЕРА
//		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,      // атрибуты файла (обычный файл и скрытый атрибут)
//		0,                            // это защита файла - ВОЗМОЖНО СЮДА ВПИХНУТЬ КАКОЕ-ТО ШИФРОВАНИЕ
//		FILE_CREATE,                      // если файл не существует, создаем
//		FILE_NON_DIRECTORY_FILE,                // открыть файлы, не каталоги (флаги в этом месте недопустимы для kernel mode)
//		NULL,                          // какие-либо параметры создаваемого файла
//		0);                            // какие-либо атрибуты создаваемого файла
//
//	if (status != STATUS_SUCCESS)
//	{
//		DbgPrintEx(0, 0, "CreateVirtualDisk() FAILED!\n");
//		return status;
//	}
//
//
//	//// Запись содержимого директории в файл виртуального диска
//	//status = ZwWriteFile(fileHandle,        // дескриптор файла
//	//  NULL,                    // хэндл события
//	//  NULL,                    // какая-то APC-рутина 
//	//  NULL,                    // контекст єтой же APC-рутины 
//	//  &ioStatusBlock,                // 
//	//  directoryData.Buffer,            // указатель на буфер с данными для записи 
//	//  directoryData.Length,            // размер буфера 
//	//  NULL,                    // позиция для записи
//	//  NULL);                    // не понял что єто))
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