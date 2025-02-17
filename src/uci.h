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

#pragma once

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "board.h"

namespace Zagreus {
class UCIOption;

class Engine {
private:
    bool didSetup = false;
    bool searchStopped = false;
    std::map<std::string, UCIOption> options{};
    Board board{};

    void handleUciCommand();
    void handleDebugCommand(std::string_view args);
    void handleIsReadyCommand(std::string_view args);
    void handleSetOptionCommand(const std::string& args);
    void handleUciNewGameCommand();
    void handlePositionCommand(std::string_view args);
    void handleGoCommand(std::string_view args);
    void handleStopCommand();
    void handlePonderHitCommand(std::string_view args);
    void handleQuitCommand(std::string_view args);
    void handlePerftCommand(const std::string& args);
    void handlePrintCommand();
    void processCommand(std::string_view command, const std::string& args);
    void processLine(const std::string& inputLine);

public:
    Engine() {
        // Needs to be here so the position command does not mess with the zobrist hash, while we still do the most expensive setup when the specification allows it
        initZobristConstants();
    }

    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) = delete;

    void startUci();
    void registerOptions();
    void sendInfoMessage(std::string_view message);
    void sendMessage(std::string_view message);
    void doSetup();
    [[nodiscard]] static std::string getVersionString() ;
    void printStartupMessage();
    void addOption(UCIOption& option);
    UCIOption& getOption(const std::string& name);
    bool hasOption(const std::string& name) const;
    bool isSearchStopped() const;
    void setSearchStopped(bool value);
};

enum UCIOptionType {
    Check,
    Spin,
    Combo,
    Button,
    String
};

class UCIOption {
private:
    std::string name;
    UCIOptionType optionType;
    std::string value;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    std::vector<std::string> var;
public:
    UCIOption() = default;

    UCIOption(std::string name, const UCIOptionType optionType) : name(std::move(name)), optionType(optionType) {}

    UCIOption(std::string name, const UCIOptionType optionType, std::string defaultValue) : name(std::move(name)), optionType(optionType), defaultValue(std::move(defaultValue)) {
    }

    UCIOption(std::string name, const UCIOptionType optionType, std::string defaultValue, std::string minValue, std::string maxValue) : name(std::move(name)), optionType(optionType), defaultValue(std::move(defaultValue)), minValue(std::move(minValue)), maxValue(std::move(maxValue)) {
    }

    UCIOptionType getOptionType() const;

    std::string getName();

    std::string getValue();

    void setValue(const std::string& value);

    std::string getDefaultValue();

    void setDefaultValue(const std::string& value);

    std::string getMinValue();

    void setMinValue(std::string value);

    std::string getMaxValue();

    void setMaxValue(std::string value);

    std::string toString();

    void addVar(std::string value);

    void setVar(std::vector<std::string> values);

    void removeVar(std::string value);

    void clearVar();

    std::string getVar(int index);

    std::vector<std::string> getVar();
};

std::string removeRedundantSpaces(std::string_view input);
} // namespace Zagreus