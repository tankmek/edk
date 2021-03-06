/*++

Copyright (c) 2004 - 2009, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Ui.h

Abstract:

  Head file UI

Revision History

--*/

#ifndef _UI_H
#define _UI_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "Setup.h"
#include "GraphicsLib.h"
#include "EfiPrintLib.h"

//
// Globals
//
#define REGULAR_NUMERIC 0
#define TIME_NUMERIC    1
#define DATE_NUMERIC    2

#define SUBTITLE_INDENT  2

typedef enum {
  UiNoOperation,
  UiDefault,
  UiSelect,
  UiUp,
  UiDown,
  UiLeft,
  UiRight,
  UiReset,
  UiSave,
  UiPrevious,
  UiPageUp,
  UiPageDown,
  UiMaxOperation
} UI_SCREEN_OPERATION;

typedef enum {
  CfInitialization,
  CfCheckSelection,
  CfRepaint,
  CfRefreshHighLight,
  CfUpdateHelpString,
  CfPrepareToReadKey,
  CfReadKey,
  CfScreenOperation,
  CfUiSelect,
  CfUiReset,
  CfUiLeft,
  CfUiRight,
  CfUiUp,
  CfUiPageUp,
  CfUiPageDown,
  CfUiDown,
  CfUiSave,
  CfUiDefault,
  CfUiNoOperation,
  CfExit,
  CfMaxControlFlag
} UI_CONTROL_FLAG;

#define UI_ACTION_NONE               0
#define UI_ACTION_REFRESH_FORM       1
#define UI_ACTION_REFRESH_FORMSET    2
#define UI_ACTION_EXIT               3

typedef struct {
  EFI_HII_HANDLE  Handle;

  //
  // Target formset/form/Question information
  //
  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;

  UINTN           TopRow;
  UINTN           BottomRow;
  UINTN           PromptCol;
  UINTN           OptionCol;
  UINTN           CurrentRow;

  //
  // Ation for Browser to taken:
  //   UI_ACTION_NONE            - navigation inside a form
  //   UI_ACTION_REFRESH_FORM    - re-evaluate expressions and repaint form
  //   UI_ACTION_REFRESH_FORMSET - re-parse formset IFR binary
  //
  UINTN           Action;

  //
  // Current selected fomset/form/Question
  //
  FORM_BROWSER_FORMSET    *FormSet;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_STATEMENT  *Statement;

  //
  // Whether the Form is editable
  //
  BOOLEAN                 FormEditable;
} UI_MENU_SELECTION;

#define UI_MENU_OPTION_SIGNATURE  EFI_SIGNATURE_32 ('u', 'i', 'm', 'm')
#define UI_MENU_LIST_SIGNATURE    EFI_SIGNATURE_32 ('u', 'i', 'm', 'l')

typedef struct {
  UINTN                   Signature;
  EFI_LIST_ENTRY          Link;

  EFI_HII_HANDLE          Handle;
  FORM_BROWSER_STATEMENT  *ThisTag;
  UINT16                  EntryNumber;

  UINTN                   Row;
  UINTN                   Col;
  UINTN                   OptCol;
  CHAR16                  *Description;
  UINTN                   Skip;           // Number of lines

  //
  // Display item sequence for date/time
  //  Date:      Month/Day/Year
  //  Sequence:  0     1   2
  //
  //  Time:      Hour : Minute : Second
  //  Sequence:  0      1        2
  //
  //
  UINTN                   Sequence;

  BOOLEAN                 GrayOut;
  BOOLEAN                 ReadOnly;

  //
  // Whether user could change value of this item
  //
  BOOLEAN                 IsQuestion;
} UI_MENU_OPTION;

#define MENU_OPTION_FROM_LINK(a)  CR (a, UI_MENU_OPTION, Link, UI_MENU_OPTION_SIGNATURE)

typedef struct _UI_MENU_LIST UI_MENU_LIST;

struct _UI_MENU_LIST {
  UINTN           Signature;
  EFI_LIST_ENTRY  Link;

  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;

  UI_MENU_LIST    *Parent;
  EFI_LIST_ENTRY  ChildListHead;
};

#define UI_MENU_LIST_FROM_LINK(a)  CR (a, UI_MENU_LIST, Link, UI_MENU_LIST_SIGNATURE)

typedef struct _MENU_REFRESH_ENTRY {
  struct _MENU_REFRESH_ENTRY  *Next;
  UI_MENU_OPTION              *MenuOption;  // Describes the entry needing an update
  UI_MENU_SELECTION           *Selection;
  UINTN                       CurrentColumn;
  UINTN                       CurrentRow;
  UINTN                       CurrentAttribute;
} MENU_REFRESH_ENTRY;

typedef struct {
  UINT16              ScanCode;
  UI_SCREEN_OPERATION ScreenOperation;
} SCAN_CODE_TO_SCREEN_OPERATION;

typedef struct {
  UI_SCREEN_OPERATION ScreenOperation;
  UI_CONTROL_FLAG     ControlFlag;
} SCREEN_OPERATION_T0_CONTROL_FLAG;


