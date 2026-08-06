#ifndef CP_DATA_STRUCTURES_QUEUESTACKS_H
#define CP_DATA_STRUCTURES_QUEUESTACKS_H

#include <stack>

class QueueStacks {
    std::stack<int> enStack;
    std::stack<int> deStack;

public:
    QueueStacks() = default;

    void push(int x) {
        enStack.push(x);
    }

    int pop() {
        moveElement();

        int head = deStack.top();
        deStack.pop();

        return head;
    }

    int peek() {
        moveElement();

        return deStack.top();
    }

    bool empty() {
        return enStack.empty() && deStack.empty();
    }

private:
    void moveElement() {
        if (deStack.empty()) {
            while (!enStack.empty()) {
                int topIdx = enStack.top();

                deStack.push(topIdx);
                enStack.pop();
            }
        }
    }
};

#endif
