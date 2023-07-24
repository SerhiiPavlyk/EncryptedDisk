
#pragma once
#include <cerrno>
#include <Ntifs.h> // Assuming this is a kernel-mode driver



typedef struct virtual_disk {
	size_t size;        // Size of the virtual disk in bytes
	char* data;         // Pointer to the virtual disk's data
	//name
	//password ?

} VIRTUAL_DISK, * PVIRTUAL_DISK;

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ PUNICODE_STRING pRegistryPath
);


// Add SAL annotations for DriverUnload
_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
VOID DriverUnload( _In_
	 PDRIVER_OBJECT DriverObject
);

int init_module(void);



void* getMy_disk();
