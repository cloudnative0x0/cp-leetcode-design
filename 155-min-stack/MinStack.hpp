#ifndef CP_LEETCODE_DESIGN_MINSTACK_HPP
#define CP_LEETCODE_DESIGN_MINSTACK_HPP

#include <vector>

class MinStack {
    std::vector<int> Arr;
    std::vector<int> MinArr;
    int topIdx;

public:
    MinStack() {
        Arr = std::vector<int>(3000);
        MinArr = std::vector<int>(3000);
        topIdx = 0;
    }

    void push(int value) {
        if (topIdx >= Arr.size()) {
            Arr.resize(Arr.size() * 2);
            MinArr.resize(MinArr.size() * 2);
        }

        Arr[topIdx] = value;

        if (topIdx == 0) {
            MinArr[topIdx] = value;
        } else {
            int currentMin = MinArr[topIdx - 1];

            if (value < currentMin) {
                MinArr[topIdx] = value;
            } else {
                MinArr[topIdx] = currentMin;
            }
        }

        topIdx++;
    }

    void pop() {
        topIdx--;
    }

    [[nodiscard]] int top() const {
        return Arr[topIdx-1];
    }

    [[nodiscard]] int getMin() const {
        return MinArr[topIdx-1];
    }
};

#endif
