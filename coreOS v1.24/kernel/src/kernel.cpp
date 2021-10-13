#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

extern "C" void _start(BootInfo* bootInfo){

    

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    

    GlobalRenderer->Next();
    GlobalRenderer->DrawWindow("Welcome to coreOS!", "Thankyou for installing coreOS", "Written by Edward Willetts", "Click OK to Begin...", 1, 0, 0);

    

    while(true){
        asm ("hlt");
    }

}