//-------------------------------------------------------------------------------------------------
//
// HexStringToUInt
//
//-------------------------------------------------------------------------------------------------
#include "RptUefiStringLib.h"
#include <Library/RptUefiString.h>

//-------------------------------------------------------------------------------------------------
//
// MacAddressString
//
//-------------------------------------------------------------------------------------------------
CHAR16* MacAddressString(IN EFI_BOOT_SERVICES *BootServices, IN EFI_MAC_ADDRESS *MacAddr, IN UINTN Length)
{
        EFI_STATUS      Status = EFI_SUCCESS;
        UINTN           i = 0;
        CHAR16          *OutputString = NULL, *TempString = NULL;
        UINTN           BufSize = sizeof(CHAR16) * (Length * 2 + (Length * 2 - 1) + 1);

        Status = BootServices->AllocatePool(EfiBootServicesData, BufSize, (VOID**)&OutputString);
        if(EFI_ERROR(Status)) {
                OutputString = NULL;
                return OutputString;
        }

        Status = BootServices->AllocatePool(EfiBootServicesData, BufSize, (VOID**)&TempString);
        if(EFI_ERROR(Status)) {
                OutputString = NULL;
                return OutputString;
        }

        TempString[0] = L'\0';
        for(i = 0; i < Length; i++) {
                UnicodeSPrint(OutputString, BufSize, L"%s%02X", TempString, MacAddr->Addr[i]);

                if(i < Length - 1)
                         UnicodeSPrint(OutputString, BufSize, L"%s:", OutputString);

                StrCpy(TempString, OutputString);
        }

        BootServices->FreePool(TempString);

        return OutputString;
}

//-------------------------------------------------------------------------------------------------
//
// IpAddressString
//
//-------------------------------------------------------------------------------------------------
CHAR16* IpAddressString(IN EFI_BOOT_SERVICES *BootServices, IN EFI_IP_ADDRESS *IpAddr, IN BOOLEAN Ipv6)
{
        EFI_STATUS      Status = EFI_SUCCESS;
        UINTN           i = 0;
        UINTN           Length = 0;
        CHAR16          *OutputString = NULL, *TempString = NULL;
        UINTN           BufSize;

        Length = Ipv6 ? 16 : 4;
        BufSize = (Length + (Length -1 ) + 1) * sizeof(CHAR16);

        Status = BootServices->AllocatePool(EfiBootServicesData, BufSize, (VOID**)&OutputString);
        if(EFI_ERROR(Status)) {
                OutputString = NULL;
                return OutputString;
        }

        Status = BootServices->AllocatePool(EfiBootServicesData, BufSize, (VOID**)&TempString);
        if(EFI_ERROR(Status)) {
                OutputString = NULL;
                return OutputString;
        }

        if(Ipv6) {
                TempString[0] = L'\0';
                for(i = 0; i < Length; i++) {
                        UnicodeSPrint(OutputString, BufSize, L"%s%02X%02X", TempString, IpAddr->v6.Addr[i + 1], IpAddr->v6.Addr[i + 0]);

                        if(i < Length - 2)
                                 UnicodeSPrint(OutputString, BufSize, L"%s:", OutputString);

                        StrCpy(TempString, OutputString);
                }
        }
        else {
                TempString[0] = L'\0';
                for(i = 0; i < Length; i++) {
                        UnicodeSPrint(OutputString, BufSize, L"%s%d", TempString, IpAddr->v4.Addr[i]);

                        if(i < Length - 1)
                                UnicodeSPrint(OutputString, BufSize, L"%s.", OutputString);

                        StrCpy(TempString, OutputString);
                }
        }

        BootServices->FreePool(TempString);

        return OutputString;
}

//-------------------------------------------------------------------------------------------------
//
// BooleanToYesNo
//
//-------------------------------------------------------------------------------------------------
CHAR16* BooleanToYesNo(IN EFI_BOOT_SERVICES *BootServices, IN BOOLEAN Boolean)
{
        EFI_STATUS      Status = EFI_SUCCESS;
        CHAR16          *YesNo = NULL;

        Status = BootServices->AllocatePool(EfiBootServicesData, sizeof(CHAR16) * 4, (VOID**)&YesNo);
        if(EFI_ERROR(Status)) {
                YesNo = NULL;
        }
        else {
                if(Boolean)
                        StrCpy(YesNo, L"Yes");
                else
                        StrCpy(YesNo, L"No");
        }

        return YesNo;
}
