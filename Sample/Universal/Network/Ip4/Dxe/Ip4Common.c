/*++

Copyright (c) 2005 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 


Module Name:

  Ip4Common.c

Abstract:

--*/

#include "Ip4Impl.h"

INTN
Ip4GetNetCast (
  IN  IP4_ADDR          IpAddr,
  IN  IP4_INTERFACE     *IpIf
  )
/*++

Routine Description:

  Return the cast type (Unicast/Boradcast) specific to a
  interface. All the addresses are host byte ordered. 

Arguments:

  IpAddr  - The IP address to classify in host byte order
  IpIf    - The interface that IpAddr received from

Returns:

  The cast type of this IP address specific to the interface.
  IP4_LOCAL_HOST       - The IpAddr equals to the interface's address
  IP4_SUBNET_BROADCAST - The IpAddr is a directed subnet boradcast to 
                         the interface
  IP4_NET_BROADCAST    - The IpAddr is a network broadcast to the interface                      
--*/
{
  if (IpAddr == IpIf->Ip) {
    return IP4_LOCAL_HOST;

  } else if (IpAddr == IpIf->SubnetBrdcast) {
    return IP4_SUBNET_BROADCAST;

  } else if (IpAddr == IpIf->NetBrdcast) {
    return IP4_NET_BROADCAST;

  }

  return 0;
}

INTN
Ip4GetHostCast (
  IN  IP4_SERVICE       *IpSb,
  IN  IP4_ADDR          Dst,
  IN  IP4_ADDR          Src
  )
/*++

Routine Description:

  Find the cast type of the packet related to the local host.
  This isn't the same as link layer cast type. For example, DHCP
  server may send local broadcast to the local unicast MAC.

Arguments:

  IpSb  - The IP4 service binding instance that received the packet
  Dst   - The destination address in the packet (host byte order)
  Src   - The source address in the packet (host byte order)

Returns:

  The cast type for the Dst, it will return on the first non-promiscuous
  cast type to a configured interface. If the packet doesn't match any of
  the interface, multicast address and local broadcast address are checked.

--*/
{
  NET_LIST_ENTRY        *Entry;
  IP4_INTERFACE         *IpIf;
  INTN                  Type;
  INTN                  Class;

  Type = 0;

  if (IpSb->MnpConfigData.EnablePromiscuousReceive) {
    Type = IP4_PROMISCUOUS;
  }
  
  //
  // Go through the interface list of the IP service, most likely.
  //
  NET_LIST_FOR_EACH (Entry, &IpSb->Interfaces) {
    IpIf = NET_LIST_USER_STRUCT (Entry, IP4_INTERFACE, Link);

    //
    // Skip the unconfigured interface and invalid source address:
    // source address can't be broadcast.
    //
    if (!IpIf->Configured || IP4_IS_BROADCAST (Ip4GetNetCast (Src, IpIf))) {
      continue;
    }

    if ((Class = Ip4GetNetCast (Dst, IpIf)) > Type) {
      return Class;
    }
  }
  
  //
  // If it is local broadcast address. The source address must
  // be a unicast address on one of the direct connected network.
  // If it is a multicast address, accept it only if we are in
  // the group.
  //
  if (Dst == IP4_ALLONE_ADDRESS) {
    IpIf = Ip4FindNet (IpSb, Src);

    if (IpIf && !IP4_IS_BROADCAST (Ip4GetNetCast (Src, IpIf))) {
      return IP4_LOCAL_BROADCAST;
    }
    
  } else if (IP4_IS_MULTICAST (Dst) && Ip4FindGroup (&IpSb->IgmpCtrl, Dst)) {
    return IP4_MULTICAST;
  }

  return Type;
}

IP4_INTERFACE *
Ip4FindInterface (
  IN IP4_SERVICE        *IpSb,
  IN IP4_ADDR           Ip
  )
