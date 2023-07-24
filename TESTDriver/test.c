#include "test.h"


PVIRTUAL_DISK my_disk = NULL;

// In module initialization function
int init_module(void)
{
	//my_disk = "\\ dir"
		
		// disk in system
		
		// read from this file 

		// write to this file


	//	= (PVIRTUAL_DISK)ExAllocatePoolWithTag(NonPagedPool, sizeof(PVIRTUAL_DISK), 'VDIS');
	if (!my_disk)
	{
		return -1;
	}

	// Set the disk size as needed (e.g., 1 MB disk)
	my_disk->size = 1024 * 1024;

	// Allocate memory for the virtual disk's data
	my_disk->data = (char*)ExAllocatePoolWithTag(NonPagedPool, my_disk->size, 'VDAT');
	if (!my_disk->data)
	{
		ExFreePoolWithTag(my_disk, 'VDIS');
		return -1;
	}
	return 0;
	// Initialize the virtual disk's data if required
	// RtlZeroMemory(my_disk->data, my_disk->size);

	// Continue with other initialization tasks (e.g., setting up IRP handlers)
}

void* getMy_disk()
{
	return (void*)my_disk;
}

