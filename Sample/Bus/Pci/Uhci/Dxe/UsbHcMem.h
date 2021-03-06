/*++

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  EhciMem.h

Abstract:

  This file contains the definination for host controller memory management routines

Revision History
--*/

#ifndef _EFI_EHCI_MEM_H_
#define _EFI_EHCI_MEM_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "pci22.h"

#include EFI_PROTOCOL_DEFINITION (PciIo)

#define USB_HC_BIT(a)                  ((UINTN)(1 << (a)))

#define USB_HC_BIT_IS_SET(Data, Bit)   \
          ((BOOLEAN)(((Data) & USB_HC_BIT(Bit)) == USB_HC_BIT(Bit)))

#define USB_HC_HIGH_32BIT(Addr64)    \
          ((UINT32)(RShiftU64((UINTN)(Addr64), 32) & 0XFFFFFFFF))

EFI_FORWARD_DECLARATION (USBHC_MEM_BLOCK);

typedef struct _USBHC_MEM_BLOCK {
  UINT8                   *Bits;    // Bit array to record which unit is allocated
  UINTN                   BitsLen; 
  UINT8                   *Buf;
  UINT8                   *BufHost;
  UINTN                   BufLen;   // Memory size in bytes
  VOID                    *Mapping;     
  USBHC_MEM_BLOCK         *Next;
} USBHC_MEM_BLOCK;

//
// USBHC_MEM_POOL is used to manage the memory used by USB 
// host controller. EHCI requires the control memory and transfer
// data to be on the same 4G memory. 
//
typedef struct _USBHC_MEM_POOL {
  EFI_PCI_IO_PROTOCOL     *PciIo;       
  BOOLEAN                 Check4G;      
  UINT32                  Which4G;      
  USBHC_MEM_BLOCK         *Head;
} USBHC_MEM_POOL;

enum {
  USBHC_MEM_UNIT           = 64,     // Memory allocation unit, must be 2^n, n>4
  
  USBHC_MEM_UNIT_MASK      = USBHC_MEM_UNIT - 1,
  USBHC_MEM_DEFAULT_PAGES  = 16,
};

#define USBHC_MEM_ROUND(Len)  (((Len) + USBHC_MEM_UNIT_MASK) & (~USBHC_MEM_UNIT_MASK))

//
// Advance the byte and bit to the next bit, adjust byte accordingly.
//
#define NEXT_BIT(Byte, Bit)   \
          do {                \
            (Bit)++;          \
            if ((Bit) > 7) {  \
              (Byte)++;       \
              (Bit) = 0;      \
            }                 \
          } while (0)       


            
USBHC_MEM_POOL *
UsbHcInitMemPool (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN BOOLEAN              Check4G,
  IN UINT32               Which4G
  )
/*++

Routine Description:

  Initialize the memory management pool for the host controller

Arguments:

  Pool    - The USB memory pool to initialize
  PciIo   - The PciIo that can be used to access the host controller
  Check4G - Whether the host controller requires allocated memory 
            from one 4G address space.
  Which4G - The 4G memory area each memory allocated should be from

Returns:

  EFI_SUCCESS         : The memory pool is initialized
  EFI_OUT_OF_RESOURCE : Fail to init the memory pool

--*/
;
            

EFI_STATUS
UsbHcFreeMemPool (
  IN USBHC_MEM_POOL       *Pool
  )
/*++

Routine Description:

  Release the memory management pool

Arguments:

  Pool   - The USB memory pool to free

Returns:

  EFI_SUCCESS      : The memory pool is freed
  EFI_DEVICE_ERROR : Failed to free the memory pool

--*/
;


VOID *
UsbHcAllocateMem (
  IN  USBHC_MEM_POOL      *Pool,
  IN  UINTN               Size
  )
/*++

Routine Description:

  Allocate some memory from the host controller's memory pool
  which can be used to communicate with host controller.

Arguments:

  Pool      - The host controller's memory pool
  Size      - Size of the memory to allocate

Returns:

  The allocated memory or NULL
  
--*/
;


VOID
UsbHcFreeMem (
  IN USBHC_MEM_POOL       *Pool,
  IN VOID                 *Mem,
  IN UINTN                Size
  )
/*++

Routine Description:

  Free the allocated memory back to the memory pool

Arguments:

  Pool    - The memory pool of the host controller
  Mem     - The memory to free
  Size    - The size of the memory to free

Returns:

  VOID

--*/
;
#endif
