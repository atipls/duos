#include "DeviceNameService.h"
#include "uspios.h"
#include "util.h"
#include <mem/Memory.h>
#include <support/Runtime.h>

static TDeviceNameService *s_pThis = nullptr;

TDeviceNameService::TDeviceNameService() {
    deviceInfoHead = nullptr;
    s_pThis = this;
}

TDeviceNameService::~TDeviceNameService() {
    while (deviceInfoHead != nullptr) {
        auto nextDevice = deviceInfoHead->pNext;

        Memory::Deallocate(deviceInfoHead->pName);

        deviceInfoHead->pName = nullptr;
        deviceInfoHead->pDevice = nullptr;

        Memory::Deallocate(deviceInfoHead);

        deviceInfoHead = nextDevice;
    }

    s_pThis = nullptr;
}

void TDeviceNameService::AddDevice(const char *name, void *device, boolean isBlockDevice) {
    auto nameLength = Runtime::StringLength(name);

    auto deviceInfo = allocate<TDeviceInfo>();
    deviceInfo->pName = (char *) Memory::Allocate(nameLength + 1);

    Runtime::Copy(deviceInfo->pName, name, nameLength);
    deviceInfo->pName[nameLength] = 0;

    deviceInfo->pDevice = device;
    deviceInfo->bBlockDevice = isBlockDevice;
    deviceInfo->pNext = deviceInfoHead;
    deviceInfoHead = deviceInfo;
}

void *TDeviceNameService::GetDevice(const char *name, boolean isBlockDevice) {
    TDeviceInfo *pInfo = deviceInfoHead;
    while (pInfo != nullptr) {
        if (strcmp(name, pInfo->pName) == 0 && pInfo->bBlockDevice == isBlockDevice)
            return pInfo->pDevice;

        pInfo = pInfo->pNext;
    }

    return nullptr;
}

TDeviceNameService *DeviceNameService() { return s_pThis; }
