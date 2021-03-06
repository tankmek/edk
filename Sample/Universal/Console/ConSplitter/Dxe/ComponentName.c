/*++

Copyright (c) 2004 - 2008, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  ComponentName.c

Abstract:

--*/

#include "ConSplitter.h"

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
ConSplitterComponentNameGetDriverName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
#endif
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

EFI_STATUS
EFIAPI
ConSplitterConInComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

EFI_STATUS
EFIAPI
ConSplitterSimplePointerComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#ifndef DISABLE_CONSOLE_EX
EFI_STATUS
EFIAPI
ConSplitterAbsolutePointerComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
;
#endif
#endif

EFI_STATUS
EFIAPI
ConSplitterConOutComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

EFI_STATUS
EFIAPI
ConSplitterStdErrComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

//
// EFI Component Name Protocol
//
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
EFI_COMPONENT_NAME2_PROTOCOL    gConSplitterConInComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterConInComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#else
EFI_COMPONENT_NAME_PROTOCOL     gConSplitterConInComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterConInComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
EFI_COMPONENT_NAME2_PROTOCOL    gConSplitterSimplePointerComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterSimplePointerComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#else
EFI_COMPONENT_NAME_PROTOCOL     gConSplitterSimplePointerComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterSimplePointerComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#ifndef DISABLE_CONSOLE_EX
EFI_COMPONENT_NAME2_PROTOCOL    gConSplitterAbsolutePointerComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterAbsolutePointerComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#endif
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
EFI_COMPONENT_NAME2_PROTOCOL    gConSplitterConOutComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterConOutComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#else
EFI_COMPONENT_NAME_PROTOCOL     gConSplitterConOutComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterConOutComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#endif

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
EFI_COMPONENT_NAME2_PROTOCOL    gConSplitterStdErrComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterStdErrComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#else
EFI_COMPONENT_NAME_PROTOCOL     gConSplitterStdErrComponentName = {
  ConSplitterComponentNameGetDriverName,
  ConSplitterStdErrComponentNameGetControllerName,
  LANGUAGE_CODE_ENGLISH
};
#endif

STATIC EFI_UNICODE_STRING_TABLE mConSplitterDriverNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Console Splitter Driver"
  },
  {
    NULL,
    NULL
  }
};

STATIC EFI_UNICODE_STRING_TABLE mConSplitterConInControllerNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Primary Console Input Device"
  },
  {
    NULL,
    NULL
  }
};

STATIC EFI_UNICODE_STRING_TABLE mConSplitterSimplePointerControllerNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Primary Simple Pointer Device"
  },
  {
    NULL,
    NULL
  }
};

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
STATIC EFI_UNICODE_STRING_TABLE mConSplitterAbsolutePointerControllerNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Primary Absolute Pointer Device"
  },
  {
    NULL,
    NULL
  }
};
#endif

STATIC EFI_UNICODE_STRING_TABLE mConSplitterConOutControllerNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Primary Console Output Device"
  },
  {
    NULL,
    NULL
  }
};

STATIC EFI_UNICODE_STRING_TABLE mConSplitterStdErrControllerNameTable[] = {
  {
    LANGUAGE_CODE_ENGLISH,
    L"Primary Standard Error Device"
  },
  {
    NULL,
    NULL
  }
};

EFI_STATUS
EFIAPI
ConSplitterComponentNameGetDriverName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
#endif
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the EFI Driver.

  Arguments:
    This       - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    Language   - A pointer to a three character ISO 639-2 language identifier.
                 This is the language of the driver name that that the caller 
                 is requesting, and it must match one of the languages specified
                 in SupportedLanguages.  The number of languages supported by a 
                 driver is up to the driver writer.
    DriverName - A pointer to the Unicode string to return.  This Unicode string
                 is the name of the driver specified by This in the language 
                 specified by Language.

  Returns:
    EFI_SUCCESS           - The Unicode string for the Driver specified by This
                            and the language specified by Language was returned 
                            in DriverName.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - DriverName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterConInComponentName.SupportedLanguages,
          mConSplitterDriverNameTable,
          DriverName
          );
}