extern EFI_LIST_ENTRY      Menu;
extern MENU_REFRESH_ENTRY  *gMenuRefreshHead;
extern UI_MENU_SELECTION   *gCurrentSelection;
extern BOOLEAN             mHiiPackageListUpdated;

//
// Global Functions
//
VOID
UiInitMenu (
  VOID
  )
;

VOID
UiFreeMenu (
  VOID
  )
;

UI_MENU_LIST *
UiAddMenuList (
  IN OUT UI_MENU_LIST     *Parent,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  )
;

UI_MENU_LIST *
UiFindChildMenuList (
  IN UI_MENU_LIST         *Parent,
  IN UINT16               FormId
  )
;

UI_MENU_LIST *
UiFindMenuList (
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  )
;

UI_MENU_OPTION *
UiAddMenuOption (
  IN CHAR16                  *String,
  IN EFI_HII_HANDLE          Handle,
  IN FORM_BROWSER_STATEMENT  *Statement,
  IN UINT16                  NumberOfLines,
  IN UINT16                  MenuItemCount
  )
;

EFI_STATUS
UiDisplayMenu (
  IN OUT UI_MENU_SELECTION           *Selection
  )
;

VOID
FreeBrowserStrings (
  VOID
  )
;

EFI_STATUS
SetupBrowser (
  IN OUT UI_MENU_SELECTION    *Selection
  )
;

VOID
ValueToString (
  IN CHAR16   *Buffer,
  IN BOOLEAN  Comma,
  IN INT64    v
  )
;

EFI_STATUS
UiIntToString (
  IN UINTN      num,
  IN OUT CHAR16 *str,
  IN UINT16     size
  )
;

void
StrnCpy (
  IN CHAR16   *Dest,
  IN CHAR16   *Src,
  IN UINTN    Length
  )
;

VOID
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
;

EFI_STATUS
UiWaitForSingleEvent (
  IN EFI_EVENT                Event,
  IN UINT64                   Timeout, OPTIONAL
  IN UINT8                    RefreshInterval OPTIONAL
  )
;

VOID
CreatePopUp (
  IN  UINTN                       ScreenWidth,
  IN  UINTN                       NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  )
;

EFI_STATUS
ReadString (
  IN  UI_MENU_OPTION              *MenuOption,
  IN  CHAR16                      *Prompt,
  OUT CHAR16                      *StringPtr
  )
;

EFI_STATUS
GetSelectionInputPopUp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  )
;

EFI_STATUS
GetNumericInput (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption
  )
;

VOID
UpdateStatusBar (
  IN  UINTN                       MessageType,
  IN  UINT8                       Flags,
  IN  BOOLEAN                     State
  )
;

EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  )
;

EFI_STATUS
PrintFormattedNumber (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN OUT CHAR16               *FormattedNumber,
  IN UINTN                    BufferSize
  )
;

QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  )
;

UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  )
;

VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index,
  IN UINT64                   Value
  )
;

EFI_STATUS
ProcessOptions (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected,
  OUT CHAR16                      **OptionString
  )
;

VOID
ProcessHelpString (
  IN  CHAR16                      *StringPtr,
  OUT CHAR16                      **FormattedString,
  IN  UINTN                       RowCount
  )
;

VOID
UpdateKeyHelp (
  IN  UI_MENU_SELECTION           *Selection,
  IN  UI_MENU_OPTION              *MenuOption,
  IN  BOOLEAN                     Selected
  )
;

VOID
ClearLines (
  UINTN                                       LeftColumn,
  UINTN                                       RightColumn,
  UINTN                                       TopRow,
  UINTN                                       BottomRow,
  UINTN                                       TextAttribute
  )
;

UINTN
GetStringWidth (
  CHAR16                                      *String
  )
;

UINT16
GetLineByWidth (
  IN      CHAR16                      *InputString,
  IN      UINT16                      LineWidth,
  IN OUT  UINTN                       *Index,
  OUT     CHAR16                      **OutputString
  )
;

UINT16
GetWidth (
  IN FORM_BROWSER_STATEMENT        *Statement,
  IN EFI_HII_HANDLE                 Handle
  )
;

VOID
NewStrCat (
  CHAR16                                      *Destination,
  CHAR16                                      *Source
  )
;

EFI_STATUS
WaitForKeyStroke (
  OUT  EFI_INPUT_KEY           *Key
  )
;

VOID
ResetScopeStack (
  VOID
  )
;

EFI_STATUS
PushScope (
  IN UINT8   Operand
  )
;

EFI_STATUS
PopScope (
  OUT UINT8     *Operand
  )
;

FORM_BROWSER_FORM *
IdToForm (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT16                FormId
)
;

FORM_BROWSER_STATEMENT *
IdToQuestion (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN UINT16                QuestionId
  )
;

FORM_EXPRESSION *
IdToExpression (
  IN FORM_BROWSER_FORM  *Form,
  IN UINT8              RuleId
  )
;

VOID
ExtendValueToU64 (
  IN  EFI_HII_VALUE   *Value
  )
;

INTN
CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  )
;

EFI_STATUS
EvaluateExpression (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN OUT FORM_EXPRESSION   *Expression
  )
;

#endif // _UI_H
