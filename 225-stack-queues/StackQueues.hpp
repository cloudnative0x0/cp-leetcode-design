#ifndef CP_DATA_STRUCTURES_STACKQUEUES_HPP
#define CP_DATA_STRUCTURES_STACKQUEUES_HPP

#include <queue>

class StackQueues {
    std::queue<int> enQueue;
    std::queue<int> deQueue;

public:
    StackQueues() = default;

    void push(int x) {
        enQueue.push(x);

        while (!deQueue.empty()) {
            enQueue.push(deQueue.front());
            deQueue.pop();
        }

        std::swap(deQueue, enQueue);
    }

    int pop() {
        int head = deQueue.front();
        deQueue.pop();

        return head;
    }

    [[nodiscard]] int top() const {
        return deQueue.front();
    }

    [[nodiscard]] bool empty() const {
        return deQueue.empty();
    }
};

#endif
