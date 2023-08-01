#pragma once

#include <windows.h>

struct IImage
{
    IImage() :devId_(0), mountPoint_(0) {}
    virtual ~IImage() {}
    virtual void Read(char* buf, UINT64 offset, UINT32 bytesCount) = 0;
    virtual void Write(const char* buf, UINT64 offset, UINT32 bytesCount) = 0;
    virtual UINT64 Size() = 0;

    int GetId() { return devId_; }
    void SetId(int devId) { devId_ = devId; }
    wchar_t GetMountPoint() { return mountPoint_; }
    void SetMountPoint(int mountPoint) { mountPoint_ = mountPoint; }
private:
    int devId_;
    wchar_t mountPoint_;
};