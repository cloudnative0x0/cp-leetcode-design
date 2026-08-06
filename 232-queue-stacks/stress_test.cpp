#include <cassert>
#include <iostream>
#include <queue>
#include <sstream>
#include <vector>

#include "QueueStacks.hpp"


struct OpLog {
    std::vector<std::string> lines;

    void add(const std::string& line) {
        lines.push_back(line);
    }

    void dump() const {
        std::cerr << "Operation log leading to failure:\n";
        for (const auto& l : lines) {
            std::cerr << "  " << l << "\n";
        }
    }
};

void runSingleTest(int seed, int opsCount) {
    std::srand(seed);

    QueueStacks impl;
    std::queue<int> reference;
    OpLog log;

    for (int i = 0; i < opsCount; ++i) {
        int choice = std::rand() % 10;

        if (choice < 5) {
            int value = std::rand() % 1000;
            impl.push(value);
            reference.push(value);

            std::ostringstream oss;
            oss << "push(" << value << ")";
            log.add(oss.str());
        } else if (choice < 7) {
            bool implEmpty = impl.empty();
            bool refEmpty = reference.empty();

            if (implEmpty != refEmpty) {
                log.add("empty() mismatch");
                log.dump();
                std::cerr << "impl.empty() = " << implEmpty
                          << ", reference.empty() = " << refEmpty << "\n";
                assert(false && "empty() mismatch");
            }

            std::ostringstream oss;
            oss << "empty() -> " << implEmpty;
            log.add(oss.str());

            if (refEmpty) {
                continue;
            }

            int implVal = impl.peek();
            int refVal = reference.front();

            std::ostringstream oss2;
            oss2 << "peek() -> impl=" << implVal << " ref=" << refVal;
            log.add(oss2.str());

            if (implVal != refVal) {
                log.dump();
                std::cerr << "peek() mismatch: impl=" << implVal
                          << " ref=" << refVal << "\n";
                assert(false && "peek() mismatch");
            }
        } else {
            bool implEmpty = impl.empty();
            bool refEmpty = reference.empty();

            if (implEmpty != refEmpty) {
                log.add("empty() mismatch before pop");
                log.dump();
                std::cerr << "impl.empty() = " << implEmpty
                          << ", reference.empty() = " << refEmpty << "\n";
                assert(false && "empty() mismatch before pop");
            }

            if (refEmpty) {
                log.add("pop() skipped (empty)");
                continue;
            }

            int implVal = impl.pop();
            int refVal = reference.front();
            reference.pop();

            std::ostringstream oss;
            oss << "pop() -> impl=" << implVal << " ref=" << refVal;
            log.add(oss.str());

            if (implVal != refVal) {
                log.dump();
                std::cerr << "pop() mismatch: impl=" << implVal
                          << " ref=" << refVal << "\n";
                assert(false && "pop() mismatch");
            }
        }

        bool implEmptyFinal = impl.empty();
        bool refEmptyFinal = reference.empty();
        if (implEmptyFinal != refEmptyFinal) {
            log.dump();
            std::cerr << "Post-op empty() mismatch: impl=" << implEmptyFinal
                      << " ref=" << refEmptyFinal << "\n";
            assert(false && "Post-op empty() mismatch");
        }
    }
}

int main(int argc, char** argv) {
    int iterations = 2000;
    int opsPerTest = 200;
    int baseSeed = 42;

    if (argc > 1) iterations = std::atoi(argv[1]);
    if (argc > 2) opsPerTest = std::atoi(argv[2]);
    if (argc > 3) baseSeed = std::atoi(argv[3]);

    for (int t = 0; t < iterations; ++t) {
        runSingleTest(baseSeed + t, opsPerTest);

        if ((t + 1) % 200 == 0) {
            std::cout << "Passed " << (t + 1) << "/" << iterations
                      << " test cases\n";
        }
    }

    std::cout << "All " << iterations
              << " stress test cases passed successfully.\n";
    return 0;
}
