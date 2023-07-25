#include <stdio.h>
#include <windows.h>

#define DEVICE_NAME L"\\\\.\\GLOBALROOT\\Device\\DEVICE_TEST_NAME"

#define CORE_MNT_DISPATCHER        0x8001

#define CORE_MNT_MOUNT_IOCTL \
    CTL_CODE(CORE_MNT_DISPATCHER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main()
{   
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    DWORD bytesReturned;
    CHAR writeData[] = "This is a test message from the console app.";

    // Step 1: Open the driver's device
    hDevice = CreateFileW(DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("Error opening device: %d\n", GetLastError());
        return -1;
    }

    // Step 2: Send the IRP_MJ_WRITE request to the driver
    if (!DeviceIoControl(hDevice, CORE_MNT_MOUNT_IOCTL, writeData, sizeof(writeData), NULL, 0, &bytesReturned, NULL))
    {
        printf("Error sending IOCTL: %d\n", GetLastError());
    }
    else
    {
        printf("IRP_MJ_WRITE request sent successfully to the driver.\n");
    }

    // Step 3: Close the device handle
    CloseHandle(hDevice);

    return 0;
}
