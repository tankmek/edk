/*++
Copyright (c) 2004 - 2009, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:

    UsbBus.h

  Abstract:

    Usb Bus Driver Binding and Bus IO Protocol

  Revision History

--*/

#ifndef _EFI_USB_BUS_H_
#define _EFI_USB_BUS_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "Usb.h"

#include EFI_PROTOCOL_DEFINITION (DriverBinding)
#include EFI_PROTOCOL_DEFINITION (DevicePath)
#include EFI_PROTOCOL_DEFINITION (UsbHostController)
#include EFI_PROTOCOL_DEFINITION (ComponentName)
#include EFI_PROTOCOL_DEFINITION (ComponentName2)

EFI_FORWARD_DECLARATION (USB_DEVICE);
EFI_FORWARD_DECLARATION (USB_INTERFACE);
EFI_FORWARD_DECLARATION (USB_BUS);
EFI_FORWARD_DECLARATION (USB_HUB_API);

#include "UsbUtility.h"
#include "UsbDesc.h"
#include "UsbHub.h"
#include "UsbEnumer.h"

enum {
  USB_MAX_LANG_ID           = 16,
  USB_MAX_INTERFACE         = 16,
  USB_MAX_DEVICES           = 128,

  USB_BUS_STALL_1_MSEC      = 1000,

  //
  // Roothub and hub's polling interval, set by experience,
  // The unit of roothub is 100us, means 1s as interval, and
  // the unit of hub is 1ms, means 64ms as interval.
  //
  USB_ROOTHUB_POLL_INTERVAL = 1000 * 10000U,
  USB_HUB_POLL_INTERVAL     = 64,

  //
  // Wait for port stable to work, refers to specification
  // [USB20-9.1.2]
  //
  USB_WAIT_PORT_STABLE_STALL     = 100 * USB_BUS_STALL_1_MSEC,

  // 
  // Wait for port statue reg change, set by experience
  //
  USB_WAIT_PORT_STS_CHANGE_STALL = 5 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for set device address, refers to specification
  // [USB20-9.2.6.3, it says 2ms]
  //
  USB_SET_DEVICE_ADDRESS_STALL   = 20 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for retry max packet size, set by experience
  //
  USB_RETRY_MAX_PACK_SIZE_STALL  = 100 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for hub port power-on, refers to specification
  // [USB20-11.23.2]
  //
  USB_SET_PORT_POWER_STALL       = 2 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for port reset, refers to specification 
  // [USB20-7.1.7.5, it says 10ms for hub and 50ms for 
  // root hub]
  //
  USB_SET_PORT_RESET_STALL       = 20 * USB_BUS_STALL_1_MSEC,
  USB_SET_ROOT_PORT_RESET_STALL  = 50 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for clear roothub port reset, set by experience
  //
  USB_CLR_ROOT_PORT_RESET_STALL  = 1 * USB_BUS_STALL_1_MSEC,

  //
  // Wait for set roothub port enable, set by experience
  // 
  USB_SET_ROOT_PORT_ENABLE_STALL = 20 * USB_BUS_STALL_1_MSEC,

  //
  // Send general device request timeout 50ms, refers to 
  // specification[USB20-11.24.1]
  //
  USB_GENERAL_DEVICE_REQUEST_TIMEOUT = 50,

  //
  // Send clear feature request timeout 10ms, set by experience
  //
  USB_CLEAR_FEATURE_REQUEST_TIMEOUT  = 10,
  
  //
  // Bus raises TPL to TPL_NOTIFY to serialize all its operations
  // to protect shared data structures.
  //
  USB_BUS_TPL               = EFI_TPL_NOTIFY,

  USB_INTERFACE_SIGNATURE   = EFI_SIGNATURE_32 ('U', 'S', 'B', 'I'),
  USB_BUS_SIGNATURE         = EFI_SIGNATURE_32 ('U', 'S', 'B', 'B'),
};

#define USB_BIT(a)                  ((UINTN)(1 << (a)))
#define USB_BIT_IS_SET(Data, Bit)   ((BOOLEAN)(((Data) & (Bit)) == (Bit)))

#define EFI_USB_BUS_PROTOCOL_GUID \
          {0x2B2F68CC, 0x0CD2, 0x44cf, 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75}

#define USB_INTERFACE_FROM_USBIO(a) \
          CR(a, USB_INTERFACE, UsbIo, USB_INTERFACE_SIGNATURE)

#define USB_BUS_FROM_THIS(a) \
          CR(a, USB_BUS, BusId, USB_BUS_SIGNATURE)

//
// Used to locate USB_BUS
//
typedef struct _EFI_USB_BUS_PROTOCOL {
  UINT64                    Reserved;
} EFI_USB_BUS_PROTOCOL;


