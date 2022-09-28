//
// Created by robin on 17.07.2022.
//

#ifndef DORY_MOVECOLLECTORS_H
#define DORY_MOVECOLLECTORS_H

#include <unordered_map>
#include <vector>
#include "movegen.h"
#include "utils.h"
#include "fenreader.h"

/**
 * A namespace containing various classes for collecting the moves generated by movegen.
 */
namespace MoveCollectors {

    /**
     * Fastest Movecollector, but tree depth has to be known at compiletime!
     *
     * @tparam saveBoards - whether resulting boards at lowest level should be saved in 'positions'
     * @tparam print - whether moves should be printed to stdout. Only recommended for very small depths
     */
    template<bool saveBoards, bool print>
    class LimitedDFS {
    public:
        static unsigned long long totalNodes;
        static std::vector<Board> positions;

        template<State state, int depth>
        static void generateGameTree(Board& board) {
            totalNodes = 0;
            build<state, depth>(board);

            if constexpr (saveBoards)
                totalNodes = positions.size();
        }

    private:
        template<State state, int depth>
        static void build(Board& board) {
            if constexpr (depth > 0) {
                MoveGenerator<LimitedDFS<saveBoards, print>>::template generate<state, depth>(board);
            }
        }

        template<State state, int depth, Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove(const Board &board, BB from, BB to) {
            if constexpr (depth == 1) {
                if constexpr (saveBoards)
                    positions.push_back(board);
                else
                    totalNodes++;
            }

            if constexpr (print) {
                Utils::printMove( {from, to, piece, flags} );
            }
        }

        template<State nextState, int depth>
        static void next(Board& nextBoard) {
            build<nextState, depth-1>(nextBoard);
        }

        friend class MoveGenerator<LimitedDFS<saveBoards, print>>;
    };

    template<bool saveList, bool print>
    unsigned long long LimitedDFS<saveList, print>::totalNodes{0};
    template<bool saveList, bool print>
    std::vector<Board> LimitedDFS<saveList, print>::positions{};


    /**
     * A Movecollector that produces a list of successor boards from the given position.
     */
    class SuccessorBoards {
    public:
        static std::vector<ExtendedBoard> positions;

        static void getLegalMoves(ExtendedBoard& eboard) {
            Utils::template run<SuccessorBoards, 1>(eboard.state_code, eboard.board);
        }

        template<State state, int depth>
        static void main(Board& board) {
            generateGameTree<state, depth>(board);
        }

        template<State state, int depth>
        static void generateGameTree(Board& board) {
            positions.clear();
            MoveGenerator<SuccessorBoards>::template generate<state, 1>(board);
        }

    private:
        template<State state, int depth, Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove(Board &board, [[maybe_unused]] BB from, [[maybe_unused]] BB to) {
            positions.push_back(getExtendedBoard<state>(board));
        }

        template<State nextState, int depth>
        static void next([[maybe_unused]] Board& nextBoard) {}

        friend class MoveGenerator<SuccessorBoards>;
    };

    std::vector<ExtendedBoard> SuccessorBoards::positions{};


    class PerftCollector {
    public:
        static std::vector<unsigned long long> nodes;
        static int maxDepth;

        template<State state, int depth>
        static void generateGameTree(Board& board) {
            nodes.clear();
            nodes.resize(depth+1);
            maxDepth = depth;
            build<state, depth>(board);
        }

    private:
        template<State state, int depth>
        static void build(Board& board) {
            if constexpr (depth > 0) {
                MoveGenerator<PerftCollector>::template generate<state, depth>(board);
            }
        }

        template<State state, int depth, Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove([[maybe_unused]] const Board &board, [[maybe_unused]] BB from, [[maybe_unused]] BB to) {
            nodes.at(maxDepth - depth + 1)++;
        }

        template<State nextState, int depth>
        static void next(Board& nextBoard) {
            build<nextState, depth-1>(nextBoard);
        }

        friend class MoveGenerator<PerftCollector>;
    };

    std::vector<unsigned long long> PerftCollector::nodes{};
    int PerftCollector::maxDepth{0};

    /**
     * A Movecollector for listing the divide output for a given position.
     * For every legal move the number of resulting follow-up positions at the given depth is calculated.
     * Used mainly for debugging purposes.
     */
    class Divide {
    public:
        static std::vector<std::string> moves;
        static std::vector<uint64_t> nodes;
        static unsigned long long curr, totalNodes;
        static int maxDepth;

        template<State state, int depth>
        static void generateGameTree(Board& board) {
            maxDepth = depth;
            build<state, depth>(board);
        }

        static void print() {
            for(unsigned int i{0}; i < curr; i++) {
                std::cout << moves.at(i) << ": " << nodes.at(i) << std::endl;
            }

            std::cout << "\nTotal nodes searched: " << totalNodes << std::endl;
        }

    private:
        template<State state, int depth>
        static void build(Board& board) {
            if constexpr (depth > 0) {
                MoveGenerator<Divide>::template generate<state, depth>(board);
            }
        }

        template<State state, int depth, Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove([[maybe_unused]] const Board &board, BB from, BB to) {
            if (depth == maxDepth) {
                Move m{from, to, piece, flags};
                moves.push_back(Utils::moveNameShort<state.whiteToMove>(m));
                nodes.push_back(0);
                curr++;
            }

            if constexpr(depth == 1) {
                nodes.back()++;
                totalNodes++;
            }
        }

        template<State nextState, int depth>
        static void next(Board& nextBoard) {
            build<nextState, depth-1>(nextBoard);
        }

        friend class MoveGenerator<Divide>;
    };

    std::vector<std::string> Divide::moves{};
    std::vector<uint64_t> Divide::nodes{};
    unsigned long long Divide::curr{0};
    unsigned long long Divide::totalNodes{0};
    int Divide::maxDepth{1};
}

#endif //DORY_MOVECOLLECTORS_H
