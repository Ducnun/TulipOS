#include "system/console.h"

void Console::replaceLine(std::string& line, const std::string& replacement) {
    for (size_t i = 0; i < line.size(); ++i) {
        _output.print('\b');
    }
    line = replacement;
    _output.print(util::StringView(line));
}

bool Console::readLine(util::StringView prompt, char* buffer, size_t capacity) {
    if (!buffer || capacity == 0) return false;

    buffer[0] = '\0';
    _output.print(prompt);
    std::string line;
    std::string savedLine;
    size_t historyIndex = _history.size();

    while (true) {
        KeyEvent event;
        _input.waitForKeyEvent(event);

        if (event.action == KeyAction::Pause) {
            _output.print('\n');
            return false;
        }

        if (event.action == KeyAction::ArrowUp) {
            if (!_history.empty() && historyIndex > 0) {
                if (historyIndex == _history.size()) savedLine = line;
                --historyIndex;
                replaceLine(line, _history[historyIndex]);
            }
            continue;
        }

        if (event.action == KeyAction::ArrowDown) {
            if (historyIndex < _history.size()) {
                ++historyIndex;
                replaceLine(line, historyIndex == _history.size()
                    ? savedLine
                    : _history[historyIndex]);
            }
            continue;
        }

        if (event.action != KeyAction::Char) continue;

        const char character = event.character;
        if (character == '\n' || character == '\r') {
            _output.print('\n');
            const size_t copied = line.size() < capacity - 1 ? line.size() : capacity - 1;
            for (size_t i = 0; i < copied; ++i) buffer[i] = line[i];
            buffer[copied] = '\0';
            if (!line.empty() && (_history.empty() || _history.back() != line)) {
                if (_history.size() == HISTORY_LIMIT) _history.erase(_history.begin());
                _history.push_back(line);
            }
            return true;
        }

        if (character == '\b' || character == 0x7f) {
            if (!line.empty()) {
                line.pop_back();
                _output.print('\b');
            }
            historyIndex = _history.size();
            continue;
        }

        if ((character >= ' ' || character == '\t') && line.size() < capacity - 1) {
            line += character;
            _output.print(character);
            historyIndex = _history.size();
        }
    }
}