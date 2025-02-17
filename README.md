<p align="center">
  <img src="https://github.com/Dannyj1/Zagreus/blob/master/Logo.png?raw=true" alt="Zagreus Logo">
</p>

![License](https://img.shields.io/github/license/Dannyj1/Zagreus?style=for-the-badge) ![Downloads](https://img.shields.io/github/downloads/Dannyj1/Zagreus/total?style=for-the-badge) ![GitHub commit activity](https://img.shields.io/github/commit-activity/t/Dannyj1/Zagreus?style=for-the-badge) ![GitHub commits since latest release (by date including pre-releases)](https://img.shields.io/github/commits-since/Dannyj1/Zagreus/latest?include_prereleases&style=for-the-badge)

# Note

This is a development branch for the **rewrite** from scratch of Zagreus. Although the engine functions, it's not near
the performance where it should be and a lot of things are missing. The code related to the latest stable release can be
accessed in the [master branch here](https://github.com/Dannyj1/Zagreus/tree/master). No changes will happen to the
master branch until the rewrite is finished.

Consider the information in this readme outdated for the rewrite.

# Zagreus Chess Engine
Zagreus is a work in progress UCI Chess Engine. You can play against Zagreus on
Lichess: https://lichess.org/@/Zagreus_Engine. You will also be able to find it's elo rating on there. At the time of
writing this is around ~2100 ELO rapid.
Zagreus is a constantly changing engine with hopefully an increase in elo every update.

You can download binaries here: https://github.com/Dannyj1/Zagreus/releases

This engine requires a GUI that supports the UCI (Universal Chess Interface) protocol to use it. I personally
use [CuteChess](https://cutechess.com/).

# Development

Development of all new changes and bug fixes happens in a separate branch for each change. When the change is properly
tested using OpenBench and proven to improve ELO, it will be merged with the master branch. Because of this, the master
branch is not the latest released version and may contain bugs.
Every stable release (as in the version has been uploaded under releases) will have a tag. If you want to compile the
stable version for yourself, you can use the tag. You can list all tags and search for tags under the branches dropdown
on GitHub.

# Features

- Bitboard board representation with Plain Magic Bitboards for sliding piece move generation
- Tapered Evaluation with:
    - Material
    - Piece-Square Tables
    - Several evaluation terms per piece type (e.g. passed pawn, king safety, etc.)
    - Penalty for undefended minor pieces
    - And more
- Evaluation values automatically tuned using a gradient descent tuner with the Adam optimizer.
- Principal Variation Search with Alpha-Beta pruning
- Quiescence Search with delta pruning and SEE move ordering
- Move ordering using MVV/LVA, killer moves, history heuristic, countermove heuristic
- Transposition Table
- Null Move Pruning
- And more! This list is constantly growing and changing, but it is difficult to keep track of all features and changes.

# UCI Options

Zagreus Engine has the following UCI options that can be changed:

- `MoveOverhead` - The amount of time that will be substracted from the internal timer for each move. This helps when
  using the engine over the internet, to prevent it from losing on time due to lag. The default is 0.
- `Hash` - The size of the transposition table in megabytes, rounded to the nearest power of 2. The default is 512MB.

# Build Instructions

To build Zagreus, you will need to use LLVM. On Windows, I use [LLVM MinGW](https://github.com/mstorsjo/llvm-mingw). On
Linux I just use [LLVM](https://releases.llvm.org/download.html), but it is also possible to use GCC. LLVM seems to have
slightly better performance.
Zagreus uses CMake to build. On Windows you can use the [CMake-GUI](https://cmake.org/runningcmake/). On Linux, use the
following commands:

Clone the repository:

```bash
git clone https://github.com/Dannyj1/Zagreus.git
```

If you want to use Clang/LLVM, set the compiler to clang++ (not required. If you don't run these commands, your default
compiler will be used which works fine in most cases):

```bash
export CC=/usr/bin/clang-<version here>
export CXX=/usr/bin/clang++-<version here>
```

Build:

```bash
cd <directory of Zagreus engine>
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

# Credits

Thanks to:

- [AndyGrant](https://github.com/AndyGrant) for the easy to use and open
  source [OpenBench Testing Framework](https://github.com/AndyGrant/OpenBench), making testing my engine SO much easier
  and allowing you to distribute test over multiple devices (I use a private instance of OpenBench)
- [maksimKorzh](https://github.com/maksimKorzh) for the
  great [tutorials](https://www.youtube.com/channel/UCB9-prLkPwgvlKKqDgXhsMQ)
  and [engine](https://github.com/maksimKorzh/chess_programming)
- The [Chessprogramming wiki](https://www.chessprogramming.org/Main_Page) contributors for all the information about
  chess programming
- Some ideas in this engine are inspired by [Stockfish](https://github.com/official-stockfish/Stockfish)

# License

Copyright (C) 2023-2025 Danny Jelsma

Zagreus is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Zagreus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Zagreus. If not, see <https://www.gnu.org/licenses/>.
