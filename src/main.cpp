#include <iostream>

#include "dory.h"
#include "utils/perft.h"

void timeEvaluation(const Dory::Board& board, int depth, bool whiteToMove) {
    auto start = std::chrono::high_resolution_clock::now();
    auto [eval, line] = Dory::searchDepth(board, depth, whiteToMove);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> seconds = end - start;
    auto ms_int = duration_cast<std::chrono::milliseconds>(seconds);

    DoryUtils::printLine(line, eval);

    unsigned long long nodes = Dory::searchResults.nodesSearched;

    std::cout << "\n\nGenerated " <<  nodes << " nodes in " << ms_int.count() << "ms";

    double knps = (static_cast<double>(nodes) / 1000) / seconds.count();
    if (knps < 1000) {
        std::cout << "\t\t(" << knps << " k nps)\n";
    } else {
        std::cout << "\t\t(" << (knps / 1000) << " M nps)\n";
    }
}

int main() {
    std::string command, fen, depth_str, num_lines_str;
    std::getline(std::cin, command);
    std::getline(std::cin, fen);
    std::getline(std::cin, depth_str);
    int depth = static_cast<int>(std::strtol(depth_str.c_str(), nullptr, 10));

    const auto [board, whiteToMove] = DoryUtils::parseFEN(fen);

    if(command == "perft") {
        auto res = DoryUtils::perftSingleDepth(board, whiteToMove, depth);
        printf("%llu\n", res);
        return 0;
    }
    if(command == "eval") {
        int eval = Dory::staticEvaluation(board, whiteToMove);
        printf("Eval: %d\n", eval);
        return 0;
    }

    Dory::initialize();
    timeEvaluation(board, depth, whiteToMove);

    std::cout << "Table lookups:\t" << Dory::searchResults.tableLookups << std::endl;
    std::cout << "Table size:\t" << Dory::searchResults.trTableSizeMb() << " MB" << std::endl;
    std::cout << "Searched " << Dory::searchResults.nodesSearched << " nodes";
}