/*
 This file is part of Zagreus.

 Zagreus is a UCI chess engine
 Copyright (C) 2023-2025  Danny Jelsma

 Zagreus is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published
 by the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Zagreus is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with Zagreus.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "uci.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include "bitboard.h"
#include "board.h"
#include "eval_features.h"
#include "magics.h"
#include "move.h"
#include "perft.h"
#include "pst.h"
#include "search.h"
#include "tt.h"
#include "types.h"

namespace Zagreus {
constexpr std::string_view startPosFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void Engine::doSetup() {
    // According to the UCI specification, bitboard, magic bitboards and other stuff should be done only when "isready" or "setoption" is called
    if (didSetup) {
        return;
    }

    didSetup = true;
    initializeSearch();
    initializeMagicBitboards();
    initializeBetweenLookupTable();
    initializeAttackLookupTables();
    initializePst();

    UCIOption hashOption = getOption("Hash");
    TranspositionTable::getTT()->setTableSize(std::stoi(hashOption.getValue()));
}

std::string Engine::getVersionString() {
    const std::string majorVersion = ZAGREUS_VERSION_MAJOR;
    const std::string minorVersion = ZAGREUS_VERSION_MINOR;
    std::string versionString = "v" + majorVersion + "." + minorVersion;

    if (majorVersion == "dev") {
        versionString = majorVersion + "-" + minorVersion;
    }

    return versionString;
}

void Engine::printStartupMessage() {
    sendMessage(R"(
 ______
 |___  /
    / /  __ _   __ _  _ __  ___  _   _  ___
   / /  / _` | / _` || '__|/ _ \| | | |/ __|
  / /__| (_| || (_| || |  |  __/| |_| |\__ \
 /_____|\__,_| \__, ||_|   \___| \__,_||___/
                __/ |
               |___/

Zagreus  Copyright (C) 2023-2025  Danny Jelsma

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under the conditions of the GNU Affero General Public License v3.0 or later.
You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.)");
    sendMessage("");

    sendMessage(
        "Zagreus UCI chess engine " + getVersionString() + " by Danny Jelsma (https://github.com/Dannyj1/Zagreus)");
    sendMessage("");
}

void Engine::handleUciCommand() {
    sendMessage("id name Zagreus " + getVersionString());
    sendMessage("id author Danny Jelsma");

    if (!this->options.empty()) {
        for (auto& [name, option] : this->options) {
            sendMessage(option.toString());
        }
    }

    sendMessage("uciok");
}

void Engine::handleDebugCommand(std::string_view args) {
    sendMessage("Debug mode is currently not implemented.");
}

void Engine::handleIsReadyCommand(std::string_view args) {
    if (!didSetup) {
        doSetup();
    }

    sendMessage("readyok");
}

void Engine::handleSetOptionCommand(const std::string& args) {
    std::istringstream iss(args);
    std::string arg;
    std::string section;
    std::string name;
    std::string value;

    while (iss >> arg) {
        std::string lowercaseWord = arg;
        std::ranges::transform(lowercaseWord, lowercaseWord.begin(),
                               [](const unsigned char c) { return std::tolower(c); });

        if (lowercaseWord == "name") {
            section = arg;
            continue;
        }

        if (lowercaseWord == "value") {
            section = arg;
            continue;
        }

        if (section == "name") {
            if (name.empty()) {
                name = arg;
            } else {
                name += " " + arg;
            }
        }

        if (section == "value") {
            if (value.empty()) {
                value = arg;
            } else {
                value += " " + arg;
            }
        }
    }

    if (name.empty()) {
        sendMessage("ERROR: No option name provided.");
        return;
    }

    if (!hasOption(name)) {
        sendMessage("ERROR: Option " + name + " does not exist.");
        return;
    }

    UCIOption& option = this->getOption(name);

    if (value.empty()) {
        if (option.getOptionType() == Button) {
            if (option.getValue() == "true") {
                value = "false";
            } else {
                value = "true";
            }
        } else {
            sendMessage("ERROR: No option value provided.");
            return;
        }
    }

    option.setValue(value);

    if (!didSetup) {
        doSetup();
    } else if (name == "Hash") {
        TranspositionTable::getTT()->setTableSize(std::stoi(value));
    }
}

void Engine::handleUciNewGameCommand() {
    board.reset();
    TranspositionTable::getTT()->reset();
}

void Engine::handlePositionCommand(const std::string_view args) {
    // If the first arg is not "startpos" or "fen", report invalid usage
    if (!args.starts_with("startpos") && !args.starts_with("fen")) {
        sendMessage("ERROR: Invalid usage of position command.");
        return;
    }

    // Go through each argument one by one
    std::istringstream iss((args.data()));
    std::string arg;

    // If the first argument is "startpos", set the board to the starting position
    iss >> arg;

    if (arg == "startpos") {
        board.setFromFEN(startPosFEN);
    } else if (arg == "fen") {
        std::string fen;

        // Append args to the fen string until we reach the "moves" keyword or the end of the string
        while (iss >> arg && arg != "moves") {
            if (fen.empty()) {
                fen = arg;
            } else {
                fen += " " + arg;
            }
        }

        if (fen.empty()) {
            sendMessage("ERROR: No FEN string provided.");
            return;
        }

        if (!board.setFromFEN(fen)) {
            sendMessage("ERROR: Invalid FEN string provided.");
            return;
        }
    }

    while (iss >> arg) {
        if (arg == "moves") {
            continue;
        }

        if (arg.size() != 4 && arg.size() != 5) {
            sendMessage("ERROR: Invalid move notation provided.");
            return;
        }

        Move move = getMoveFromMoveNotation(arg);

        // TODO: Move this to the getMoveFromMoveNotation function, need to work around circular dependency first
        Piece movedPiece = board.getPieceOnSquare(getFromSquare(move));

        if (movedPiece == WHITE_KING) {
            if (getFromSquare(move) == E1 && getToSquare(move) == G1 && board.canCastle<WHITE_KINGSIDE>()) {
                move = encodeMove(E1, G1, CASTLING);
            } else if (getFromSquare(move) == E1 && getToSquare(move) == C1 && board.canCastle<WHITE_QUEENSIDE>()) {
                move = encodeMove(E1, C1, CASTLING);
            }
        } else if (movedPiece == BLACK_KING) {
            if (getFromSquare(move) == E8 && getToSquare(move) == G8 && board.canCastle<BLACK_KINGSIDE>()) {
                move = encodeMove(E8, G8, CASTLING);
            } else if (getFromSquare(move) == E8 && getToSquare(move) == C8 && board.canCastle<BLACK_QUEENSIDE>()) {
                move = encodeMove(E8, C8, CASTLING);
            }
        } else if (movedPiece == WHITE_PAWN || movedPiece == BLACK_PAWN) {
            if (getToSquare(move) == board.getEnPassantSquare()) {
                    move = encodeMove(getFromSquare(move), getToSquare(move), EN_PASSANT);
            }
        }

        board.makeMove(move);
    }
}

void Engine::handleGoCommand(std::string_view args) {
    if (!didSetup) {
        doSetup();
    }

    // If the board is empty, set it to the starting position
    if (board.getOccupiedBitboard() == 0ULL) {
        if (!board.setFromFEN(startPosFEN)) {
            sendMessage("ERROR: Could not initialize default position.");
            return;
        }
    }

    std::istringstream iss(args.data());
    std::string arg;

    // Parse arguments. Currently, only wtime and btime or supported, just ignore the others.
    uint32_t whiteTime = 0;
    uint32_t blackTime = 0;
    uint32_t whiteInc = 0;
    uint32_t blackInc = 0;
    uint16_t depth = 0;

    while (iss >> arg) {
        if (arg == "wtime") {
            iss >> whiteTime;
        } else if (arg == "btime") {
            iss >> blackTime;
        } else if (arg == "depth") {
            iss >> depth;
        } else if (arg == "winc") {
            iss >> whiteInc;
        } else if (arg == "binc") {
            iss >> blackInc;
        }
    }

    if (whiteTime == 0 && blackTime == 0 && depth == 0) {
        // TODO: implement support for other args
        sendMessage("ERROR: No time control or depth limit provided.");
        return;
    }

    this->searchStopped = false;
    SearchParams params{};
    SearchStats stats{};

    params.whiteTime = whiteTime;
    params.blackTime = blackTime;
    params.whiteInc = whiteInc;
    params.blackInc = blackInc;
    params.depth = depth;

    Move bestMove;

    if (board.getSideToMove() == WHITE) {
        bestMove = search<WHITE>(*this, board, params, stats);
    } else {
        bestMove = search<BLACK>(*this, board, params, stats);
    }

    sendMessage("bestmove " + getMoveNotation(bestMove));
}

void Engine::handleStopCommand() {
    this->searchStopped = true;
    sendInfoMessage("Search stopped.");
}

void Engine::handlePonderHitCommand(std::string_view args) {
}

void Engine::handleQuitCommand(std::string_view args) {
}

void Engine::handlePerftCommand(const std::string& args) {
    if (!didSetup) {
        doSetup();
    }

    if (args.empty() || args == " " || args == "\n") {
        sendMessage("ERROR: No depth provided.");
        return;
    }

    if (args.contains(' ')) {
        sendMessage("ERROR: Too many arguments provided.");
        return;
    }

    int depth = 0;

    try {
        depth = std::stoi(args);
    } catch (const std::invalid_argument& e) {
        sendMessage("ERROR: Depth must be an integer.");
        return;
    }

    if (depth <= 0) {
        sendMessage("ERROR: Depth must be at least 1.");
        return;
    }

    if (board.getOccupiedBitboard() == 0ULL) {
        board.setFromFEN(startPosFEN);
    }

    const auto start = std::chrono::high_resolution_clock::now();
    const uint64_t nodes = perft(board, depth);
    const auto end = std::chrono::high_resolution_clock::now();
    const std::string tookSeconds = std::to_string(std::chrono::duration<double>(end - start).count());

    sendInfoMessage(
        "Depth: " + std::to_string(depth) + ", Nodes: " + std::to_string(nodes) + ", Time: " + tookSeconds + "s");
}

void Engine::handlePrintCommand() {
    board.print();
}

void Engine::processCommand(const std::string_view command, const std::string& args) {
    if (command == "uci") {
        handleUciCommand();
    } else if (command == "debug") {
        handleDebugCommand(args);
    } else if (command == "isready") {
        handleIsReadyCommand(args);
    } else if (command == "setoption") {
        handleSetOptionCommand(args);
    } else if (command == "register") {
        // Ignore this command, not relevant to this engine
    } else if (command == "ucinewgame") {
        handleUciNewGameCommand();
    } else if (command == "position") {
        handlePositionCommand(args);
    } else if (command == "go") {
        std::thread searchThread{&Engine::handleGoCommand, this, args};

        searchThread.detach();
    } else if (command == "stop") {
        handleStopCommand();
    } else if (command == "ponderhit") {
        handlePonderHitCommand(args);
    } else if (command == "quit") {
        handleQuitCommand(args);
    } else if (command == "perft") {
        handlePerftCommand(args);
    } else if (command == "print") {
        handlePrintCommand();
    } else {
        // If unknown, we must skip it and process the rest.
        if (args.empty() || args == " " || args == "\n") {
            std::cout << "Unknown command: " << command << std::endl;
            return;
        }

        std::string newCommand;
        std::string newArgs;

        if (args.contains(' ')) {
            newCommand = args.substr(0, args.find(' '));
            newArgs = args.substr(args.find(' ') + 1);
        } else {
            newCommand = args;
        }

        processCommand(newCommand, newArgs);
    }
}

void Engine::addOption(UCIOption& option) {
    this->options[option.getName()] = option;
}

UCIOption& Engine::getOption(const std::string& name) {
    UCIOption result = this->options[name];

    if (result.getValue().empty() && !result.getDefaultValue().empty()) {
            result.setValue(result.getDefaultValue());
    }

    return this->options[name];
}

bool Engine::hasOption(const std::string& name) const {
    return this->options.contains(name);
}

bool Engine::isSearchStopped() const {
    return this->searchStopped;
}

void Engine::setSearchStopped(bool value) {
    this->searchStopped = value;
}

void Engine::processLine(const std::string& inputLine) {
    std::string line = removeRedundantSpaces(inputLine);
    std::string command;
    std::string args;

    if (const size_t space_pos = line.find(' '); space_pos != std::string::npos) {
        command = line.substr(0, space_pos);
        args = line.substr(space_pos + 1);
    } else {
        command = line;
        args = "";
    }

    if (args == "\n" || args == " ") {
        args = "";
    }

    processCommand(command, args);
}

void Engine::registerOptions() {
    UCIOption hashOption{"Hash", Spin, "16", "1", "33554432"};
    addOption(hashOption);
}

void Engine::startUci() {
    registerOptions();
    printStartupMessage();
    std::string line;

    while (std::getline(std::cin, line)) {
        processLine(line);
    }
}

void Engine::sendInfoMessage(const std::string_view message) {
    std::cout << "info " << message << std::endl;
}

void Engine::sendMessage(const std::string_view message) {
    std::cout << message << std::endl;
}

std::string removeRedundantSpaces(const std::string_view input) {
    std::string result;
    bool inSpace = false; // Track if we are in a sequence of spaces/tabs

    for (size_t i = 0; i < input.length(); ++i) {
        char current = input[i];

        if (current == '\r' || current == '\t') {
            current = ' ';
        }

        if (std::isspace(current)) {
            if (current == '\n') {
                // Add newline and reset inSpace
                result += '\n';
                inSpace = false;
            } else if (!inSpace) {
                // Only add space if it's the first whitespace in a sequence
                if (!result.empty() && result.back() != '\n' && result.back() != ' ') {
                    result += ' ';
                }
                inSpace = true;
            }
        } else {
            // Add non-space character and reset inSpace
            result += current;
            inSpace = false;
        }
    }

    // Trim trailing space if any
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }

    return result;
}

UCIOptionType UCIOption::getOptionType() const {
    return this->optionType;
}

std::string UCIOption::getName() {
    return this->name;
}

std::string UCIOption::getValue() {
    std::string result = this->value;

    if (result.empty()) {
        return this->defaultValue;
    }

    return result;
}

void UCIOption::setValue(const std::string& value) {
    this->value = value;
}

std::string UCIOption::getDefaultValue() {
    return this->defaultValue;
}

void UCIOption::setDefaultValue(const std::string& value) {
    this->defaultValue = value;
}

std::string UCIOption::getMinValue() {
    return this->minValue;
}

void UCIOption::setMinValue(std::string value) {
    this->minValue = value;
}

std::string UCIOption::getMaxValue() {
    return this->maxValue;
}

void UCIOption::setMaxValue(std::string value) {
    this->maxValue = value;
}

std::string getUciOptionTypeAsString(const UCIOptionType type) {
    switch (type) {
        case Check:
            return "check";
        case Spin:
            return "spin";
        case Combo:
            return "combo";
        case Button:
            return "button";
        case String:
            return "string";
        default:
            return "unknown";
    }
}

std::string UCIOption::toString() {
    std::string result = "option name " + this->name + " type " + getUciOptionTypeAsString(this->optionType);

    if (!this->defaultValue.empty()) {
        result += " default " + this->defaultValue;
    }

    if (!this->minValue.empty()) {
        result += " min " + this->minValue;
    }

    if (!this->maxValue.empty()) {
        result += " max " + this->maxValue;
    }

    if (!this->var.empty()) {
        for (const auto& optionValue : this->var) {
            result += " var " + optionValue;
        }
    }

    return result;
}

void UCIOption::addVar(std::string value) {
    this->var.push_back(value);
}

void UCIOption::setVar(std::vector<std::string> values) {
    this->var = values;
}

void UCIOption::removeVar(std::string value) {
    this->var.erase(std::remove(this->var.begin(), this->var.end(), value), this->var.end());
}

void UCIOption::clearVar() {
    this->var.clear();
}

std::string UCIOption::getVar(const int index) {
    return this->var.at(index);
}

std::vector<std::string> UCIOption::getVar() {
    return this->var;
}
} // namespace Zagreus
