//-------------------------------------------------------------------------------------------------
//
// Includes
//
//-------------------------------------------------------------------------------------------------
#include "RptDpLib.h"
#include <Library/RptDp.h>

//-------------------------------------------------------------------------------------------------
//
// DpLength
//
//-------------------------------------------------------------------------------------------------
UINTN DpLength(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
        UINTN   Size, Length;

        if(!pDp)
                return 0;

        Size = 0;

        while(!(IsEndDpNode(pDp) && pDp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)) {
                Length = LengthOfDpNode(pDp);

                if((pDp->Type == 0) || (Length == 0))
                        return Size;

                Size += Length;

                pDp = NextDpNode(pDp);
        }

        return Size + sizeof(EFI_DEVICE_PATH_PROTOCOL);         // add size of END_DEVICE_PATH node
}

//-------------------------------------------------------------------------------------------------
//
// DpCopy
//
//-------------------------------------------------------------------------------------------------
VOID* DpCopy(IN EFI_BOOT_SERVICES *BootServices, EFI_DEVICE_PATH_PROTOCOL *pDp)
{
        UINTN   Length;
        UINT8   *pNewDp;

        if(!pDp)
                return NULL;

        Length = DpLength(pDp);

        BootServices->AllocatePool(EfiBootServicesData, Length, &pNewDp);
        BootServices->CopyMem(pNewDp, pDp, Length);

        return pNewDp;
}

//-------------------------------------------------------------------------------------------------
//
// DpTailNode
//
//-------------------------------------------------------------------------------------------------
VOID* DpTailNode(EFI_DEVICE_PATH_PROTOCOL *pDp)
{
        EFI_DEVICE_PATH_PROTOCOL *pTailNode = NULL;

        if(!pDp) {
                pTailNode = NULL;
        }
        else {
                while(!IsEndDpNode(pDp)) {
                        pTailNode = pDp;
                        pDp = NextDpNode(pDp);
                }
        }

        return pTailNode;
}

//-------------------------------------------------------------------------------------------------
//
// DpDiscardTailNode
//
//-------------------------------------------------------------------------------------------------
VOID* DpDiscardTailNode(IN EFI_BOOT_SERVICES *BootServices, EFI_DEVICE_PATH_PROTOCOL *pDp)
{
        EFI_DEVICE_PATH_PROTOCOL        *pTmp, *pNewDp, *pTailNode;
        UINTN                           Length;

        if(!pDp)
                return NULL;

        // Calculate new Device Path length
        pTmp = DpTailNode(pDp);
        Length = (UINTN)pTmp - (UINTN)pDp;

        if(Length == 0)
                return NULL;

        // Allocate memory
        BootServices->AllocatePool(EfiBootServicesData, Length + sizeof(EFI_DEVICE_PATH_PROTOCOL), &pNewDp);
        BootServices->CopyMem(pNewDp, pDp, Length);

        // Set new End Node
        pTailNode = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)pNewDp + Length);
        pTailNode->Type = END_DEVICE_PATH_TYPE;
        pTailNode->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
        UpdateLengthOfDpNode(pTailNode, END_DEVICE_PATH_LENGTH);

        return pNewDp;
}
