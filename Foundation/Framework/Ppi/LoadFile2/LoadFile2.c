/*++

Copyright (c) 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  LoadFile2.c

Abstract:

  PI 1.0 spec definition.

--*/


#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (LoadFile2)


EFI_GUID gEfiLoadFile2PpiGuid = EFI_PEI_LOAD_FILE_GUID;
EFI_GUID_STRING(&gEfiLoadFile2PpiGuid, "PeiLoadFile2", "PeiLoadFile2 PPI");
