#pragma once
#include "DriverControl.h"
#include <subauth.h>
#define CORE_MNT_DISPATCHER        0x8001

#define CORE_MNT_MOUNT_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CORE_MNT_EXCHANGE_IOCTL CTL_CODE(CORE_MNT_DISPATCHER, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CORE_MNT_UNMOUNT_IOCTL  CTL_CODE(CORE_MNT_DISPATCHER, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum _DiskOperationType
{
    directOperationSuccess = 0,
    directOperationEmpty = 0,
    directOperationRead,
    directOperationWrite,
    directOperationFail,
    directOperationMax = directOperationFail
}DiskOperationType;

struct CORE_MNT_MOUNT_REQUEST
{
    ULONG64 totalLength;
    const wchar_t* FileName;
    WCHAR   mountPojnt;
};

struct CORE_MNT_MOUNT_RESPONSE
{
    ULONG32 deviceId;
};

struct CORE_MNT_EXCHANGE_REQUEST
{
    ULONG32 deviceId;
    ULONG32 lastType;
    ULONG32 lastStatus;
    ULONG32 lastSize;
    char* data;
    ULONG32 dataSize;
};

struct CORE_MNT_EXCHANGE_RESPONSE
{
    ULONG32 type;
    ULONG32 size;
    ULONG64 offset;
};

struct CORE_MNT_UNMOUNT_REQUEST
{
    ULONG32 deviceId;
};

class DriverControl 
{
	utils::DriverControl m_coreControl;
public:
    DriverControl();
    int Mount(__int64 totalSize, wchar_t mountPoint, const wchar_t* FileName);
    void RequestExchange(int deviceId, int lastType, int lastStatus, int lastSize, char* data, int dataSize,
        int* type, int* size, __int64* offset);
    void Unmount(UINT32 devId);
};

