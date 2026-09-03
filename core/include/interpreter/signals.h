#pragma once

struct GotoSignal {
    int targetLine;
    GotoSignal(int line) : targetLine(line) {}
};

struct EndSignal {};

struct ReturnSignal {};