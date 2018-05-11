#ifndef _RptPciH_
#define _RptPciH_

//-------------------------------------------------------------------------------------------------
//
// Function Prototypes
//
//-------------------------------------------------------------------------------------------------
#include <Library/RptPciLib/RptPciLib.h>

//-------------------------------------------------------------------------------------------------
//
// Function Prototypes
//
//-------------------------------------------------------------------------------------------------
unsigned long MmPciAddress(unsigned long PcieBase, PCI_PHYSIC_LOC *PciPhysicLoc);
int GetPcieLinkRegs(unsigned long MmAddr, PCIE_CAP_LINK_REGS *PcieLink);
unsigned long FindPcieCapOffset(unsigned long MmAddr);

#endif

