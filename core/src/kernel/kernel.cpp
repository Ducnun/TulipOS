#include "kernel/kernel.h"

Kernel::Kernel(IHost& host) : _host(host)
{}

void Kernel::begin(){
    _host.begin();
    // initStorage();

    _console->run();
}

void Kernel::initStorage() {
    auto storageList = _host.getAvailableStorage(); // Get all storage devices

    for (auto& [prefix, device] : storageList) {
        // Mount each block device to the VFS
        _storage->mount(prefix, *device);
    }
}