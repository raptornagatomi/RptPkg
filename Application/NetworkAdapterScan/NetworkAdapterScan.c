//-------------------------------------------------------------------------------------------------
//
// Includes
//
//-------------------------------------------------------------------------------------------------
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/PxeBaseCode.h>
#include <Uefi/UefiBaseType.h>
#include <Protocol/Ip4.h>
#include <Protocol/Ip6.h>
#include <Library/PrintLib.h>
#include <Library/RptDp.h>
#include <Library/RptUefiString.h>

//-------------------------------------------------------------------------------------------------
//
// Constants
//
//-------------------------------------------------------------------------------------------------
#define ETHERNET_MAC_ADDRESS_LENGTH             6

const CHAR16* Ipv6AddressOrigin[] = {
        L"Manually configured",
        L"IPv6 stateless auto-configuration",
        L"IPv6 stateful configuration"
};

//-------------------------------------------------------------------------------------------------
//
// Function Prototypes
//
//-------------------------------------------------------------------------------------------------
EFI_STATUS NetworkDeviceScan(IN EFI_BOOT_SERVICES *BootServices);
CHAR16* GetControllerName(IN EFI_BOOT_SERVICES *BootServices, IN EFI_HANDLE Controller);
CHAR16* GetControllerNameHelper(IN EFI_BOOT_SERVICES *BootServices, IN EFI_HANDLE Controller, IN EFI_HANDLE Child, OUT BOOLEAN *ChildFound);
VOID PrintDevicePath(IN EFI_BOOT_SERVICES *BootServices, IN EFI_DEVICE_PATH_PROTOCOL *pDp);

//-------------------------------------------------------------------------------------------------
//
// Entry Point: UefiMain
//
//-------------------------------------------------------------------------------------------------
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
        EFI_STATUS      Status = EFI_SUCCESS;

        Status = NetworkDeviceScan(SystemTable->BootServices);
        if(EFI_ERROR(Status)) {
                Print(L"Error occurred as scanning Network Adapters: %r\n", Status);
        }

        return EFI_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