/*++

Routine Description:

  Find an interface whose configured IP address is Ip

Arguments:

  IpSb  - The IP4 service binding instance
  Ip    - The Ip address (host byte order) to find

Returns:

  The IP4_INTERFACE point if found, otherwise NULL

--*/
{
  NET_LIST_ENTRY        *Entry;
  IP4_INTERFACE         *IpIf;

  NET_LIST_FOR_EACH (Entry, &IpSb->Interfaces) {
    IpIf = NET_LIST_USER_STRUCT (Entry, IP4_INTERFACE, Link);

    if (IpIf->Configured && (IpIf->Ip == Ip)) {
      return IpIf;
    }
  }

  return NULL;
}

IP4_INTERFACE *
Ip4FindNet (
  IN IP4_SERVICE        *IpSb,
  IN IP4_ADDR           Ip
  )
/*++

Routine Description:

  Find an interface that Ip is on that connected network.

Arguments:

  IpSb  - The IP4 service binding instance
  Ip    - The Ip address (host byte order) to find
  
Returns:

  The IP4_INTERFACE point if found, otherwise NULL

--*/
{
  NET_LIST_ENTRY        *Entry;
  IP4_INTERFACE         *IpIf;

  NET_LIST_FOR_EACH (Entry, &IpSb->Interfaces) {
    IpIf = NET_LIST_USER_STRUCT (Entry, IP4_INTERFACE, Link);

    if (IpIf->Configured && IP4_NET_EQUAL (Ip, IpIf->Ip, IpIf->SubnetMask)) {
      return IpIf;
    }
  }

  return NULL;
}

IP4_INTERFACE *
Ip4FindStationAddress (
  IN IP4_SERVICE        *IpSb,
  IN IP4_ADDR           Ip,
  IN IP4_ADDR           Netmask
  )
/*++

Routine Description:

  Find an interface of the service with the same Ip/Netmask pair.

Arguments:

  IpSb    - Ip4 service binding instance
  Ip      - The Ip adress to find (host byte order)
  Netmask - The network to find (host byte order)

Returns:

  The IP4_INTERFACE point if found, otherwise NULL

--*/
{
  NET_LIST_ENTRY  *Entry;
  IP4_INTERFACE   *IpIf;

  NET_LIST_FOR_EACH (Entry, &IpSb->Interfaces) {
    IpIf = NET_LIST_USER_STRUCT (Entry, IP4_INTERFACE, Link);

    if (IpIf->Configured && (IpIf->Ip == Ip) && (IpIf->SubnetMask == Netmask)) {
      return IpIf;
    }
  }

  return NULL;
}

EFI_STATUS
Ip4GetMulticastMac (
  IN  EFI_MANAGED_NETWORK_PROTOCOL *Mnp,
  IN  IP4_ADDR                     Multicast,
  OUT EFI_MAC_ADDRESS              *Mac
  )
/*++

Routine Description:

  Get the MAC address for a multicast IP address. Call
  Mnp's McastIpToMac to find the MAC address in stead of
  hard code the NIC to be Ethernet.

Arguments:

  Mnp       - The Mnp instance to get the MAC address.
  Multicast - The multicast IP address to translate.
  Mac       - The buffer to hold the translated address.

Returns:

  Returns EFI_SUCCESS if the multicast IP is successfully 
  translated to a multicast MAC address. Otherwise some error.
  

--*/
{
  EFI_IP_ADDRESS        EfiIp;

  EFI_IP4 (EfiIp.v4) = HTONL (Multicast);
  return Mnp->McastIpToMac (Mnp, FALSE, &EfiIp, Mac);
}

IP4_HEAD *
Ip4NtohHead (
  IN IP4_HEAD           *Head
  )
/*++

Routine Description:

  Convert the multibyte field in IP header's byter order. 
  In spite of its name, it can also be used to convert from 
  host to network byte order.
  
Arguments:

  Head  - The IP head to convert

Returns:

  Point to the converted IP head

--*/
{
  Head->TotalLen  = NTOHS (Head->TotalLen);
  Head->Id        = NTOHS (Head->Id);
  Head->Fragment  = NTOHS (Head->Fragment);
  Head->Src       = NTOHL (Head->Src);
  Head->Dst       = NTOHL (Head->Dst);

  return Head;
}

EFI_STATUS
Ip4SetVariableData (
  IN IP4_SERVICE  *IpSb
  )