//
// Stands for the real USB device. Each device may
// has several seperately working interfaces.
//
typedef struct _USB_DEVICE { 
  USB_BUS                   *Bus;

  //
  // Configuration information
  //
  UINT8                     Speed; 
  UINT8                     Address;
  UINT8                     MaxPacket0;

  //
  // The device's descriptors and its configuration
  //
  USB_DEVICE_DESC           *DevDesc;
  USB_CONFIG_DESC           *ActiveConfig;

  UINT16                    LangId [USB_MAX_LANG_ID];
  UINT16                    TotalLangId;
  
  UINT8                     NumOfInterface;
  USB_INTERFACE             *Interfaces [USB_MAX_INTERFACE];
  
  //
  // Parent child relationship
  //
  EFI_USB2_HC_TRANSACTION_TRANSLATOR Translator;
  
  UINT8                     ParentAddr;
  USB_INTERFACE             *ParentIf;
  UINT8                     ParentPort;       // Start at 0
} USB_DEVICE;

//
// Stands for different functions of USB device
//
typedef struct _USB_INTERFACE {
  UINTN                     Signature;
  USB_DEVICE                *Device;
  USB_INTERFACE_DESC        *IfDesc;
  USB_INTERFACE_SETTING     *IfSetting;
  
  //
  // Handles and protocols
  //
  EFI_HANDLE                Handle;
  EFI_USB_IO_PROTOCOL       UsbIo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsManaged;

  //
  // Hub device special data
  //
  BOOLEAN                   IsHub;
  USB_HUB_API               *HubApi;
  UINT8                     NumOfPort;  
  EFI_EVENT                 HubNotify;  

  //
  // Data used only by normal hub devices
  //
  USB_ENDPOINT_DESC         *HubEp;
  UINT8                     *ChangeMap;

  //
  // Data used only by root hub to hand over device to
  // companion UHCI driver if low/full speed devices are
  // connected to EHCI.
  //
  UINT8                     MaxSpeed;
} USB_INTERFACE;

//
// Stands for the current USB Bus
// 
typedef struct _USB_BUS {
  UINTN                     Signature;
  EFI_USB_BUS_PROTOCOL      BusId;

  //
  // Managed USB host controller 
  //
  EFI_HANDLE                HostHandle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_USB2_HC_PROTOCOL      *Usb2Hc;
  EFI_USB_HC_PROTOCOL       *UsbHc;
  
  //
  // An array of device that is on the bus. Devices[0] is 
  // for root hub. Device with address i is at Devices[i].
  //
  USB_DEVICE                *Devices[USB_MAX_DEVICES];
  
  //
  // USB Bus driver need to control the recursive connect policy of the bus, only those wanted
  // usb child device will be recursively connected.
  // 
  // WantedUsbIoDPList tracks the Usb child devices which user want to recursivly fully connecte,
  // every wanted child device is stored in a item of the WantedUsbIoDPList, whose structrure is 
  // DEVICE_PATH_LIST_ITEM
  //
  EFI_LIST_ENTRY            WantedUsbIoDPList;
  
} USB_BUS;


#define USB_US_LAND_ID   0x0409

#define DEVICE_PATH_LIST_ITEM_SIGNATURE     EFI_SIGNATURE_32('d','p','l','i')
typedef struct _DEVICE_PATH_LIST_ITEM{
  UINTN                                 Signature;
  EFI_LIST_ENTRY                        Link;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
} DEVICE_PATH_LIST_ITEM;

typedef struct {
  USB_CLASS_DEVICE_PATH           UsbClass;
  EFI_DEVICE_PATH_PROTOCOL        End;
} USB_CLASS_FORMAT_DEVICE_PATH;  
  
EFI_STATUS
EFIAPI
UsbBusFreeUsbDPList (
  IN     EFI_LIST_ENTRY                                 *UsbIoDPList
  );

EFI_STATUS
EFIAPI
UsbBusAddWantedUsbIoDP (
  IN EFI_USB_BUS_PROTOCOL         *UsbBusId,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

BOOLEAN
EFIAPI
UsbBusIsWantedUsbIO (
  IN USB_BUS                 *Bus,
  IN USB_INTERFACE           *UsbIf
  );
  
EFI_STATUS
EFIAPI
UsbBusRecursivelyConnectWantedUsbIo (
  IN EFI_USB_BUS_PROTOCOL         *UsbBusId
  );

extern EFI_USB_IO_PROTOCOL           mUsbIoProtocol;
extern EFI_DRIVER_BINDING_PROTOCOL   mUsbBusDriverBinding;
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
extern EFI_COMPONENT_NAME2_PROTOCOL  mUsbBusComponentName;
#else
extern EFI_COMPONENT_NAME_PROTOCOL   mUsbBusComponentName;
#endif

#endif
