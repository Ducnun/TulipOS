#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "tokenizer.h"
#include "parser.h"
#include "executor.h"


class Program{
public:
    /**
     * @brief Construct a new Program object
     * Initializes program counter as 0
     */
    Program(): _programCounter(0) {}

    // Program management

    /**
     * @brief Adds a program line storing both the raw string as well as parsing the tokens directly
     * @param lineNumber Line at which to insert the new program line
     * @param source Raw input from the terminal
     */
    void insertLine(int lineNumber, const std::string& source);

    /**
     * @brief Deletes a line from the program
     * @param lineNumber Line which will be removed from the program
     */
    void deleteLine(int lineNumber);

    /**
     * @brief Clears the entire program and resets the program counter
     */
    void clear();

    /**
     * @brief Get the raw lines, before any cleaning/parsing
     * @return std::vector<std::string> Vector that holds the raw input, including numbering, in order from first to last in program order
     */
    std::vector<std::string> getSource();
    
    // Flow

    /**
     * @brief Resets program counter
     */
    void resetProgramCounter();

    /**
     * @brief Get the next statement based on program counter
     * @return ParsedStatement Is either a valid statement or a statement with statementtype end if the line does not exist or is an explicit end line
     */
    ParsedStatement getNextStatement();

    /**
     * @brief Moves the program counter to the specified line
     * @param target Target line to jump to
     */
    void jumpTo(int target);

    /**
     * @brief Get the current value of the program counter
     * @return int Current counter value
     */
    int getProgramCounter() {return _programCounter;}

    // Storage
    void saveToSD(const std::string& filename);
    void loadFromSD(const std::string& filename);

private:
    std::map<int, ParsedStatement> _statements; ///< Holds the parsed statements
    std::map<int, std::string> _sourceLines; ///< Holds the raw input lines
    std::vector<int> _lineNumbers; ///< Stores the currently available line numbers
    int _programCounter;    ///< Tracks the program counter

    /**
     * @brief Private helper function that rebuilds the line vector when a new line is inserted. Helps track valid numbers
     */
    void rebuildLineNumbers();
};
