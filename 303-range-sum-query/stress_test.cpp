#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "RangeSumQuery.hpp"

using namespace std;

int bruteForceSumRange(const vector<int>& nums, int left, int right) {
    int sum = 0;
    for (int i = left; i <= right; i++) {
        sum += nums[i];
    }
    return sum;
}

bool runOneTest(unsigned int seed, int maxSize, int maxQueries, bool verbose) {
    mt19937 rng(seed);
    uniform_int_distribution<int> sizeDist(1, maxSize);
    uniform_int_distribution<int> valDist(-1000, 1000);

    int n = sizeDist(rng);
    vector<int> nums(n);
    for (int i = 0; i < n; i++) {
        nums[i] = valDist(rng);
    }

    vector<int> numsCopy = nums;
    RangeSumQuery rsq(numsCopy);

    uniform_int_distribution<int> idxDist(0, n - 1);
    int queriesCount = uniform_int_distribution<int>(1, maxQueries)(rng);

    vector<string> history;
    history.push_back("nums.size() = " + to_string(n));

    for (int q = 0; q < queriesCount; q++) {
        int a = idxDist(rng);
        int b = idxDist(rng);
        int left = min(a, b);
        int right = max(a, b);

        int got = rsq.sumRange(left, right);
        int expected = bruteForceSumRange(nums, left, right);

        history.push_back("sumRange(" + to_string(left) + ", " + to_string(right) +
                           ") -> got=" + to_string(got) + " expected=" + to_string(expected));

        if (got != expected) {
            if (verbose) {
                cerr << "MISMATCH: sumRange(" << left << ", " << right << ")"
                     << " got=" << got << " expected=" << expected << "\n";
            }
            goto fail;
        }
    }

    return true;

fail:
    if (verbose) {
        cerr << "seed = " << seed << "\n";
        cerr << "nums = [";
        for (size_t i = 0; i < nums.size(); i++) {
            cerr << nums[i] << (i + 1 < nums.size() ? ", " : "");
        }
        cerr << "]\n";
        cerr << "History:\n";
        for (auto& h : history) cerr << "  " << h << "\n";
    }
    return false;
}

int main(int argc, char** argv) {
    int iterations = 100000;
    int maxSize = 200;
    int maxQueries = 100;

    if (argc > 1) iterations = atoi(argv[1]);
    if (argc > 2) maxSize = atoi(argv[2]);
    if (argc > 3) maxQueries = atoi(argv[3]);

    random_device rd;

    for (int iter = 0; iter < iterations; iter++) {
        unsigned int seed = rd();
        bool ok = runOneTest(seed, maxSize, maxQueries, false);

        if (!ok) {
            cerr << "\n=== FAILED on iteration " << iter << " ===\n";
            runOneTest(seed, maxSize, maxQueries, true);
            return 1;
        }

        if (iter % 10000 == 0 && iter > 0) {
            cerr << "OK: " << iter << " iterations passed\n";
        }
    }

    cerr << "All " << iterations << " stress tests passed!\n";
    return 0;
}
