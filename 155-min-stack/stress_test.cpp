#include <cassert>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include "MinStack.hpp"

int main() {
    std::cout << "Starting stress test..." << std::endl;
    std::mt19937 rng(42);

    const int iterations       = 100000;
    const int opsPerIteration  = 50;
    const int maxStackSize     = 30;

    std::uniform_int_distribution<int> opDist(0, 3);
    std::uniform_int_distribution<int> valDist(-10000, 10000);

    for (int i = 0; i < iterations; ++i) {
        MinStack minStack;
        std::vector<int> ref;

        for (int j = 0; j < opsPerIteration; ++j) {
            int op;
            if (ref.empty()) {
                op = 0;
            } else if (ref.size() >= static_cast<std::size_t>(maxStackSize)) {
                op = opDist(rng) % 3 + 1;
            } else {
                op = opDist(rng);
            }

            switch (op) {
                case 0: { // push
                    int val = valDist(rng);
                    minStack.push(val);
                    ref.push_back(val);
                    break;
                }
                case 1: { // pop
                    minStack.pop();
                    ref.pop_back();
                    break;
                }
                case 2: { // top
                    assert(!ref.empty());
                    assert(minStack.top() == ref.back());
                    break;
                }
                case 3: { // getMin
                    assert(!ref.empty());
                    int expectedMin = *std::min_element(ref.begin(), ref.end());
                    assert(minStack.getMin() == expectedMin);
                    break;
                }
                default:
                    assert(false && "unreachable");
            }

            if (!ref.empty() && (op == 0 || op == 1)) {
                assert(minStack.top() == ref.back());
                int expectedMin = *std::min_element(ref.begin(), ref.end());
                assert(minStack.getMin() == expectedMin);
            }
        }
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}