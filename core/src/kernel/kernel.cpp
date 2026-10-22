#include "kernel/kernel.h"

Kernel::Kernel(IHost& host) : _host(host)
{}

bool Kernel::begin(){
    if (!_memory || !_taskControl || !_storage || !_input || !_output) return false;

    _host.begin();
    if (!initStorage()) return false;

    _console = std::make_unique<Console>(*_input, *_output);
    _consoleProgramIO = std::make_unique<ConsoleProgramIO>(*_console, *_output);
    _shell = std::make_unique<Shell>(*_console, *_consoleProgramIO, *_storage, *_taskControl);
    _shell->run();
    return true;
}

bool Kernel::initStorage() {
    const auto storageList = _host.getAvailableStorage();
    if (storageList.empty()) {
        _host.debugLog("No storage devices found; starting without mounted storage");
        return true;
    }

    for (auto& [prefix, device] : storageList) {
        if (!device) {
            _host.debugLog("Storage device descriptor has no block device; skipping it");
            continue;
        }
        if (!_storage->mount(prefix, *device)) {
            _host.debugLog("Storage autodetect or mount failed; continuing without that device");
        }
    }

    return true;
}