EFI_STATUS
EFIAPI
ConSplitterConInComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the controller
    that is being managed by an EFI Driver.

  Arguments:
    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    ControllerHandle - The handle of a controller that the driver specified by 
                       This is managing.  This handle specifies the controller 
                       whose name is to be returned.
    ChildHandle      - The handle of the child controller to retrieve the name 
                       of.  This is an optional parameter that may be NULL.  It 
                       will be NULL for device drivers.  It will also be NULL 
                       for a bus drivers that wish to retrieve the name of the 
                       bus controller.  It will not be NULL for a bus driver 
                       that wishes to retrieve the name of a child controller.
    Language         - A pointer to a three character ISO 639-2 language 
                       identifier.  This is the language of the controller name 
                       that that the caller is requesting, and it must match one
                       of the languages specified in SupportedLanguages.  The 
                       number of languages supported by a driver is up to the 
                       driver writer.
    ControllerName   - A pointer to the Unicode string to return.  This Unicode
                       string is the name of the controller specified by 
                       ControllerHandle and ChildHandle in the language 
                       specified by Language from the point of view of the 
                       driver specified by This. 

  Returns:
    EFI_SUCCESS           - The Unicode string for the user readable name in the
                            language specified by Language for the driver 
                            specified by This was returned in DriverName.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ControllerName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This is not currently 
                            managing the controller specified by 
                            ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  EFI_STATUS                  Status;
  
  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiLibTestManagedDevice (
             ControllerHandle,
             gConSplitterConInDriverBinding.DriverBindingHandle,
             &gEfiConsoleInDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // here ChildHandle is not an Optional parameter.
  //
  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiLibTestChildHandle (
             ControllerHandle,
             ChildHandle,
             &gEfiConsoleInDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterConInComponentName.SupportedLanguages,
          mConSplitterConInControllerNameTable,
          ControllerName
          );
}

EFI_STATUS
EFIAPI
ConSplitterSimplePointerComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the controller
    that is being managed by an EFI Driver.

  Arguments:
    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    ControllerHandle - The handle of a controller that the driver specified by 
                       This is managing.  This handle specifies the controller 
                       whose name is to be returned.
    ChildHandle      - The handle of the child controller to retrieve the name 
                       of.  This is an optional parameter that may be NULL.  It 
                       will be NULL for device drivers.  It will also be NULL 
                       for a bus drivers that wish to retrieve the name of the 
                       bus controller.  It will not be NULL for a bus driver 
                       that wishes to retrieve the name of a child controller.
    Language         - A pointer to a three character ISO 639-2 language 
                       identifier.  This is the language of the controller name 
                       that that the caller is requesting, and it must match one
                       of the languages specified in SupportedLanguages.  The 
                       number of languages supported by a driver is up to the 
                       driver writer.
    ControllerName   - A pointer to the Unicode string to return.  This Unicode
                       string is the name of the controller specified by 
                       ControllerHandle and ChildHandle in the language 
                       specified by Language from the point of view of the 
                       driver specified by This. 

  Returns:
    EFI_SUCCESS           - The Unicode string for the user readable name in the
                            language specified by Language for the driver 
                            specified by This was returned in DriverName.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ControllerName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This is not currently 
                            managing the controller specified by 
                            ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  EFI_STATUS                  Status;
  
  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiLibTestManagedDevice (
             ControllerHandle,
             gConSplitterSimplePointerDriverBinding.DriverBindingHandle,
             &gEfiSimplePointerProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // here ChildHandle is not an Optional parameter.
  //
  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiLibTestChildHandle (
             ControllerHandle,
             ChildHandle,
             &gEfiSimplePointerProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterSimplePointerComponentName.SupportedLanguages,
          mConSplitterSimplePointerControllerNameTable,
          ControllerName
          );
}

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#ifndef DISABLE_CONSOLE_EX
EFI_STATUS
EFIAPI
ConSplitterAbsolutePointerComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the controller
    that is being managed by an EFI Driver.

  Arguments:
    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    ControllerHandle - The handle of a controller that the driver specified by 
                       This is managing.  This handle specifies the controller 
                       whose name is to be returned.
    ChildHandle      - The handle of the child controller to retrieve the name 
                       of.  This is an optional parameter that may be NULL.  It 
                       will be NULL for device drivers.  It will also be NULL 
                       for a bus drivers that wish to retrieve the name of the 
                       bus controller.  It will not be NULL for a bus driver 
                       that wishes to retrieve the name of a child controller.
    Language         - A pointer to RFC3066 language identifier. 
                       This is the language of the controller name 
                       that that the caller is requesting, and it must match one
                       of the languages specified in SupportedLanguages.  The 
                       number of languages supported by a driver is up to the 
                       driver writer.
    ControllerName   - A pointer to the Unicode string to return.  This Unicode
                       string is the name of the controller specified by 
                       ControllerHandle and ChildHandle in the language 
                       specified by Language from the point of view of the 
                       driver specified by This. 

  Returns:
    EFI_SUCCESS           - The Unicode string for the user readable name in the
                            language specified by Language for the driver 
                            specified by This was returned in DriverName.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ControllerName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This is not currently 
                            managing the controller specified by 
                            ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  EFI_STATUS                    Status;
  
  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiLibTestManagedDevice (
             ControllerHandle,
             gConSplitterAbsolutePointerDriverBinding.DriverBindingHandle,
             &gEfiAbsolutePointerProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // here ChildHandle is not an Optional parameter.
  //
  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiLibTestChildHandle (
             ControllerHandle,
             ChildHandle,
             &gEfiAbsolutePointerProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterAbsolutePointerComponentName.SupportedLanguages,
          mConSplitterAbsolutePointerControllerNameTable,
          ControllerName
          );
}
#endif
#endif

