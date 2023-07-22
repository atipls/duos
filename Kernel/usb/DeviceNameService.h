#pragma once

#include <Ati/Types.h>

struct TDeviceInfo {
    TDeviceInfo *pNext;
    char *pName;
    void *pDevice;
    boolean bBlockDevice;
};

struct TDeviceNameService {
    TDeviceNameService();
    ~TDeviceNameService();

    void AddDevice(const char *name, void *device, boolean isBlockDevice);
    void *GetDevice(const char *name, boolean isBlockDevice);

private:
    TDeviceInfo *deviceInfoHead;
};

TDeviceNameService *DeviceNameService();
