# EDK II Package: RptPkg
## Description
This an EDK II package used to collect all my UEFI Shell application together. <br>
Currently, below applications are contained in this package,
* Hello
  > A sample "Hello World" program.

* PcieLinkDump
  > A small utility to dump current status of PCIe Link of target device.
  > This is an UEFI version of my another DOS utility with the same program name.
  > The DOS version could be found on github,
  >
  > **Repository URL**: https://github.com/raptornagatomi/PcieLinkDump

## Built Environment
The built environment is established with edk2-vUDK2018 which could be got from github of UDK2018.

**Respository URL**: https://github.com/tianocore/tianocore.github.io/wiki/UDK2018

## How to Build
I adopt Visual Studio 2015 as my TOOL_CHAIN, therefore the step 1 below may be different if you adopt another TOOL_CHAIN.
1. Run "Developer Command Prompt for VS2015"
2. Change directory to the workspace directory of UDK2018
3. Run batch file "edksetup.bat" (without any parameter)
   > edksetup
4. Clean previous built if necessary
   > build cleanall
5. Build RptPkg
   > build -t VS2015x86 -p RptPkg\RptPkg.dsc
6. The output directory is ***edk2-vUDK2018\Build\RptPkg\DEBUG_VS2015x86\X64\RptPkg***
