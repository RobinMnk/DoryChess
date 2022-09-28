# Dory - Chess Move Generation Engine

Dory is a powerful chess move generation engine, written in C++20 capable of enumerating legal moves at a speed peaking over **1 Billion positions per second**.

It is easy to customize and highly performant, making it a great core for chess engines, chess data analysis or other chess projects.

- **Customization** [[more](#Move-Collectors)]</br>
  The move enumeration behavior can be easily adjusted with custom Move Collectors to serve different purposes. 
- **Efficiency** [[more](#Performance)]</br>
  From the starting position the engine produces positions at roughly 240 million nodes/sec, with increasing speed to over 1 billion node/sec on emptier boards using only a single thread.

The implementation is thoroughly [Perft tested](#Perft-Testing) to ensure its correctness.

## Installation

Make sure you have recent versions of Cmake and of a C++ compiler installed. Then, to build run the following commands from the root directory

```bash
mkdir build
cmake -G Ninja -B build
cmake --build build --target Dory
```

Note: If you also wish to run the [test suites](#Perft Testing), omit the `--target ChessEngine` flag in the last command.

## Usage

To just get the number of legal moves from a given position, first build the program as described above and then switch to the build directory and run

```bash
./Dory "<FEN String>" <depth>
```

with the corresponding FEN string of the position (or `startpos` for the starting position). </br>
*Note that the FEN string has to be wrapped in quotes!*

For example, the nodes at depth 6 from the starting position can be generated like this:

```
./ChessEngine startpos 6
Generated 119060324 nodes in 512ms
232.54 M nps
```

## Performance

The implementation relies heavily on compile time programming, making the execution significantly faster at runtime. From the starting position the legal moves can enumerated at a speed of roughly 240 Million nodes per second. On emptier boards the speed can increase to surpass the mark of 1 Billion nodes per second:

See for example this endgame position at depth 6:

```
./ChessEngine "8/pp2k2p/2nppn2/2p5/1P3N2/3P2N1/P1PK3P/8 w - - 0 1" 6
Generated 250923676 nodes in 243ms
1029.1 M nps
```

## Perft Testing

A standard way of verifying the correctness of a chess engines move generation algorithm is via [Perft Testing](https://www.chessprogramming.org/Perft). We count the number of nodes that are reachable from a given position at a certain depth and compare that to the consensus number computed by other trusted engines, such as Stockfish.

To test the implementation and run the full test suites, build the program as described [above](#Installation) and run the `tester`:

```bash
./tester
[==========] Running 21 tests from 2 test suites.
...
[  PASSED  ] 21 tests.
```

## Move Collectors

The engines main advantage is its modularity. It uses the concept of Move Collectors to handle the behavior when discovering a new position. Upon finding a new move the Move Collector specifies whether to just count the moves, save them to a list or print them to stdout. It is therefore very easy and straightforward to adapt the engines behavior and use it as the basis for any chess project.

## References

This project is a successor of an earlier chess move generation project of mine which was written in Java. It is based on the same algorithm, but enhanced significantly with efficient compile-time programming.

- [Chessprogramming Wiki](https://www.chessprogramming.org) (Lots of super helpful information)
- [Gigantua ](https://github.com/Gigantua/Gigantua) (inspiration and some implementation details)

