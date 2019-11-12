#ifndef _RptDpH_
#define _RptDpH_
//-------------------------------------------------------------------------------------------------
//
// Includes
//
//-------------------------------------------------------------------------------------------------
#include <Library/RptDpLib/RptDpLib.h>

//-------------------------------------------------------------------------------------------------
//
// Function Prototypes
//
//-------------------------------------------------------------------------------------------------
UINTN DpLength(EFI_DEVICE_PATH_PROTOCOL *pDp);
VOID* DpCopy(IN EFI_BOOT_SERVICES *BootServices, EFI_DEVICE_PATH_PROTOCOL *pDp);
VOID* DpTailNode(EFI_DEVICE_PATH_PROTOCOL *pDp);
VOID* DpDiscardTailNode(IN EFI_BOOT_SERVICES *BootServices, EFI_DEVICE_PATH_PROTOCOL *pDp);

#endif
