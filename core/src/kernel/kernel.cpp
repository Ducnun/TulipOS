#include "kernel/kernel.h"

void Kernel::boot(){
    // Mount hardware drives into the VFS
    _storage.mountHardware(_host.getAvailableStorage());
    _console.println("Kernel Booted.");

    Shell shell(*this);
    shell.runREPL();
}