//
// NetworkDeviceScan
//
//-------------------------------------------------------------------------------------------------
EFI_STATUS NetworkDeviceScan(IN EFI_BOOT_SERVICES *BootServices)
{
        EFI_STATUS                              Status = EFI_SUCCESS;
        UINTN                                   Count;
        EFI_HANDLE                              *HandleBuffer;
        EFI_HANDLE                              NetworkDeviceHandle, PciDeviceHandle;
        UINTN                                   i;
        EFI_DEVICE_PATH_PROTOCOL                *DevicePath;
        EFI_DEVICE_PATH_PROTOCOL                *pDpNode;
        EFI_DEVICE_PATH_PROTOCOL                *pMac, *pIpv4, *pIpv6, *pPci;
        EFI_DEVICE_PATH_PROTOCOL                *pNetworkDev, *pPciDev, *pTmpDp;
        EFI_GUID                                EfiSimpleNetworkProtocolGuid = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
        EFI_SIMPLE_NETWORK_PROTOCOL             *Snp;
        CHAR16                                  *pControllerName;
        EFI_PCI_IO_PROTOCOL                     *PciIo;
        EFI_GUID                                PciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;

        // Locate all handles that support Device Path Protocol
        Status = BootServices->LocateHandleBuffer(ByProtocol,
                                                  &gEfiDevicePathProtocolGuid,
                                                  NULL,
                                                  &Count,
                                                  &HandleBuffer);
        if(EFI_ERROR(Status)) {
                return Status;
        }

        // Go through entire handle buffer we just extracted to find out information of network adapters
        for(i = 0; i < Count; i++) {
                Status = BootServices->HandleProtocol(HandleBuffer[i], &gEfiDevicePathProtocolGuid, (VOID *)&DevicePath);
                if(EFI_ERROR(Status))
                        continue;

                pDpNode = DevicePath;

                pMac = pIpv4 = pIpv6 = pPci = NULL;
                while(!IsDevicePathEnd(pDpNode)) {
                        if((pDpNode->Type == MESSAGING_DEVICE_PATH) && (pDpNode->SubType == MSG_MAC_ADDR_DP)) {
                                pMac = pDpNode;
                        }
                        else if((pDpNode->Type == MESSAGING_DEVICE_PATH) && (pDpNode->SubType == MSG_IPv4_DP)) {
                                pIpv4 = pDpNode;
                        }
                        else if((pDpNode->Type == MESSAGING_DEVICE_PATH) && (pDpNode->SubType == MSG_IPv6_DP)) {
                                pIpv6 = pDpNode;
                        }
                        else if((pDpNode->Type == HARDWARE_DEVICE_PATH) && (pDpNode->SubType == HW_PCI_DP)) {
                                pPci = pDpNode;
                        }

                        if(pIpv4 != NULL || pIpv6 != NULL) {
                                pNetworkDev = DpDiscardTailNode(BootServices, DevicePath);
                        }

                        pDpNode = NextDevicePathNode(pDpNode);
                }

                // Print Information
                if(pIpv4 != NULL || pIpv6 != NULL) {
                        // Device Name
                        pTmpDp = pNetworkDev;
                        Status = BootServices->LocateDevicePath(&gEfiDevicePathProtocolGuid, &pTmpDp, &NetworkDeviceHandle);
                        if(!EFI_ERROR(Status)) {
                                pControllerName = GetControllerName(BootServices, NetworkDeviceHandle);

                                if(!pControllerName)
                                        pControllerName = L"Unknown Device";

                                Print(L"%s\n", pControllerName);
                        }

                        // Device Path
                        Print(L"  ");
                        PrintDevicePath(BootServices, pNetworkDev);
                        Print(L"\n");

                        // PCI Device Information
                        if(pPci) {
                                BOOLEAN PciLocFound = FALSE;
                                if(pNetworkDev) {
                                        pPciDev = DpDiscardTailNode(BootServices, pNetworkDev);
                                        Status = BootServices->LocateDevicePath(&PciIoProtocolGuid, &pPciDev, &PciDeviceHandle);
                                        if(!EFI_ERROR(Status)) {
                                                UINTN   Seg, Bus, Dev, Fnc;

                                                Status = BootServices->HandleProtocol(PciDeviceHandle, &PciIoProtocolGuid, &PciIo);
                                                if(!EFI_ERROR(Status)) {
                                                        PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fnc);
                                                        Print(L"  PCI Location             : %02X:%02X.%02X\n", Bus, Dev, Fnc);

                                                        PciLocFound = TRUE;
                                                }
                                        }
                                }

                                if(!PciLocFound)
                                        Print(L"  PCI Location             : D%d:F%d\n", ((PCI_DEVICE_PATH*)pPci)->Device, ((PCI_DEVICE_PATH*)pPci)->Function);
                        }

                        // Simple Network Protocol
                        Status = BootServices->HandleProtocol(HandleBuffer[i], &EfiSimpleNetworkProtocolGuid, &Snp);
                        if(EFI_ERROR(Status)) {
                                Snp = NULL;
                        }

                        if(Snp) {
                                Print(L"  Current MAC Address      : %s\n", MacAddressString(BootServices, &(Snp->Mode->CurrentAddress), ETHERNET_MAC_ADDRESS_LENGTH));
                                Print(L"  Permanent MAC Address    : %s\n", MacAddressString(BootServices, &(Snp->Mode->PermanentAddress), ETHERNET_MAC_ADDRESS_LENGTH));
                                Print(L"  Media Present Supported  : %s\n", BooleanToYesNo(BootServices, Snp->Mode->MediaPresentSupported));
                                Print(L"  Media Present            : %s\n", BooleanToYesNo(BootServices, Snp->Mode->MediaPresent));
                        }

                        // IPv4
                        if(pIpv4) {
                                Print(L"  Local IP Address (IPv4)  : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv4_DEVICE_PATH*)pIpv4)->LocalIpAddress, FALSE));
                                Print(L"  Remote IP Address (IPv4) : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv4_DEVICE_PATH*)pIpv4)->RemoteIpAddress, FALSE));
                                Print(L"  Local Port               : %d\n", ((IPv4_DEVICE_PATH*)pIpv4)->LocalPort);
                                Print(L"  Remote Port              : %d\n", ((IPv4_DEVICE_PATH*)pIpv4)->RemotePort);
                                Print(L"  Static IP Address        : %s\n", BooleanToYesNo(BootServices, ((IPv4_DEVICE_PATH*)pIpv4)->StaticIpAddress));
                                Print(L"  Gateway (IPv4)           : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv4_DEVICE_PATH*)pIpv4)->GatewayIpAddress, FALSE));
                                Print(L"  Subnet Mask (IPv4)       : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv4_DEVICE_PATH*)pIpv4)->SubnetMask, FALSE));
                        }

                        // IPv6
                        if(pIpv6) {
                                Print(L"  Local IP Address (IPv6)  : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv6_DEVICE_PATH*)pIpv6)->LocalIpAddress, TRUE));
                                Print(L"  Remote IP Address (IPv6) : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv6_DEVICE_PATH*)pIpv6)->RemoteIpAddress, TRUE));
                                Print(L"  Local Port               : %d\n", ((IPv6_DEVICE_PATH*)pIpv6)->LocalPort);
                                Print(L"  Remote Port              : %d\n", ((IPv6_DEVICE_PATH*)pIpv6)->RemotePort);
                                Print(L"  IP Address Origin        : %s\n", Ipv6AddressOrigin[((IPv6_DEVICE_PATH*)pIpv6)->IpAddressOrigin]);
                                Print(L"  Prefix Length            : %d\n", ((IPv6_DEVICE_PATH*)pIpv6)->PrefixLength);
                                Print(L"  Gateway (IPv6)           : %s\n", IpAddressString(BootServices, (EFI_IP_ADDRESS*)&((IPv6_DEVICE_PATH*)pIpv6)->GatewayIpAddress, TRUE));
                        }

                        // Empty line
                        if(pMac != NULL || pIpv4 != NULL || pIpv6 != NULL) {
                                Print(L"\n");
                        }
                }
        }

        // Free memory
        if(pNetworkDev)
                BootServices->FreePool(pNetworkDev);

        return Status;
}

//-------------------------------------------------------------------------------------------------
//
// GetControllerName
//
//-------------------------------------------------------------------------------------------------
CHAR16* GetControllerName(IN EFI_BOOT_SERVICES *BootServices, IN EFI_HANDLE Controller)
{
        EFI_HANDLE                              Parent;
        EFI_DEVICE_PATH_PROTOCOL                *pCtlrDp = NULL, *pParentDp = NULL;
        BOOLEAN                                 ChildFound;
        CHAR16                                  *pName = NULL;

        //
        // Try to get the name with the controller handle just passed in directly
        //
        pName = GetControllerNameHelper(BootServices, Controller, NULL, NULL);
        if(pName)
                return pName;

        //
        // If cannot find the name with the passed in handle
        // Try its parent handles
        //

        pName = NULL;

        if(!EFI_ERROR(BootServices->HandleProtocol(Controller, &gEfiDevicePathProtocolGuid, &pCtlrDp))) {                       // Find the Device Path of the controller handle
                do {
                        ChildFound = TRUE;
                        pParentDp = DpDiscardTailNode(BootServices, pCtlrDp);                                                   // Find the Device Path of Parent Handle

                        if(!EFI_ERROR(BootServices->LocateDevicePath(&gEfiDevicePathProtocolGuid, &pParentDp, &Parent))) {      // Find Parent Handle
                                pName = GetControllerNameHelper(BootServices, Parent, Controller, &ChildFound);
                        }
                        else {
                                pName = NULL;
                                break;
                        }
                } while(!ChildFound);
        }

        // Free memory
        if(pParentDp)
                BootServices->FreePool(pParentDp);

        return pName;
}

//-------------------------------------------------------------------------------------------------
//
// GetControllerNameHelper
//
//-------------------------------------------------------------------------------------------------
CHAR16* GetControllerNameHelper(IN EFI_BOOT_SERVICES *BootServices, IN EFI_HANDLE Controller, IN EFI_HANDLE Child, OUT BOOLEAN *ChildFound)
{
        UINT32                                  Attributes = Child ? EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER : EFI_OPEN_PROTOCOL_BY_DRIVER;
        UINTN                                   i, NumOfProtocols;
        EFI_GUID                                **ppProtocols;
        UINTN                                   j, NumOfProtocolInfo;
        EFI_GUID                                ComponentName2ProtocolGuid = EFI_COMPONENT_NAME2_PROTOCOL_GUID;
        EFI_GUID                                ComponentNameProtocolGuid = EFI_COMPONENT_NAME_PROTOCOL_GUID;
        EFI_OPEN_PROTOCOL_INFORMATION_ENTRY     *pProtocolInfo;
        EFI_COMPONENT_NAME_PROTOCOL             *pComponentNameProtocol;
        CHAR16                                  *pName = NULL;

        // Get all protocols installed on the handle
        if(EFI_ERROR(BootServices->ProtocolsPerHandle(Controller, &ppProtocols, &NumOfProtocols)))
                return NULL;

        if(ChildFound)
                *ChildFound = FALSE;

        for(i = 0; i < NumOfProtocols; i++) {
                // Retrieves the list of agents that currently have a protocol interface opened
                if(EFI_ERROR(BootServices->OpenProtocolInformation(Controller, ppProtocols[i], &pProtocolInfo, &NumOfProtocolInfo)))
                        continue;

                for(j = 0; j < NumOfProtocolInfo; j++) {
                        if(pProtocolInfo[j].Attributes != Attributes)
                                continue;

                        if(Child) {
                                if(pProtocolInfo[j].ControllerHandle != Child)
                                        continue;

                                if(ChildFound)
                                        *ChildFound = TRUE;
                        }

                        //
                        // Get Controller Name
                        //
                        if(!EFI_ERROR(BootServices->HandleProtocol(pProtocolInfo[j].AgentHandle, &ComponentName2ProtocolGuid, &pComponentNameProtocol))) {
                                if(!EFI_ERROR(pComponentNameProtocol->GetControllerName(pComponentNameProtocol, Controller, Child, "en-US", &pName))) {
                                        break;          // if controller name was got successfully, break the loop
                                }
                                else if(!EFI_ERROR(pComponentNameProtocol->GetControllerName(pComponentNameProtocol, Controller, Child, "en", &pName))) {
                                        break;          // if controller name was got successfully, break the loop
                                }
                        }
                        else if(!EFI_ERROR(BootServices->HandleProtocol(pProtocolInfo[j].AgentHandle, &ComponentNameProtocolGuid, &pComponentNameProtocol))){
                                if(!EFI_ERROR(pComponentNameProtocol->GetControllerName(pComponentNameProtocol, Controller, Child, "eng", &pName))) {
                                        break;          // if controller name was got successfully, break the loop
                                }
                        }
                }

                // Free memory
                if(NumOfProtocolInfo)
                        BootServices->FreePool(pProtocolInfo);
        }

        // Free memory
        if(NumOfProtocols)
                BootServices->FreePool(ppProtocols);

        return pName;
}

//-------------------------------------------------------------------------------------------------
//
// PrintDevicePath
//
//-------------------------------------------------------------------------------------------------
VOID PrintDevicePath(IN EFI_BOOT_SERVICES *BootServices, IN EFI_DEVICE_PATH_PROTOCOL *pDp)
{
        EFI_STATUS                              Status = EFI_SUCCESS;
        EFI_GUID                                EfiDevicePathToTextProtocolGuid = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;
        EFI_DEVICE_PATH_TO_TEXT_PROTOCOL        *DpToText = NULL;
        EFI_DEVICE_PATH_PROTOCOL                *pDpNode = NULL;

        Status = BootServices->LocateProtocol(&EfiDevicePathToTextProtocolGuid, NULL, &DpToText);
        if(EFI_ERROR(Status)) {
                return;
        }

        pDpNode = pDp;
        while(!IsDevicePathEnd(pDpNode)) {
                Print(L"%s", DpToText->ConvertDeviceNodeToText(pDpNode, TRUE, FALSE));

                pDpNode = NextDevicePathNode(pDpNode);

                if(!IsDevicePathEnd(pDpNode))
                        Print(L"/");
        }
}
