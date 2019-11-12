#ifndef _RptUefiStringH_
#define _RptUefiStringH_
//-------------------------------------------------------------------------------------------------
//
// Includes
//
//-------------------------------------------------------------------------------------------------
#include <Library/RptUefiStringLib/RptUefiStringLib.h>

//-------------------------------------------------------------------------------------------------
//
// Function Prototypes
//
//-------------------------------------------------------------------------------------------------
CHAR16* MacAddressString(IN EFI_BOOT_SERVICES *BootServices, IN EFI_MAC_ADDRESS *MacAddr, IN UINTN Length);
CHAR16* IpAddressString(IN EFI_BOOT_SERVICES *BootServices, IN EFI_IP_ADDRESS *IpAddr, IN BOOLEAN Ipv6);
CHAR16* BooleanToYesNo(IN EFI_BOOT_SERVICES *BootServices, IN BOOLEAN Boolean);

#endif