EFI_STATUS
EFIAPI
ConSplitterConOutComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the controller
    that is being managed by an EFI Driver.

  Arguments:
    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    ControllerHandle - The handle of a controller that the driver specified by 
                       This is managing.  This handle specifies the controller 
                       whose name is to be returned.
    ChildHandle      - The handle of the child controller to retrieve the name 
                       of.  This is an optional parameter that may be NULL.  It 
                       will be NULL for device drivers.  It will also be NULL 
                       for a bus drivers that wish to retrieve the name of the 
                       bus controller.  It will not be NULL for a bus driver 
                       that wishes to retrieve the name of a child controller.
    Language         - A pointer to a three character ISO 639-2 language 
                       identifier.  This is the language of the controller name 
                       that that the caller is requesting, and it must match one
                       of the languages specified in SupportedLanguages.  The 
                       number of languages supported by a driver is up to the 
                       driver writer.
    ControllerName   - A pointer to the Unicode string to return.  This Unicode
                       string is the name of the controller specified by 
                       ControllerHandle and ChildHandle in the language 
                       specified by Language from the point of view of the 
                       driver specified by This. 

  Returns:
    EFI_SUCCESS           - The Unicode string for the user readable name in the
                            language specified by Language for the driver 
                            specified by This was returned in DriverName.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ControllerName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This is not currently 
                            managing the controller specified by 
                            ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  EFI_STATUS                    Status;
  
  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiLibTestManagedDevice (
             ControllerHandle,
             gConSplitterConOutDriverBinding.DriverBindingHandle,
             &gEfiConsoleOutDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // here ChildHandle is not an Optional parameter.
  //
  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EfiLibTestChildHandle (
             ControllerHandle,
             ChildHandle,
             &gEfiConsoleOutDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterConOutComponentName.SupportedLanguages,
          mConSplitterConOutControllerNameTable,
          ControllerName
          );
}

EFI_STATUS
EFIAPI
ConSplitterStdErrComponentNameGetControllerName (
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  IN  EFI_COMPONENT_NAME2_PROTOCOL                    *This,
#else
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
#endif
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
/*++

  Routine Description:
    Retrieves a Unicode string that is the user readable name of the controller
    that is being managed by an EFI Driver.

  Arguments:
    This             - A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
    ControllerHandle - The handle of a controller that the driver specified by 
                       This is managing.  This handle specifies the controller 
                       whose name is to be returned.
    ChildHandle      - The handle of the child controller to retrieve the name 
                       of.  This is an optional parameter that may be NULL.  It 
                       will be NULL for device drivers.  It will also be NULL 
                       for a bus drivers that wish to retrieve the name of the 
                       bus controller.  It will not be NULL for a bus driver 
                       that wishes to retrieve the name of a child controller.
    Language         - A pointer to a three character ISO 639-2 language 
                       identifier.  This is the language of the controller name 
                       that that the caller is requesting, and it must match one
                       of the languages specified in SupportedLanguages.  The 
                       number of languages supported by a driver is up to the 
                       driver writer.
    ControllerName   - A pointer to the Unicode string to return.  This Unicode
                       string is the name of the controller specified by 
                       ControllerHandle and ChildHandle in the language 
                       specified by Language from the point of view of the 
                       driver specified by This. 

  Returns:
    EFI_SUCCESS           - The Unicode string for the user readable name in the
                            language specified by Language for the driver 
                            specified by This was returned in DriverName.
    EFI_INVALID_PARAMETER - ControllerHandle is not a valid EFI_HANDLE.
    EFI_INVALID_PARAMETER - ChildHandle is not NULL and it is not a valid 
                            EFI_HANDLE.
    EFI_INVALID_PARAMETER - Language is NULL.
    EFI_INVALID_PARAMETER - ControllerName is NULL.
    EFI_UNSUPPORTED       - The driver specified by This is not currently 
                            managing the controller specified by 
                            ControllerHandle and ChildHandle.
    EFI_UNSUPPORTED       - The driver specified by This does not support the 
                            language specified by Language.

--*/
{
  EFI_STATUS                    Status;
  
  //
  // Make sure this driver is currently managing ControllHandle
  //
  Status = EfiLibTestManagedDevice (
             ControllerHandle,
             gConSplitterStdErrDriverBinding.DriverBindingHandle,
             &gEfiStandardErrorDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // here ChildHandle is not an Optional parameter.
  //
  if (ChildHandle == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  Status = EfiLibTestChildHandle (
             ControllerHandle,
             ChildHandle,
             &gEfiStandardErrorDeviceGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EfiLibLookupUnicodeString (
          Language,
          gConSplitterStdErrComponentName.SupportedLanguages,
          mConSplitterStdErrControllerNameTable,
          ControllerName
          );
}
