#include "StackQueues.hpp"
#include <iostream>
#include <random>
#include <stack>
#include <string>
#include <vector>

using namespace std;

bool runOneTest(unsigned int seed, int maxOps, bool verbose) {
    mt19937 rng(seed);
    uniform_int_distribution<int> opDist(0, 3);
    uniform_int_distribution<int> valDist(1, 1000);

    StackQueues myStack;
    stack<int> refStack;
    vector<string> history;

    int opsCount = uniform_int_distribution<int>(1, maxOps)(rng);

    for (int i = 0; i < opsCount; i++) {
        int op = opDist(rng);

        if ((op == 1 || op == 2) && refStack.empty()) {
            op = 0;
        }

        if (op == 0) {
            int x = valDist(rng);
            myStack.push(x);
            refStack.push(x);
            history.push_back("push(" + to_string(x) + ")");
        } else if (op == 1) {
            int got = myStack.pop();
            int expected = refStack.top();
            refStack.pop();
            history.push_back("pop() -> got=" + to_string(got) +
                               " expected=" + to_string(expected));
            if (got != expected) {
                if (verbose) {
                    cerr << "MISMATCH on pop(): got=" << got
                         << " expected=" << expected << "\n";
                }
                goto fail;
            }
        } else if (op == 2) {
            int got = myStack.top();
            int expected = refStack.top();
            history.push_back("top() -> got=" + to_string(got) +
                               " expected=" + to_string(expected));
            if (got != expected) {
                if (verbose) {
                    cerr << "MISMATCH on top(): got=" << got
                         << " expected=" << expected << "\n";
                }
                goto fail;
            }
        } else { // op == 3
            bool got = myStack.empty();
            bool expected = refStack.empty();
            history.push_back(string("empty() -> got=") + (got ? "true" : "false") +
                               " expected=" + (expected ? "true" : "false"));
            if (got != expected) {
                if (verbose) {
                    cerr << "MISMATCH on empty(): got=" << got
                         << " expected=" << expected << "\n";
                }
                goto fail;
            }
        }
    }

    return true;

fail:
    if (verbose) {
        cerr << "seed = " << seed << "\n";
        cerr << "History of operations:\n";
        for (auto &h : history) cerr << "  " << h << "\n";
    }
    return false;
}

int main(int argc, char** argv) {
    int iterations = 100000;
    int maxOps = 50;

    if (argc > 1) iterations = atoi(argv[1]);
    if (argc > 2) maxOps = atoi(argv[2]);

    random_device rd;

    for (int iter = 0; iter < iterations; iter++) {
        unsigned int seed = rd();
        bool ok = runOneTest(seed, maxOps, /*verbose=*/false);

        if (!ok) {
            cerr << "\n=== FAILED on iteration " << iter << " ===\n";
            runOneTest(seed, maxOps, /*verbose=*/true);
            return 1;
        }

        if (iter % 10000 == 0 && iter > 0) {
            cerr << "OK: " << iter << " iterations passed\n";
        }
    }

    cerr << "All " << iterations << " stress tests passed!\n";
    return 0;
}