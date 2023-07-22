//
// usbdevicefactory.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "usbdevicefactory.h"
#include "uspios.h"
#include <support/Assert.h>
#include <mem/Memory.h>

// for factory
#include "usbstandardhub.h"
#include "usbmassdevice.h"
#include "usbkeyboard.h"
#include "usbmouse.h"
#include "usbgamepad.h"
#include "usbmidi.h"
#include "smsc951x.h"
#include "lan7800.h"

TUSBFunction *USBDeviceFactoryGetDevice (TUSBFunction *pParent, TString *pName)
{
	assert (pParent != 0);
	assert (pName != 0);
	
	TUSBFunction *pResult = 0;

	if (   StringCompare (pName, "int9-0-2") == 0
	    || StringCompare (pName, "int9-0-0") == 0)
	{
		TUSBStandardHub *pDevice = (TUSBStandardHub *) Memory::Allocate (sizeof (TUSBStandardHub));
		assert (pDevice != 0);
		USBStandardHub (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
	else if (StringCompare (pName, "int8-6-50") == 0)
	{
		TUSBBulkOnlyMassStorageDevice *pDevice = (TUSBBulkOnlyMassStorageDevice *) Memory::Allocate (sizeof (TUSBBulkOnlyMassStorageDevice));
		assert (pDevice != 0);
		USBBulkOnlyMassStorageDevice (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
	else if (StringCompare (pName, "int3-1-1") == 0)
	{
		TUSBKeyboardDevice *pDevice = (TUSBKeyboardDevice *) Memory::Allocate (sizeof (TUSBKeyboardDevice));
		assert (pDevice != 0);
		USBKeyboardDevice (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
	else if (StringCompare (pName, "int3-1-2") == 0)
	{
		TUSBMouseDevice *pDevice = (TUSBMouseDevice *) Memory::Allocate (sizeof (TUSBMouseDevice));
		assert (pDevice != 0);
		USBMouseDevice (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
	else if (StringCompare (pName, "ven424-ec00") == 0)
	{
		TSMSC951xDevice *pDevice = (TSMSC951xDevice *) Memory::Allocate (sizeof (TSMSC951xDevice));
		assert (pDevice != 0);
		SMSC951xDevice (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
	else if (StringCompare (pName, "ven424-7800") == 0)
	{
		TLAN7800Device *pDevice = (TLAN7800Device *) Memory::Allocate (sizeof (TLAN7800Device));
		assert (pDevice != 0);
		LAN7800Device (pDevice, pParent);
		pResult = (TUSBFunction *) pDevice;
	}
    else if (StringCompare (pName, "int3-0-0") == 0)
    {
        TUSBGamePadDevice *pDevice = (TUSBGamePadDevice *) Memory::Allocate (sizeof (TUSBGamePadDevice));
        assert (pDevice != 0);
        USBGamePadDevice (pDevice, pParent);
        pResult = (TUSBFunction *) pDevice;
    }
	else if (StringCompare (pName, "int1-3-0") == 0)
	{
		TUSBMIDIDevice *pDevice = (TUSBMIDIDevice *) Memory::Allocate (sizeof (TUSBMIDIDevice));
		assert (pDevice != 0);
		USBMIDIDevice (pDevice, pParent);
		pResult = (TUSBFunction *)pDevice;
	}
	// new devices follow

	if (pResult != 0)
	{
		LogWrite ("usbdev", LOG_NOTICE, "Using device/interface %s", StringGet (pName));
	}
	
	_String (pName);
	Memory::Deallocate(pName);
	
	return pResult;
}
