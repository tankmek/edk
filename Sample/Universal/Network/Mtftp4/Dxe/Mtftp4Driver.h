/*++

Copyright (c) 2006 - 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

Module Name:
 
  Mtftp4Driver.h
 
Abstract:
 
--*/

#ifndef __EFI_MTFTP4_DRIVER_H__
#define __EFI_MTFTP4_DRIVER_H__

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "NetLib.h"
#include "NetBuffer.h"

#include EFI_PROTOCOL_CONSUMER (Udp4)
#include EFI_PROTOCOL_PRODUCER (Mtftp4)

EFI_STATUS
Mtftp4DriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

EFI_STATUS
Mtftp4DriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

EFI_STATUS
Mtftp4DriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL *This,
  IN  EFI_HANDLE                  Controller,
  IN  UINTN                       NumberOfChildren,
  IN  EFI_HANDLE                  *ChildHandleBuffer
  );

EFI_STATUS
Mtftp4ServiceBindingCreateChild (
  IN EFI_SERVICE_BINDING_PROTOCOL *This,
  IN OUT EFI_HANDLE               *ChildHandle
  );

EFI_STATUS
Mtftp4ServiceBindingDestroyChild (
  IN EFI_SERVICE_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                   ChildHandle
  );

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
extern EFI_COMPONENT_NAME2_PROTOCOL gMtftp4ComponentName;
#else
extern EFI_COMPONENT_NAME_PROTOCOL  gMtftp4ComponentName;
#endif
extern EFI_DRIVER_BINDING_PROTOCOL  gMtftp4DriverBinding;
#endif