/*++

Routine Description:

  Set the Ip4 variable data.

Arguments:

  IpSb - Ip4 service binding instance

Returns:

  EFI_OUT_OF_RESOURCES - There are not enough resources to set the variable.
  other                - Set variable failed.

--*/
{
  UINT32                 NumConfiguredInstance;
  NET_LIST_ENTRY         *Entry;
  UINTN                  VariableDataSize;
  EFI_IP4_VARIABLE_DATA  *Ip4VariableData;
  EFI_IP4_ADDRESS_PAIR   *Ip4AddressPair;
  IP4_PROTOCOL           *IpInstance;
  CHAR16                 *NewMacString;
  EFI_STATUS             Status;

  NumConfiguredInstance = 0;

  //
  // Go through the children list to count the configured children.
  //
  NET_LIST_FOR_EACH (Entry, &IpSb->Children) {
    IpInstance = NET_LIST_USER_STRUCT_S (Entry, IP4_PROTOCOL, Link, IP4_PROTOCOL_SIGNATURE);

    if (IpInstance->State == IP4_STATE_CONFIGED) {
      NumConfiguredInstance++;
    }
  }

  //
  // Calculate the size of the Ip4VariableData. As there may be no IP child,
  // we should add extra buffer for the address paris only if the number of configured
  // children is more than 1.
  //
  VariableDataSize = sizeof (EFI_IP4_VARIABLE_DATA);

  if (NumConfiguredInstance > 1) {
    VariableDataSize += sizeof (EFI_IP4_ADDRESS_PAIR) * (NumConfiguredInstance - 1);
  }
  
  Ip4VariableData = NetAllocatePool (VariableDataSize);
  if (Ip4VariableData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ip4VariableData->DriverHandle = IpSb->Image;
  Ip4VariableData->AddressCount = NumConfiguredInstance;

  Ip4AddressPair = &Ip4VariableData->AddressPairs[0];

  //
  // Go through the children list to fill the configured children's address pairs.
  //
  NET_LIST_FOR_EACH (Entry, &IpSb->Children) {
    IpInstance = NET_LIST_USER_STRUCT_S (Entry, IP4_PROTOCOL, Link, IP4_PROTOCOL_SIGNATURE);

    if (IpInstance->State == IP4_STATE_CONFIGED) {
      Ip4AddressPair->InstanceHandle       = IpInstance->Handle;
      EFI_IP4 (Ip4AddressPair->Ip4Address) = NTOHL (IpInstance->Interface->Ip);
      EFI_IP4 (Ip4AddressPair->SubnetMask) = NTOHL (IpInstance->Interface->SubnetMask);

      Ip4AddressPair++;
    }
  }

  //
  // Get the mac string.
  //
  Status = NetLibGetMacString (IpSb->Controller, IpSb->Image, &NewMacString);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  if (IpSb->MacString != NULL) {
    //
    // The variable is set already, we're going to update it.
    //
    if (EfiStrCmp (IpSb->MacString, NewMacString) != 0) {
      //
      // The mac address is changed, delete the previous variable first.
      //
      gRT->SetVariable (
             IpSb->MacString,
             &gEfiIp4ServiceBindingProtocolGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS,
             0,
             NULL
             );
    }

    NetFreePool (IpSb->MacString);
  }

  IpSb->MacString = NewMacString;

  Status = gRT->SetVariable (
                  IpSb->MacString,
                  &gEfiIp4ServiceBindingProtocolGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableDataSize,
                  (VOID *) Ip4VariableData
                  );

ON_ERROR:

  NetFreePool (Ip4VariableData);

  return Status;
}

VOID
Ip4ClearVariableData (
  IN IP4_SERVICE  *IpSb
  )
/*++

Routine Description:

  Clear the variable and free the resource.

Arguments:

  IpSb    - Ip4 service binding instance

Returns:

  None.

--*/
{
  ASSERT (IpSb->MacString != NULL);

  gRT->SetVariable (
         IpSb->MacString,
         &gEfiIp4ServiceBindingProtocolGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS,
         0,
         NULL
         );

  NetFreePool (IpSb->MacString);
  IpSb->MacString = NULL;
}
