#include <iostream>

#include "movecollectors.h"
#include "fenreader.h"

using Collector = MoveCollectors::LimitedDFS<false, false>;

struct Runner {
    template<State state, int depth>
    static void main(Board& board) {
        Utils::time_movegen<Collector, state, depth>(board);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << R"( "<FEN>" "<Depth>")" << std::endl;
        return 1;
    }
    std::cout << "Chess Engine" << std::endl;

    std::string_view fen{argv[1]};
    int depth = static_cast<int>(std::strtol(argv[2], nullptr, 10));

    PieceSteps::load();
    Utils::loadFEN<Runner>(fen, depth);

    return 0;
}
