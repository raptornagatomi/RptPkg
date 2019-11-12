#ifndef _RptDpLibH_
#define _RptDpLibH_
//-------------------------------------------------------------------------------------------------
//
// Includes
//
//-------------------------------------------------------------------------------------------------
#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePathToText.h>

//-------------------------------------------------------------------------------------------------
//
// Macros
//
//-------------------------------------------------------------------------------------------------
#define IsEndDpNode(pDp)                                ((pDp)->Type == END_DEVICE_PATH_TYPE)
#define LengthOfDpNode(pPath)                           ((pPath)->Length[0] | ((pPath)->Length[1] << 8))
#define NextDpNode(pPath)                               ((EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)(pPath) + LengthOfDpNode(pPath)))
#define UpdateLengthOfDpNode(pPath, NodeLength)         ((pPath)->Length[0] = (UINT8)(NodeLength), (pPath)->Length[1] = (NodeLength) >> 8)

#endif
