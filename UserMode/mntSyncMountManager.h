#pragma once
#include <iostream>
#include <map>
#include <Windows.h>
#include "mntDriverControl.h"
#include "mntImage.h"
#include <mutex>



class SyncMountManager
{
public:
    SyncMountManager() {}
    DriverControl* GetDriverControl()
    {
        return &m_driverControl;
    }
    int AsyncMountImage(std::auto_ptr<IImage> image, wchar_t mountPoint , const wchar_t* FileName);
    void UnmountImage(int deviceId);
private:
    struct DispatchMountContext
    {
        DispatchMountContext(SyncMountManager* mntMan, int dev, IImage* img) :
            mountManager(mntMan), devId(dev), image(img) {}
        
        SyncMountManager* mountManager;
        int devId;
        IImage* image;
    };
    
    typedef std::map<int, IImage*> MountedImageMap;
    typedef MountedImageMap::iterator MountedImageMapIter;
    typedef std::pair<MountedImageMap::iterator, bool> MountedImageMapPairIB;

    std::mutex mountersLock;

    MountedImageMap mounters_;

    DriverControl m_driverControl;

    SyncMountManager(SyncMountManager&);
    
    SyncMountManager& operator=(SyncMountManager&);

    static void mountDispatchThread(void* pContext);

    void OnUnmount(IImage* image, const std::string& reason);
    void Erase(int devId)
    {
        std::lock_guard<std::mutex> guard(mountersLock);
        mounters_.erase(devId);
    }
};