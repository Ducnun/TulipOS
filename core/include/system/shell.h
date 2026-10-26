#pragma once

#include "engine/basic_engine.h"
#include "engine/iprogramio.h"
#include "kernel/api/istorage_service.h"
#include "kernel/api/itask_control.h"
#include "system/api/iconsole.h"
#include "utils/stringview.h"
#include <cstddef>

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

    struct Command {
        const char* name;
        const char* usage;
        const char* description;
        CommandResult (Shell::*handler)(util::StringView arguments);
    };

    IConsole& _console;
    IProgramIO& _programIO;
    IStorageService& _storage;
    ITaskControl& _taskControl;
    BasicEngine _engine;

    static const Command* commandTable(size_t& count);
    CommandResult executeCommand(util::StringView line);
    CommandResult commandExit(util::StringView arguments);
    CommandResult commandRun(util::StringView arguments);
    CommandResult commandList(util::StringView arguments);
    CommandResult commandNew(util::StringView arguments);
    CommandResult commandDump(util::StringView arguments);
    CommandResult commandCls(util::StringView arguments);
    CommandResult commandPwd(util::StringView arguments);
    CommandResult commandLs(util::StringView arguments);
    CommandResult commandLoad(util::StringView arguments);
    CommandResult commandHelp(util::StringView arguments);
    CommandResult commandCd(util::StringView arguments);
    void listDirectory();
    void loadProgram(util::StringView path);
};
