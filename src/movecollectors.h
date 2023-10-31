//
// Created by robin on 17.07.2022.
//

#ifndef DORY_MOVECOLLECTORS_H
#define DORY_MOVECOLLECTORS_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include "board.h"
#include "movegen.h"
#include "utils.h"

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
    template<int depth>
    class LimitedDFS {
    public:
        static unsigned long long totalNodes;

        template<State state>
        static void generateGameTree(const Board& board) {
            if constexpr (depth > 0) {
                MoveGenerator<LimitedDFS<depth>>::template generate<state>(board);
            }
        }

    private:
        template<State state, Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove(const Board& board, BB from, BB to) {
            if constexpr (depth == 1) {
                totalNodes++;
            }

            constexpr State nextState = getNextState<state, flags>();
            Board nextBoard = board.getNextBoard<state, piece, flags>(from, to);
            LimitedDFS<depth-1>::template generateGameTree<nextState>(nextBoard);
        }

        friend class MoveGenerator<LimitedDFS<depth>>;
    };

    template<int depth>
    unsigned long long LimitedDFS<depth>::totalNodes{0};


//    /**
//     * A Movecollector that produces a list of successor boards from the given position.
//     */
//    class SuccessorBoards {
//    public:
//        static std::vector<ExtendedBoard> positions;
//
//        static void getLegalMoves(ExtendedBoard& eboard) {
//            Utils::template run<SuccessorBoards, 1>(eboard.state_code, eboard.board);
//        }
//
//        template<State state, int depth>
//        static void main(Board& board) {
//            generateGameTree<state, depth>(board);
//        }
//
//        template<State state, int depth>
//        static void generateGameTree(Board& board) {
//            positions.clear();
//            MoveGenerator<SuccessorBoards>::template generate<state, 1>(board);
//        }
//
//    private:
//        template<State state, int depth, Piece_t piece, Flag_t flags = MoveFlag::Silent>
//        static void registerMove(const Board &board, [[maybe_unused]] BB from, [[maybe_unused]] BB to) {
//            positions.push_back(getExtendedBoard<state>(board));
//        }
//
//        template<State nextState, int depth>
//        static void next([[maybe_unused]] Board& nextBoard) {}
//
//        static void done() {}
//
//        friend class MoveGenerator<SuccessorBoards>;
//    };
//
//    std::vector<ExtendedBoard> SuccessorBoards::positions{};


    static std::vector<unsigned long long> nodes;

    template<int depth>
    class PerftCollector {
    public:

        template<State state>
        static void generateGameTree(const Board& board) {
            if constexpr (depth > 0) {
                MoveGenerator<PerftCollector<depth>>::template generate<state>(board);
            }
        }

    private:
        template<State state,  Piece_t piece, Flag_t flags = MoveFlag::Silent>
        static void registerMove(const Board& board, BB from, BB to) {
            nodes.at(depth)++;
            constexpr State nextState = getNextState<state, flags>();
            Board nextBoard = board.getNextBoard<state, piece, flags>(from, to);
            PerftCollector<depth-1>::template generateGameTree<nextState>(nextBoard);
        }

        friend class MoveGenerator<PerftCollector<depth>>;
    };

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
                moves.push_back(Utils::moveNameShort(m));
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

        static void done() {
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