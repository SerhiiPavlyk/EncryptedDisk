#pragma once

#define ROOT_DIR_NAME        L"\\??\\H:\\DISK"
#define DIRECT_DISK_PREFIX ROOT_DIR_NAME L"\\disk"

struct IImage;
class DriverControl;

int UndefineLogicDrive(wchar_t mountPoint);

void UnmountImage(int devId, wchar_t mountPoint, DriverControl* drvCtrl);

int MountImage(IImage* image, wchar_t mountPoint, DriverControl* drvCtrl, const wchar_t* FileName);

void DispatchImage(int deviceId, wchar_t mountPoint, IImage* image, DriverControl* drvCtrl);

void DispatchImageOperation(IImage* image, int type, int size, __int64 in_offset, char* buf, int bufsize, int* status);

