/*++

Copyright (c) 2005 - 2007, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

Module Name:

  IpIo.c

Abstract:

  The implementation of the IpIo layer.

--*/

#include "IpIo.h"

#define NET_PROTO_HDR(Buf, Type)  ((Type *) ((Buf)->BlockOp[0].Head))
#define ICMP_ERRLEN(IpHdr) \
  (sizeof(IP4_ICMP_HEAD) + EFI_IP4_HEADER_LEN(IpHdr) + 8)

NET_LIST_ENTRY  mActiveIpIoList = {
  &mActiveIpIoList,
  &mActiveIpIoList
};

EFI_IP4_CONFIG_DATA  mIpIoDefaultIpConfigData = {
  EFI_IP_PROTO_UDP,
  FALSE,
  TRUE,
  FALSE,
  FALSE,
  FALSE,
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  0,
  255,
  FALSE,
  FALSE,
  0,
  0
};

STATIC
VOID
EFIAPI
IpIoTransmitHandlerDpc (
  IN VOID      *Context
  );

STATIC
VOID
EFIAPI
IpIoTransmitHandler (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

STATIC
EFI_STATUS
IpIoCreateIpChildOpenProtocol (
  IN  EFI_HANDLE  ControllerHandle,
  IN  EFI_HANDLE  ImageHandle,
  IN  EFI_HANDLE  *ChildHandle,
  OUT VOID        **Interface
  )
/*++

Routine Description:

  This function create an ip child ,open the IP protocol, return the opened
  Ip protocol to Interface.
  
Arguments:

  ControllerHandle - The controller handle.
  ImageHandle      - The image handle.
  ChildHandle      - Pointer to the buffer to save the ip child handle.
  Interface        - Pointer used to get the ip protocol interface.

Returns:

  EFI_SUCCESS - The ip child is created and the ip protocol interface is retrieved.
  other       - The required operation failed.

--*/
{
  EFI_STATUS  Status;

  //
  // Create an ip child.
  //
  Status = NetLibCreateServiceChild (
             ControllerHandle,
             ImageHandle,
             &gEfiIp4ServiceBindingProtocolGuid,
             ChildHandle
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Open the ip protocol installed on the *ChildHandle.
  //
  Status = gBS->OpenProtocol (
                  *ChildHandle,
                  &gEfiIp4ProtocolGuid,
                  Interface,
                  ImageHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    //
    // On failure, destroy the ip child.
    //
    NetLibDestroyServiceChild (
      ControllerHandle,
      ImageHandle,
      &gEfiIp4ServiceBindingProtocolGuid,
      *ChildHandle
      );
  }

  return Status;
}

STATIC
EFI_STATUS
IpIoCloseProtocolDestroyIpChild (
  IN EFI_HANDLE  ControllerHandle,
  IN EFI_HANDLE  ImageHandle,
  IN EFI_HANDLE  ChildHandle
  )
/*++

Routine Description:

  This function close the previously openned ip protocol and destroy the ip child.

Arguments:

  ControllerHandle - The controller handle.
  ImageHandle      - the image handle.
  ChildHandle      - The child handle of the ip child.

Returns:

  EFI_SUCCESS - The ip protocol is closed and the relevant ip child is destroyed.
  other       - The required operation failed.

--*/
{
  EFI_STATUS  Status;

  //
  // Close the previously openned ip protocol.
  //
  gBS->CloseProtocol (
         ChildHandle,
         &gEfiIp4ProtocolGuid,
         ImageHandle,
         ControllerHandle
         );

  //
  // Destroy the ip child.
  //
  Status = NetLibDestroyServiceChild (
             ControllerHandle,
             ImageHandle,
             &gEfiIp4ServiceBindingProtocolGuid,
             ChildHandle
             );

  return Status;
}

STATIC
EFI_STATUS
IpIoIcmpHandler (
  IN IP_IO                *IpIo,
  IN NET_BUF              *Pkt,
  IN EFI_NET_SESSION_DATA *Session
  )
/*++

Routine Description:

  Handle ICMP packets.

Arguments:

  IpIo    - Pointer to the IP_IO instance.
  Pkt     - Pointer to the ICMP packet.
  Session - Pointer to the net session of this ICMP packet.

Returns:

  EFI_SUCCESS - The ICMP packet is handled successfully.
  EFI_ABORTED - This type of ICMP packet is not supported.

--*/
{
  IP4_ICMP_ERROR_HEAD  *IcmpHdr;
  EFI_IP4_HEADER       *IpHdr;
  ICMP_ERROR           IcmpErr;
  UINT8                *PayLoadHdr;
  UINT8                Type;
  UINT8                Code;
  UINT32               TrimBytes;

  IcmpHdr = NET_PROTO_HDR (Pkt, IP4_ICMP_ERROR_HEAD);
  IpHdr   = (EFI_IP4_HEADER *) (&IcmpHdr->IpHead);

  //
  // Check the ICMP packet length.
  //
  if (Pkt->TotalSize < ICMP_ERRLEN (IpHdr)) {

    return EFI_ABORTED;
  }

  Type = IcmpHdr->Head.Type;
  Code = IcmpHdr->Head.Code;

  //
  // Analyze the ICMP Error in this ICMP pkt
  //
  switch (Type) {
  case ICMP_TYPE_UNREACH:
    switch (Code) {
    case ICMP_CODE_UNREACH_NET:
    case ICMP_CODE_UNREACH_HOST:
    case ICMP_CODE_UNREACH_PROTOCOL:
    case ICMP_CODE_UNREACH_PORT:
    case ICMP_CODE_UNREACH_SRCFAIL:
      IcmpErr = ICMP_ERR_UNREACH_NET + Code;

      break;

    case ICMP_CODE_UNREACH_NEEDFRAG:
      IcmpErr = ICMP_ERR_MSGSIZE;

      break;

    case ICMP_CODE_UNREACH_NET_UNKNOWN:
    case ICMP_CODE_UNREACH_NET_PROHIB:
    case ICMP_CODE_UNREACH_TOSNET:
      IcmpErr = ICMP_ERR_UNREACH_NET;

      break;

    case ICMP_CODE_UNREACH_HOST_UNKNOWN:
    case ICMP_CODE_UNREACH_ISOLATED:
    case ICMP_CODE_UNREACH_HOST_PROHIB:
    case ICMP_CODE_UNREACH_TOSHOST:
      IcmpErr = ICMP_ERR_UNREACH_HOST;

      break;

    default:
      return EFI_ABORTED;

      break;
    }

    break;

  case ICMP_TYPE_TIMXCEED:
    if (Code > 1) {
      return EFI_ABORTED;
    }

    IcmpErr = Code + ICMP_ERR_TIMXCEED_INTRANS;

    break;

  case ICMP_TYPE_PARAMPROB:
    if (Code > 1) {
      return EFI_ABORTED;
    }

    IcmpErr = ICMP_ERR_PARAMPROB;

    break;

  case ICMP_TYPE_SOURCEQUENCH:
    if (Code != 0) {
      return EFI_ABORTED;
    }

    IcmpErr = ICMP_ERR_QUENCH;

    break;

  default:
    return EFI_ABORTED;

    break;
  }

  //
  // Notify user the ICMP pkt only containing payload except
  // IP and ICMP header
  //
  PayLoadHdr = (UINT8 *) ((UINT8 *) IpHdr + EFI_IP4_HEADER_LEN (IpHdr));
  TrimBytes  = (UINT32) (PayLoadHdr - (UINT8 *) IcmpHdr);

  NetbufTrim (Pkt, TrimBytes, TRUE);

  IpIo->PktRcvdNotify (EFI_ICMP_ERROR, IcmpErr, Session, Pkt, IpIo->RcvdContext);

  return EFI_SUCCESS;
}

STATIC
VOID
IpIoExtFree (
  IN VOID  *Event
  )
/*++

Routine Description:

  Ext free function for net buffer. This function is
  called when the net buffer is freed. It is used to
  signal the recycle event to notify IP to recycle the
  data buffer.

Arguments:

  Event - The event to be signaled.

Returns:

  None.

--*/
{
  gBS->SignalEvent ((EFI_EVENT) Event);
}

STATIC
IP_IO_SEND_ENTRY *
IpIoCreateSndEntry (
  IN IP_IO             *IpIo,
  IN NET_BUF           *Pkt,
  IN EFI_IP4_PROTOCOL  *Sender,
  IN VOID              *Context    OPTIONAL,
  IN VOID              *NotifyData OPTIONAL,
  IN IP4_ADDR          Dest,
  IN IP_IO_OVERRIDE    *Override
  )
/*++

Routine Description:

  Create a send entry to wrap a packet before sending
  out it through IP.

Arguments:

  IpIo        - Pointer to the IP_IO instance.
  Pkt         - Pointer to the packet.
  Sender      - Pointer to the IP sender.
  NotifyData  - Pointer to the notify data.
  Dest        - Pointer to the destination IP address.
  Override    - Pointer to the overriden IP_IO data.

Returns:

  Pointer to the data structure created to wrap the packet. If NULL,
  resource limit occurred.

--*/
{
  IP_IO_SEND_ENTRY          *SndEntry;
  EFI_IP4_COMPLETION_TOKEN  *SndToken;
  EFI_IP4_TRANSMIT_DATA     *TxData;
  EFI_STATUS                Status;
  EFI_IP4_OVERRIDE_DATA     *OverrideData;
  UINT32                    Index;

  //
  // Allocate resource for SndEntry
  //
  SndEntry = NetAllocatePool (sizeof (IP_IO_SEND_ENTRY));
  if (NULL == SndEntry) {
    return NULL;
  }

  //
  // Allocate resource for SndToken
  //
  SndToken = NetAllocatePool (sizeof (EFI_IP4_COMPLETION_TOKEN));
  if (NULL == SndToken) {
    goto ReleaseSndEntry;
  }

  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  NET_TPL_EVENT,
                  IpIoTransmitHandler,
                  SndEntry,
                  &(SndToken->Event)
                  );
  if (EFI_ERROR (Status)) {
    goto ReleaseSndToken;
  }

  //
  // Allocate resource for TxData
  //
  TxData = NetAllocatePool (
    sizeof (EFI_IP4_TRANSMIT_DATA) +
    sizeof (EFI_IP4_FRAGMENT_DATA) * (Pkt->BlockOpNum - 1)
    );

  if (NULL == TxData) {
    goto ReleaseEvent;
  }

  //
  // Allocate resource for OverrideData if needed
  //
  OverrideData = NULL;
  if (NULL != Override) {

    OverrideData = NetAllocatePool (sizeof (EFI_IP4_OVERRIDE_DATA));
    if (NULL == OverrideData) {
      goto ReleaseResource;
    }
    //
    // Set the fields of OverrideData
    //
    *OverrideData = *Override;
  }

  //
  // Set the fields of TxData
  //
  NetCopyMem (&TxData->DestinationAddress, &Dest, sizeof (EFI_IPv4_ADDRESS));

  TxData->OverrideData    = OverrideData;
  TxData->OptionsLength   = 0;
  TxData->OptionsBuffer   = NULL;
  TxData->TotalDataLength = Pkt->TotalSize;
  TxData->FragmentCount   = Pkt->BlockOpNum;

  for (Index = 0; Index < Pkt->BlockOpNum; Index++) {

    TxData->FragmentTable[Index].FragmentBuffer = Pkt->BlockOp[Index].Head;
    TxData->FragmentTable[Index].FragmentLength = Pkt->BlockOp[Index].Size;
  }

  //
  // Set the fields of SndToken
  //
  SndToken->Packet.TxData = TxData;

  //
  // Set the fields of SndEntry
  //
  SndEntry->IpIo        = IpIo;
  SndEntry->Ip      = Sender;
  SndEntry->Context     = Context;
  SndEntry->NotifyData  = NotifyData;

  SndEntry->Pkt         = Pkt;
  NET_GET_REF (Pkt);

  SndEntry->SndToken = SndToken;

  NetListInsertTail (&IpIo->PendingSndList, &SndEntry->Entry);

  return SndEntry;

ReleaseResource:
  NetFreePool (TxData);

ReleaseEvent:
  gBS->CloseEvent (SndToken->Event);

ReleaseSndToken:
  NetFreePool (SndToken);

ReleaseSndEntry:
  NetFreePool (SndEntry);

  return NULL;
}

STATIC
VOID
IpIoDestroySndEntry (
  IN IP_IO_SEND_ENTRY  *SndEntry
  )
/*++

Routine Description:

  Destroy the SndEntry.

Arguments:

  SndEntry  - Pointer to the send entry to be destroyed.

Returns:

  None.

--*/
{
  EFI_IP4_TRANSMIT_DATA  *TxData;

  TxData = SndEntry->SndToken->Packet.TxData;

  if (NULL != TxData->OverrideData) {
    NetFreePool (TxData->OverrideData);
  }

  NetFreePool (TxData);
  NetbufFree (SndEntry->Pkt);
  gBS->CloseEvent (SndEntry->SndToken->Event);

  NetFreePool (SndEntry->SndToken);
  NetListRemoveEntry (&SndEntry->Entry);

  NetFreePool (SndEntry);
}

STATIC
VOID
EFIAPI
IpIoTransmitHandlerDpc (
  IN VOID      *Context
  )
/*++

Routine Description:

  Notify function for IP transmit token.

Arguments:

  Event   - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  IP_IO             *IpIo;
  IP_IO_SEND_ENTRY  *SndEntry;

  SndEntry  = (IP_IO_SEND_ENTRY *) Context;

  IpIo      = SndEntry->IpIo;

  if (IpIo->PktSentNotify && SndEntry->NotifyData) {
    IpIo->PktSentNotify (
            SndEntry->SndToken->Status,
            SndEntry->Context,
            SndEntry->Ip,
            SndEntry->NotifyData
            );
  }

  IpIoDestroySndEntry (SndEntry);
}

STATIC
VOID
EFIAPI
IpIoTransmitHandler (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/*++

Routine Description:

  Notify function for IP transmit token.

Arguments:

  Event   - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  //
  // Request IpIoTransmitHandlerDpc as a DPC at EFI_TPL_CALLBACK 
  //
  NetLibQueueDpc (EFI_TPL_CALLBACK, IpIoTransmitHandlerDpc, Context);
}

STATIC
VOID
EFIAPI
IpIoDummyHandlerDpc (
  IN VOID      *Context
  )
/*++

Routine Description:

  The dummy handler for the dummy IP receive token.

Arguments:

  Evt     - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  IP_IO_IP_INFO             *IpInfo;
  EFI_IP4_COMPLETION_TOKEN  *DummyToken;

  IpInfo      = (IP_IO_IP_INFO *) Context;
  DummyToken  = &(IpInfo->DummyRcvToken);

  if (EFI_ABORTED == DummyToken->Status) {
    //
    // The reception is actively aborted by the consumer, directly return.
    //
    return;
  } else if (EFI_SUCCESS == DummyToken->Status) {
    ASSERT (DummyToken->Packet.RxData);

    gBS->SignalEvent (DummyToken->Packet.RxData->RecycleSignal);
  }

  IpInfo->Ip->Receive (IpInfo->Ip, DummyToken);
}

STATIC
VOID
EFIAPI
IpIoDummyHandler (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/*++

Routine Description:

  Request IpIoDummyHandlerDpc as a DPC at EFI_TPL_CALLBACK.

Arguments:

  Evt     - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  //
  // Request IpIoDummyHandlerDpc as a DPC at EFI_TPL_CALLBACK 
  //
  NetLibQueueDpc (EFI_TPL_CALLBACK, IpIoDummyHandlerDpc, Context);
}

STATIC
VOID
EFIAPI
IpIoListenHandlerDpc (
  IN VOID      *Context
  )
/*++

Routine Description:

  Notify function for the IP receive token, used to process
  the received IP packets.

Arguments:

  Event   - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  IP_IO                 *IpIo;
  EFI_STATUS            Status;
  EFI_IP4_RECEIVE_DATA  *RxData;
  EFI_IP4_PROTOCOL      *Ip;
  EFI_NET_SESSION_DATA  Session;
  NET_BUF               *Pkt;

  IpIo    = (IP_IO *) Context;

  Ip      = IpIo->Ip;
  Status  = IpIo->RcvToken.Status;
  RxData  = IpIo->RcvToken.Packet.RxData;

  if (EFI_ABORTED == Status) {
    //
    // The reception is actively aborted by the consumer, directly return.
    //
    return;
  }

  if (((EFI_SUCCESS != Status) && (EFI_ICMP_ERROR != Status)) || (NULL == RxData)) {
    //
    // Only process the normal packets and the icmp error packets, if RxData is NULL
    // with Status == EFI_SUCCESS or EFI_ICMP_ERROR, just resume the receive although
    // this should be a bug of the low layer (IP).
    //
    goto Resume;
  }

  if (NULL == IpIo->PktRcvdNotify) {
    goto CleanUp;
  }

  if ((EFI_IP4 (RxData->Header->SourceAddress) != 0) &&
    !Ip4IsUnicast (EFI_NTOHL (RxData->Header->SourceAddress), 0)) {
    //
    // The source address is not zero and it's not a unicast IP address, discard it.
    //
    goto CleanUp;
  }

  //
  // Create a netbuffer representing packet
  //
  Pkt = NetbufFromExt (
          (NET_FRAGMENT *) RxData->FragmentTable,
          RxData->FragmentCount,
          0,
          0,
          IpIoExtFree,
          RxData->RecycleSignal
          );
  if (NULL == Pkt) {
    goto CleanUp;
  }

  //
  // Create a net session
  //
  Session.Source = EFI_IP4 (RxData->Header->SourceAddress);
  Session.Dest   = EFI_IP4 (RxData->Header->DestinationAddress);
  Session.IpHdr  = RxData->Header;

  if (EFI_SUCCESS == Status) {

    IpIo->PktRcvdNotify (EFI_SUCCESS, 0, &Session, Pkt, IpIo->RcvdContext);
  } else {
    //
    // Status is EFI_ICMP_ERROR
    //
    Status = IpIoIcmpHandler (IpIo, Pkt, &Session);
    if (EFI_ERROR (Status)) {
      NetbufFree (Pkt);
    }
  }

  goto Resume;

CleanUp:
  gBS->SignalEvent (RxData->RecycleSignal);

Resume:
  Ip->Receive (Ip, &(IpIo->RcvToken));
}

STATIC
VOID
EFIAPI
IpIoListenHandler (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/*++

Routine Description:

  Request IpIoListenHandlerDpc as a DPC at EFI_TPL_CALLBACK 

Arguments:

  Event   - The event signaled.
  Context - The context passed in by the event notifier.

Returns:

  None.

--*/
{
  //
  // Request IpIoListenHandlerDpc as a DPC at EFI_TPL_CALLBACK 
  //
  NetLibQueueDpc (EFI_TPL_CALLBACK, IpIoListenHandlerDpc, Context);
}

IP_IO *
IpIoCreate (
  IN EFI_HANDLE Image,
  IN EFI_HANDLE Controller
  )
/*++

Routine Description:

  Create a new IP_IO instance.

Arguments:

  Image       - The image handle of an IP_IO consumer protocol.
  Controller  - The controller handle of an IP_IO consumer
                protocol installed on.

Returns:

  Pointer to a newly created IP_IO instance.

--*/
{
  EFI_STATUS  Status;
  IP_IO       *IpIo;

  IpIo = NetAllocateZeroPool (sizeof (IP_IO));
  if (NULL == IpIo) {
    return NULL;
  }

  NetListInit (&(IpIo->PendingSndList));
  NetListInit (&(IpIo->IpList));
  IpIo->Controller  = Controller;
  IpIo->Image       = Image;

  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  NET_TPL_EVENT,
                  IpIoListenHandler,
                  IpIo,
                  &(IpIo->RcvToken.Event)
                  );
  if (EFI_ERROR (Status)) {
    goto ReleaseIpIo;
  }

  //
  // Create an IP child and open IP protocol
  //
  Status = IpIoCreateIpChildOpenProtocol (
             Controller,
             Image,
             &IpIo->ChildHandle,
             (VOID **)&(IpIo->Ip)
             );
  if (EFI_ERROR (Status)) {
    goto ReleaseIpIo;
  }

  return IpIo;

ReleaseIpIo:

  if (NULL != IpIo->RcvToken.Event) {
    gBS->CloseEvent (IpIo->RcvToken.Event);
  }

  NetFreePool (IpIo);

  return NULL;
}

