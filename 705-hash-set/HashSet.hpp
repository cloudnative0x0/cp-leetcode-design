#ifndef CP_DATA_STRUCTURES_HASHSET_HPP
#define CP_DATA_STRUCTURES_HASHSET_HPP

#include <vector>
#include <memory>

class MyHashSet {
    struct Node {
        int key;
        std::unique_ptr<Node> next;

        explicit Node(int k) : key(k), next(nullptr) {}
    };

    std::vector<std::unique_ptr<Node>> buckets;
    size_t bucket_count;

    [[nodiscard]] size_t hash(int key) const noexcept {
        const auto signed_bucket_count = static_cast<long long>(bucket_count);
        long long h = static_cast<long long>(key) % signed_bucket_count;
        if (h < 0) {
            h += signed_bucket_count;
        }

        return h;
    }

public:
    explicit MyHashSet(size_t capacity = 10007)
        : buckets(capacity), bucket_count(capacity) {}

    // block coping of hash set
    MyHashSet(const MyHashSet&) = delete;
    MyHashSet& operator = (const MyHashSet&) = delete;

    // allow move semantics
    MyHashSet(MyHashSet&&) noexcept = default;
    MyHashSet& operator = (MyHashSet&&) noexcept = default;

    ~MyHashSet() = default;

    void add(int key) {
        size_t index = hash(key);

        if (!buckets[index]) {
            buckets[index] = std::make_unique<Node>(key);
            return;
        }

        Node* current = buckets[index].get();
        while (true) {
            if (current->key == key) {
                return;
            }
            if (!current->next) {
                break;
            }

            current = current->next.get();
        }

        current->next = std::make_unique<Node>(key);
    }

    void remove(int key) {
        size_t index = hash(key);

        if (!buckets[index]) {
            return;
        }

        if (buckets[index]->key == key) {
            buckets[index] = std::move(buckets[index]->next);
            return;
        }

        Node* current = buckets[index].get();
        while (current->next) {
            if (current->next->key == key) {
                current->next = std::move(current->next->next);
                return;
            }

            current = current->next.get();
        }
    }

    [[nodiscard]] bool contains(int key) const noexcept {
        size_t index = hash(key);
        const Node* current = buckets[index].get();

        while (current) {
            if (current->key == key) {
                return true;
            }

            current = current->next.get();
        }

        return false;
    }
};

#endif