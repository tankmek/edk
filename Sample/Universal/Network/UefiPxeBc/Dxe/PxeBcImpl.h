/*++

Copyright (c) 2007 - 2008, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

Module Name:
 
  PxeBcImpl.h
 
Abstract:

--*/

#ifndef __EFI_PXEBC_IMPL_H__
#define __EFI_PXEBC_IMPL_H__

#include "Tiano.h"
#include "EfiPrintLib.h"
#include "NetLib.h"
#include "EfiPxe.h"

#include EFI_PROTOCOL_CONSUMER (Dhcp4)
#include EFI_PROTOCOL_CONSUMER (Mtftp4)
#include EFI_PROTOCOL_CONSUMER (EfiNetworkInterfaceIdentifier)
#include EFI_PROTOCOL_PRODUCER (PxeBaseCode)
#include EFI_PROTOCOL_PRODUCER (PxeBaseCodeCallBack)
#include EFI_PROTOCOL_PRODUCER (LoadFile)
#include EFI_PROTOCOL_DEFINITION (Bis)
#include EFI_GUID_DEFINITION (Smbios)
#include "Smbios.h"

EFI_FORWARD_DECLARATION (PXEBC_PRIVATE_DATA);

#include "PxeBcDriver.h"
#include "PxeArch.h"
#include "PxeBcDhcp.h"
#include "PxeBcMtftp.h"
#include "PxeBcSupport.h"

#define PXEBC_PRIVATE_DATA_SIGNATURE  EFI_SIGNATURE_32 ('P', 'X', 'E', 'P')
#define PXEBC_MTFTP_TIMEOUT           4
#define PXEBC_MTFTP_RETRIES           6

typedef struct _PXEBC_PRIVATE_DATA {
  UINT32                                    Signature;
  EFI_HANDLE                                Controller;
  EFI_HANDLE                                Image;
  EFI_HANDLE                                Dhcp4Child;
  EFI_HANDLE                                Mtftp4Child;
  EFI_HANDLE                                Udp4ReadChild;
  EFI_HANDLE                                Udp4WriteChild;

  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL *Nii;

  EFI_PXE_BASE_CODE_PROTOCOL                PxeBc;
  EFI_LOAD_FILE_PROTOCOL                    LoadFile;
  EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL       LoadFileCallback;
  EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL       *PxeBcCallback;
  EFI_DHCP4_PROTOCOL                        *Dhcp4;
  EFI_MTFTP4_PROTOCOL                       *Mtftp4;
  EFI_UDP4_PROTOCOL                         *Udp4Read;
  EFI_UDP4_PROTOCOL                         *Udp4Write;
  UINT16                                    CurrentUdpSrcPort;
  EFI_UDP4_CONFIG_DATA                      Udp4CfgData;

  EFI_PXE_BASE_CODE_MODE                    Mode;
  EFI_PXE_BASE_CODE_FUNCTION                Function;

  CHAR8                                     *BootFileName;

  EFI_IP_ADDRESS                            StationIp;
  EFI_IP_ADDRESS                            SubnetMask;
  EFI_IP_ADDRESS                            GatewayIp;
  EFI_IP_ADDRESS                            ServerIp;
  BOOLEAN                                   AddressIsOk;

  UINTN                                     FileSize;

  UINT8                                     OptionBuffer[PXEBC_DHCP4_MAX_OPTION_SIZE];
  EFI_DHCP4_PACKET                          SeedPacket;

  BOOLEAN                                   SortOffers;
  BOOLEAN                                   GotProxyOffer;
  UINT32                                    NumOffers;
  UINT32                                    SelectedOffer;
  UINT32                                    ProxyOfferType;

  //
  // Cached packets as complements of pxe mode data
  //
  PXEBC_CACHED_DHCP4_PACKET                 ProxyOffer;
  PXEBC_CACHED_DHCP4_PACKET                 Dhcp4Ack;
  PXEBC_CACHED_DHCP4_PACKET                 PxeReply;
  PXEBC_CACHED_DHCP4_PACKET                 Dhcp4Offers[PXEBC_MAX_OFFER_NUM];
  
  //
  // Arrays for different types of offers:
  //   ServerCount records the count of the servers we got the offers,
  //   OfferIndex records the index of the offer sent by the server indexed by ServerCount.
  //
  UINT32                                    ServerCount[DHCP4_PACKET_TYPE_MAX];
  UINT32                                    OfferIndex[DHCP4_PACKET_TYPE_MAX][PXEBC_MAX_OFFER_NUM];
  UINT32                                    BootpIndex;
  UINT32                                    ProxyIndex[DHCP4_PACKET_TYPE_MAX];
  UINT32                                    BinlIndex[PXEBC_MAX_OFFER_NUM];
} PXEBC_PRIVATE_DATA;

#define PXEBC_PRIVATE_DATA_FROM_PXEBC(a)          CR (a, PXEBC_PRIVATE_DATA, PxeBc, PXEBC_PRIVATE_DATA_SIGNATURE)

#define PXEBC_PRIVATE_DATA_FROM_LOADFILE(a)       CR (a, PXEBC_PRIVATE_DATA, LoadFile, PXEBC_PRIVATE_DATA_SIGNATURE)

#define PXEBC_PRIVATE_DATA_FROM_PXEBCCALLBACK(a)  CR (a, PXEBC_PRIVATE_DATA, PxeBcCallback, PXEBC_PRIVATE_DATA_SIGNATURE)

extern EFI_PXE_BASE_CODE_PROTOCOL mPxeBcProtocolTemplate;
extern EFI_LOAD_FILE_PROTOCOL     mLoadFileProtocolTemplate;

#endif
