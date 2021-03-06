/*++

Copyright (c) 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

Module Name:

  Md5.h

Abstract:

  Header file for Md5

--*/

#ifndef _MD5_H_
#define _MD5_H_

#define MD5_HASHSIZE  16

typedef struct _MD5_CTX {
  EFI_STATUS  Status;
  UINT64      Length;
  UINT32      States[MD5_HASHSIZE / sizeof (UINT32)];
  UINT8       M[64];
  UINTN       Count;
} MD5_CTX;

EFI_STATUS
MD5Init (
  IN MD5_CTX  *Md5Ctx
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Md5Ctx  - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
;

EFI_STATUS
MD5Update (
  IN MD5_CTX  *Md5Ctx,
  IN VOID     *Data,
  IN UINTN    DataLen
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Md5Ctx  - GC_TODO: add argument description
  Data    - GC_TODO: add argument description
  DataLen - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
;

EFI_STATUS
MD5Final (
  IN  MD5_CTX  *Md5Ctx,
  OUT UINT8    *HashVal
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Md5Ctx  - GC_TODO: add argument description
  HashVal - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
;

#endif // _MD5_H