EFI_STATUS
IpIoOpen (
  IN IP_IO           *IpIo,
  IN IP_IO_OPEN_DATA *OpenData
  )
/*++

Routine Description:

  Open an IP_IO instance for use.

Arguments:

  IpIo      - Pointer to an IP_IO instance that needs to open.
  OpenData  - The configuration data for the IP_IO instance.

Returns:

  EFI_SUCCESS - The IP_IO instance opened with OpenData successfully.
  other       - Error condition occurred.

--*/
{
  EFI_STATUS        Status;
  EFI_IP4_PROTOCOL  *Ip;

  if (IpIo->IsConfigured) {
    return EFI_ACCESS_DENIED;
  }

  Ip = IpIo->Ip;

  //
  // configure ip
  //
  Status = Ip->Configure (Ip, &OpenData->IpConfigData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // bugbug: to delete the default route entry in this Ip, if it is:
  // (0.0.0.0, 0.0.0.0, 0.0.0.0). Delete this statement if Ip modified
  // its code
  //
  Status = Ip->Routes (Ip, TRUE, &mZeroIp4Addr, &mZeroIp4Addr, &mZeroIp4Addr);

  if (EFI_ERROR (Status) && (EFI_NOT_FOUND != Status)) {
    return Status;
  }

  IpIo->PktRcvdNotify = OpenData->PktRcvdNotify;
  IpIo->PktSentNotify = OpenData->PktSentNotify;

  IpIo->RcvdContext   = OpenData->RcvdContext;
  IpIo->SndContext    = OpenData->SndContext;

  IpIo->Protocol      = OpenData->IpConfigData.DefaultProtocol;

  //
  // start to listen incoming packet
  //
  Status = Ip->Receive (Ip, &(IpIo->RcvToken));
  if (EFI_ERROR (Status)) {
    Ip->Configure (Ip, NULL);
    goto ErrorExit;
  }

  IpIo->IsConfigured = TRUE;
  NetListInsertTail (&mActiveIpIoList, &IpIo->Entry);

ErrorExit:

  return Status;
}

EFI_STATUS
IpIoStop (
  IN IP_IO *IpIo
  )
/*++

Routine Description:

  Stop an IP_IO instance.

Arguments:

  IpIo  - Pointer to the IP_IO instance that needs to stop.

Returns:

  EFI_SUCCESS - The IP_IO instance stopped successfully.
  other       - Error condition occurred.

--*/
{
  EFI_STATUS        Status;
  EFI_IP4_PROTOCOL  *Ip;
  IP_IO_IP_INFO     *IpInfo;

  if (!IpIo->IsConfigured) {
    return EFI_SUCCESS;
  }

  //
  // Remove the IpIo from the active IpIo list.
  //
  NetListRemoveEntry (&IpIo->Entry);

  Ip = IpIo->Ip;

  //
  // Configure NULL Ip
  //
  Status = Ip->Configure (Ip, NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IpIo->IsConfigured = FALSE;

  //
  // Detroy the Ip List used by IpIo
  //
  while (!NetListIsEmpty (&(IpIo->IpList))) {
    IpInfo = NET_LIST_HEAD (&(IpIo->IpList), IP_IO_IP_INFO, Entry);

    IpIoRemoveIp (IpIo, IpInfo);
  }

  //
  // All pending snd tokens should be flushed by reseting the IP instances.
  //
  ASSERT (NetListIsEmpty (&IpIo->PendingSndList));

  //
  // Close the receive event.
  //
  gBS->CloseEvent (IpIo->RcvToken.Event);

  return EFI_SUCCESS;
}

EFI_STATUS
IpIoDestroy (
  IN IP_IO *IpIo
  )
/*++

Routine Description:

  Destroy an IP_IO instance.

Arguments:

  IpIo  - Pointer to the IP_IO instance that needs to destroy.

Returns:

  EFI_SUCCESS - The IP_IO instance destroyed successfully.
  other       - Error condition occurred.

--*/
{
  //
  // Stop the IpIo.
  //
  IpIoStop (IpIo);

  //
  // Close the IP protocol and destroy the child.
  //
  IpIoCloseProtocolDestroyIpChild (IpIo->Controller, IpIo->Image, IpIo->ChildHandle);

  NetFreePool (IpIo);

  return EFI_SUCCESS;
}

EFI_STATUS
IpIoSend (
  IN IP_IO           *IpIo,
  IN NET_BUF         *Pkt,
  IN IP_IO_IP_INFO   *Sender,
  IN VOID            *Context    OPTIONAL,
  IN VOID            *NotifyData OPTIONAL,
  IN IP4_ADDR        Dest,
  IN IP_IO_OVERRIDE  *OverrideData
  )
/*++

Routine Description:

  Send out an IP packet.

Arguments:

  IpIo          - Pointer to an IP_IO instance used for sending IP packet.
  Pkt           - Pointer to the IP packet to be sent.
  Sender        - The IP protocol instance used for sending.
  NotifyData    - 
  Dest          - The destination IP address to send this packet to.
  OverrideData  - The data to override some configuration of the IP
                  instance used for sending.

Returns:

  EFI_SUCCESS          - The operation is completed successfully.
  EFI_NOT_STARTED      - The IpIo is not configured.
  EFI_OUT_OF_RESOURCES - Failed due to resource limit.

--*/
{
  EFI_STATUS        Status;
  EFI_IP4_PROTOCOL  *Ip;
  IP_IO_SEND_ENTRY  *SndEntry;

  if (!IpIo->IsConfigured) {
    return EFI_NOT_STARTED;
  }

  Ip = (NULL == Sender) ? IpIo->Ip : Sender->Ip;

  //
  // create a new SndEntry
  //
  SndEntry = IpIoCreateSndEntry (IpIo, Pkt, Ip, Context, NotifyData, Dest, OverrideData);
  if (NULL == SndEntry) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Send this Packet
  //
  Status = Ip->Transmit (Ip, SndEntry->SndToken);
  if (EFI_ERROR (Status)) {
    IpIoDestroySndEntry (SndEntry);
  }

  return Status;
}

VOID
IpIoCancelTxToken (
  IN IP_IO  *IpIo,
  IN VOID   *Packet
  )
/*++

Routine Description:

  Cancel the IP transmit token which wraps this Packet.

Arguments:

  IpIo   - Pointer to the IP_IO instance.
  Packet - Pointer to the packet to cancel.

Returns:

  N/A.

--*/

{
  NET_LIST_ENTRY    *Node;
  IP_IO_SEND_ENTRY  *SndEntry;
  EFI_IP4_PROTOCOL  *Ip;

  ASSERT (IpIo && Packet);

  NET_LIST_FOR_EACH (Node, &IpIo->PendingSndList) {

    SndEntry = NET_LIST_USER_STRUCT (Node, IP_IO_SEND_ENTRY, Entry);

    if (SndEntry->Pkt == Packet) {

      Ip = SndEntry->Ip;
      Ip->Cancel (Ip, SndEntry->SndToken);

      break;
    }
  }

}

IP_IO_IP_INFO *
IpIoAddIp (
  IN IP_IO  *IpIo
  )
/*++

Routine Description:

  Add a new IP instance for sending data.

Arguments:

  IpIo - Pointer to a IP_IO instance to add a new IP instance for sending purpose.

Returns:

  Pointer to the created IP_IO_IP_INFO structure, NULL is failed.

--*/
{
  EFI_STATUS     Status;
  IP_IO_IP_INFO  *IpInfo;

  ASSERT (IpIo);

  IpInfo = NetAllocatePool (sizeof (IP_IO_IP_INFO));
  if (IpInfo == NULL) {
    return IpInfo;
  }

  //
  // Init this IpInfo, set the Addr and SubnetMask to 0 before we configure the IP
  // instance.
  //
  NetListInit (&IpInfo->Entry);
  IpInfo->ChildHandle = NULL;
  IpInfo->Addr        = 0;
  IpInfo->SubnetMask  = 0;
  IpInfo->RefCnt      = 1;

  //
  // Create the IP instance and open the Ip4 protocol.
  //
  Status = IpIoCreateIpChildOpenProtocol (
             IpIo->Controller,
             IpIo->Image,
             &IpInfo->ChildHandle,
             &IpInfo->Ip
             );
  if (EFI_ERROR (Status)) {
    goto ReleaseIpInfo;
  }

  //
  // Create the event for the DummyRcvToken.
  //
  Status = gBS->CreateEvent (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  NET_TPL_EVENT,
                  IpIoDummyHandler,
                  IpInfo,
                  &IpInfo->DummyRcvToken.Event
                  );
  if (EFI_ERROR (Status)) {
    goto ReleaseIpChild;
  }

  //
  // Link this IpInfo into the IpIo.
  //
  NetListInsertTail (&IpIo->IpList, &IpInfo->Entry);

  return IpInfo;

ReleaseIpChild:

  IpIoCloseProtocolDestroyIpChild (
    IpIo->Controller,
    IpIo->Image,
    IpInfo->ChildHandle
    );

ReleaseIpInfo:

  NetFreePool (IpInfo);

  return NULL;
}

EFI_STATUS
IpIoConfigIp (
  IN     IP_IO_IP_INFO        *IpInfo,
  IN OUT EFI_IP4_CONFIG_DATA  *Ip4ConfigData OPTIONAL
  )
/*++

Routine Description:

  Configure the IP instance of this IpInfo and start the receiving if Ip4ConfigData
  is not NULL.

Arguments:

  IpInfo         - Pointer to the IP_IO_IP_INFO instance.
  Ip4ConfigData  - The IP4 configure data used to configure the ip instance, if NULL
                   the ip instance is reseted. If UseDefaultAddress is set to TRUE, and
                   the configure operation succeeds, the default address information is
                   written back in this Ip4ConfigData.

Returns:

  EFI_STATUS     - The status returned by IP4->Configure or IP4->Receive.

--*/
{
  EFI_STATUS         Status;
  EFI_IP4_PROTOCOL   *Ip;
  EFI_IP4_MODE_DATA  Ip4ModeData;

  ASSERT (IpInfo);

  if (IpInfo->RefCnt > 1) {
    //
    // This IP instance is shared, don't reconfigure it until it has only one
    // consumer. Currently, only the tcp children cloned from their passive parent
    // will share the same IP. So this cases only happens while Ip4ConfigData is NULL,
    // let the last consumer clean the IP instance.
    //
    return EFI_SUCCESS;
  }

  Ip     = IpInfo->Ip;
  Status = Ip->Configure (Ip, Ip4ConfigData);
  if (EFI_ERROR (Status)) {
    goto OnExit;
  }

  if (Ip4ConfigData != NULL) {

    if (Ip4ConfigData->UseDefaultAddress) {
      Ip->GetModeData (Ip, &Ip4ModeData, NULL, NULL);

      Ip4ConfigData->StationAddress = Ip4ModeData.ConfigData.StationAddress;
      Ip4ConfigData->SubnetMask     = Ip4ModeData.ConfigData.SubnetMask;
    }

    NetCopyMem (&IpInfo->Addr, &Ip4ConfigData->StationAddress, sizeof (IP4_ADDR));
    NetCopyMem (&IpInfo->SubnetMask, &Ip4ConfigData->SubnetMask, sizeof (IP4_ADDR));

    Status = Ip->Receive (Ip, &IpInfo->DummyRcvToken);
    if (EFI_ERROR (Status)) {
      Ip->Configure (Ip, NULL);
    }
  } else {

    //
    // The IP instance is reseted, set the stored Addr and SubnetMask to zero.
    //
    IpInfo->Addr       = 0;
    IpInfo->SubnetMask =0;
  }

OnExit:

  return Status;
}

VOID
IpIoRemoveIp (
  IN IP_IO          *IpIo,
  IN IP_IO_IP_INFO  *IpInfo
  )
/*++

Routine Description:

  Destroy an IP instance maintained in IpIo->IpList for
  sending purpose.

Arguments:

  IpIo   - Pointer to the IP_IO instance.
  IpInfo - Pointer to the IpInfo to be removed.

Returns:

  None.

--*/
{
  ASSERT (IpInfo->RefCnt > 0);

  NET_PUT_REF (IpInfo);

  if (IpInfo->RefCnt > 0) {

    return;
  }

  NetListRemoveEntry (&IpInfo->Entry);

  IpInfo->Ip->Configure (IpInfo->Ip, NULL);

  IpIoCloseProtocolDestroyIpChild (IpIo->Controller, IpIo->Image, IpInfo->ChildHandle);

  gBS->CloseEvent (IpInfo->DummyRcvToken.Event);

  NetFreePool (IpInfo);
}

IP_IO_IP_INFO *
IpIoFindSender (
  IN OUT IP_IO     **IpIo,
  IN     IP4_ADDR  Src
  )
/*++

Routine Description:

  Find the first IP protocol maintained in IpIo whose local
  address is the same with Src.

Arguments:

  IpIo  - Pointer to the pointer of the IP_IO instance.
  Src   - The local IP address.

Returns:

  Pointer to the IP protocol can be used for sending purpose and its local
  address is the same with Src.

--*/
{
  NET_LIST_ENTRY  *IpIoEntry;
  IP_IO           *IpIoPtr;
  NET_LIST_ENTRY  *IpInfoEntry;
  IP_IO_IP_INFO   *IpInfo;

  NET_LIST_FOR_EACH (IpIoEntry, &mActiveIpIoList) {
    IpIoPtr = NET_LIST_USER_STRUCT (IpIoEntry, IP_IO, Entry);

    if ((*IpIo != NULL) && (*IpIo != IpIoPtr)) {
      continue;
    }

    NET_LIST_FOR_EACH (IpInfoEntry, &IpIoPtr->IpList) {
      IpInfo = NET_LIST_USER_STRUCT (IpInfoEntry, IP_IO_IP_INFO, Entry);

      if (IpInfo->Addr == Src) {
        *IpIo = IpIoPtr;
        return IpInfo;
      }
    }
  }

  //
  // No match.
  //
  return NULL;
}

EFI_STATUS
IpIoGetIcmpErrStatus (
  IN  ICMP_ERROR  IcmpError,
  OUT BOOLEAN     *IsHard, OPTIONAL
  OUT BOOLEAN     *Notify OPTIONAL
  )
/*++

Routine Description:

  Get the ICMP error map information, the ErrorStatus will be returned.
  The IsHard and Notify are optional. If they are not NULL, this rouine will
  fill them.
  We move IcmpErrMap[] to local variable to enable EBC build.

Arguments:

  IcmpError     - IcmpError Type
  IsHard        - Whether it is a hard error
  Notify        - Whether it need to notify SockError

Returns:

  ICMP Error Status

--*/
{
  ICMP_ERROR_INFO  IcmpErrMap[] = {
    { EFI_NETWORK_UNREACHABLE,  FALSE, TRUE  }, // ICMP_ERR_UNREACH_NET
    { EFI_HOST_UNREACHABLE,     FALSE, TRUE  }, // ICMP_ERR_UNREACH_HOST
    { EFI_PROTOCOL_UNREACHABLE, TRUE,  TRUE  }, // ICMP_ERR_UNREACH_PROTOCOL
    { EFI_PORT_UNREACHABLE,     TRUE,  TRUE  }, // ICMP_ERR_UNREACH_PORT
    { EFI_ICMP_ERROR,           TRUE,  TRUE  }, // ICMP_ERR_MSGSIZE
    { EFI_ICMP_ERROR,           FALSE, TRUE  }, // ICMP_ERR_UNREACH_SRCFAIL
    { EFI_HOST_UNREACHABLE,     FALSE, TRUE  }, // ICMP_ERR_TIMXCEED_INTRANS
    { EFI_HOST_UNREACHABLE,     FALSE, TRUE  }, // ICMP_ERR_TIMEXCEED_REASS
    { EFI_ICMP_ERROR,           FALSE, FALSE }, // ICMP_ERR_QUENCH
    { EFI_ICMP_ERROR,           FALSE, TRUE  }  // ICMP_ERR_PARAMPROB
  };

  ASSERT ((IcmpError >= ICMP_ERR_UNREACH_NET) && (IcmpError <= ICMP_ERR_PARAMPROB));

  if (IsHard != NULL) {
    *IsHard = IcmpErrMap[IcmpError].IsHard;
  }

  if (Notify != NULL) {
    *Notify = IcmpErrMap[IcmpError].Notify;
  }

  return IcmpErrMap[IcmpError].Error;
}

