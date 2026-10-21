#pragma once

#include "engine/basic_engine.h"
#include "engine/iprogramio.h"
#include "kernel/api/istorage_service.h"
#include "kernel/api/itask_control.h"
#include "system/api/iconsole.h"
#include "utils/stringview.h"

class Shell {
public:
    Shell(IConsole& console, IProgramIO& programIO, IStorageService& storage,
          ITaskControl& taskControl)
        : _console(console),
          _programIO(programIO),
          _storage(storage),
          _taskControl(taskControl),
          _engine(programIO, taskControl) {}

    void run();

private:
    enum class CommandResult {
        NotHandled,
        Handled,
        Exit,
    };

    IConsole& _console;
    IProgramIO& _programIO;
    IStorageService& _storage;
    ITaskControl& _taskControl;
    BasicEngine _engine;

    CommandResult executeCommand(util::StringView line);
    void listDirectory();
};
