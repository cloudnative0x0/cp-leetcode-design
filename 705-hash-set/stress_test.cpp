#include <iostream>
#include <random>
#include <unordered_set>
#include <chrono>
#include <string>

#include "HashSet.hpp"

namespace {

int g_checks_run = 0;
int g_checks_failed = 0;
std::string g_current_test;

void check_impl(bool condition, const std::string& expr, int line) {
    ++g_checks_run;
    if (!condition) {
        ++g_checks_failed;
        std::cerr << "  [FAIL] " << g_current_test << " (line " << line
                  << "): " << expr << "\n";
    }
}

#define CHECK(cond) check_impl((cond), #cond, __LINE__)

void begin_test(const std::string& name) {
    g_current_test = name;
    std::cout << "-- " << name << "\n";
}

} // namespace


void test_default_construct_empty() {
    begin_test("constructor: a fresh set is empty for any key");
    MyHashSet s;
    CHECK(!s.contains(0));
    CHECK(!s.contains(1));
    CHECK(!s.contains(-1));
    CHECK(!s.contains(1000000));
}

void test_custom_capacity() {
    begin_test("constructor: works with a small explicit capacity");
    MyHashSet s(1);
    s.add(1);
    s.add(2);
    s.add(3);
    CHECK(s.contains(1));
    CHECK(s.contains(2));
    CHECK(s.contains(3));
    CHECK(!s.contains(4));
}


void test_add_single() {
    begin_test("add: a single element ends up in the set");
    MyHashSet s;
    s.add(42);
    CHECK(s.contains(42));
}

void test_add_duplicate_is_noop() {
    begin_test("add: inserting the same key twice does not break the set");
    MyHashSet s;
    s.add(5);
    s.add(5);
    s.add(5);
    CHECK(s.contains(5));
    s.remove(5);
    CHECK(!s.contains(5));
}

void test_add_negative_and_zero() {
    begin_test("add: negative keys and zero are handled correctly");
    MyHashSet s(17);
    s.add(0);
    s.add(-1);
    s.add(-100);
    s.add(-17); // multiple of capacity - checks hash() on negative numbers
    CHECK(s.contains(0));
    CHECK(s.contains(-1));
    CHECK(s.contains(-100));
    CHECK(s.contains(-17));
    CHECK(!s.contains(-2));
}

void test_add_forces_collision_chain() {
    begin_test("add: several keys in the same bucket form a chain");
    MyHashSet s(1); // capacity=1 -> every key lands in the same bucket
    for (int k = 0; k < 20; ++k) {
        s.add(k);
    }
    for (int k = 0; k < 20; ++k) {
        CHECK(s.contains(k));
    }
    CHECK(!s.contains(20));
}

void test_add_same_bucket_different_keys() {
    begin_test("add: different keys colliding modulo capacity");
    MyHashSet s(10);
    s.add(3);
    s.add(13);  // 13 % 10 == 3 -> same bucket as 3
    s.add(23);  // also lands in the same bucket
    CHECK(s.contains(3));
    CHECK(s.contains(13));
    CHECK(s.contains(23));
    CHECK(!s.contains(33));
}


void test_remove_from_empty_set() {
    begin_test("remove: removing from an empty set does not crash or do anything");
    MyHashSet s;
    s.remove(1); // must not throw/crash
    CHECK(!s.contains(1));
}

void test_remove_nonexistent_key() {
    begin_test("remove: removing a missing key leaves existing ones intact");
    MyHashSet s;
    s.add(1);
    s.add(2);
    s.remove(999);
    CHECK(s.contains(1));
    CHECK(s.contains(2));
}

void test_remove_only_element_in_bucket() {
    begin_test("remove: the sole element in a bucket is fully removed");
    MyHashSet s;
    s.add(7);
    CHECK(s.contains(7));
    s.remove(7);
    CHECK(!s.contains(7));
}

void test_remove_head_of_chain() {
    begin_test("remove: removing the head of a chain (first inserted into the bucket)");
    MyHashSet s(1);
    s.add(1); // head
    s.add(2);
    s.add(3);
    s.remove(1);
    CHECK(!s.contains(1));
    CHECK(s.contains(2));
    CHECK(s.contains(3));
}

void test_remove_middle_of_chain() {
    begin_test("remove: removing a node in the middle of a chain");
    MyHashSet s(1);
    s.add(1);
    s.add(2); // middle
    s.add(3);
    s.remove(2);
    CHECK(s.contains(1));
    CHECK(!s.contains(2));
    CHECK(s.contains(3));
}

void test_remove_tail_of_chain() {
    begin_test("remove: removing the tail of a chain");
    MyHashSet s(1);
    s.add(1);
    s.add(2);
    s.add(3); // tail
    s.remove(3);
    CHECK(s.contains(1));
    CHECK(s.contains(2));
    CHECK(!s.contains(3));
}

void test_remove_all_then_readd() {
    begin_test("remove: removing every element of a chain and reinserting");
    MyHashSet s(1);
    for (int k = 0; k < 10; ++k) s.add(k);
    for (int k = 0; k < 10; ++k) s.remove(k);
    for (int k = 0; k < 10; ++k) CHECK(!s.contains(k));

    s.add(100);
    CHECK(s.contains(100));
}

void test_remove_twice_is_safe() {
    begin_test("remove: removing the same key twice is safe");
    MyHashSet s;
    s.add(5);
    s.remove(5);
    s.remove(5); // second time - the key is already gone
    CHECK(!s.contains(5));
}


void test_contains_on_empty_set() {
    begin_test("contains: false for any key on an empty set");
    MyHashSet s;
    CHECK(!s.contains(0));
    CHECK(!s.contains(-5));
    CHECK(!s.contains(5));
}

void test_contains_reflects_add_remove_order() {
    begin_test("contains: correctly reflects the sequence of add/remove calls");
    MyHashSet s;
    CHECK(!s.contains(10));
    s.add(10);
    CHECK(s.contains(10));
    s.remove(10);
    CHECK(!s.contains(10));
    s.add(10);
    CHECK(s.contains(10));
}

void test_contains_does_not_confuse_colliding_keys() {
    begin_test("contains: does not confuse different keys with the same hash");
    MyHashSet s(5);
    s.add(2);
    s.add(7);  // 7 % 5 == 2 -> collides with 2
    CHECK(s.contains(2));
    CHECK(s.contains(7));
    s.remove(2);
    CHECK(!s.contains(2));
    CHECK(s.contains(7)); // 7 must not be affected
}


void test_move_construct_transfers_state() {
    begin_test("move constructor: the new object contains all elements of the old one");
    MyHashSet s1;
    s1.add(1);
    s1.add(2);
    s1.add(3);

    MyHashSet s2(std::move(s1));
    CHECK(s2.contains(1));
    CHECK(s2.contains(2));
    CHECK(s2.contains(3));
}

void test_move_assign_transfers_state() {
    begin_test("move assignment: the target receives all elements of the source");
    MyHashSet s1;
    s1.add(10);
    s1.add(20);

    MyHashSet s2;
    s2.add(999); // must be overwritten by the move assignment

    s2 = std::move(s1);
    CHECK(s2.contains(10));
    CHECK(s2.contains(20));
    CHECK(!s2.contains(999));
}


bool run_unit_tests() {
    std::cout << "=== Unit tests ===\n";

    test_default_construct_empty();
    test_custom_capacity();

    test_add_single();
    test_add_duplicate_is_noop();
    test_add_negative_and_zero();
    test_add_forces_collision_chain();
    test_add_same_bucket_different_keys();

    test_remove_from_empty_set();
    test_remove_nonexistent_key();
    test_remove_only_element_in_bucket();
    test_remove_head_of_chain();
    test_remove_middle_of_chain();
    test_remove_tail_of_chain();
    test_remove_all_then_readd();
    test_remove_twice_is_safe();

    test_contains_on_empty_set();
    test_contains_reflects_add_remove_order();
    test_contains_does_not_confuse_colliding_keys();

    test_move_construct_transfers_state();
    test_move_assign_transfers_state();

    std::cout << g_checks_run << " checks run, " << g_checks_failed
              << " failed.\n\n";
    return g_checks_failed == 0;
}


namespace {

struct StressResult {
    bool ok;
    unsigned long long seed;
    std::string message;
};

StressResult run_single_stress_test(unsigned long long seed, int op_count, int key_range) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<int> key_dist(-key_range, key_range);
    std::uniform_int_distribution<int> op_dist(0, 2); // 0=add, 1=remove, 2=contains

    MyHashSet hash_set(/*capacity=*/17); // small capacity -> lots of collisions
    std::unordered_set<int> reference;

    for (int i = 0; i < op_count; ++i) {
        int key = key_dist(rng);
        int op = op_dist(rng);

        switch (op) {
            case 0: // add
                hash_set.add(key);
                reference.insert(key);
                break;
            case 1: // remove
                hash_set.remove(key);
                reference.erase(key);
                break;
            case 2: { // contains
                bool expected = reference.count(key) > 0;
                bool actual = hash_set.contains(key);
                if (expected != actual) {
                    return {false, seed,
                            "Mismatch on contains(" + std::to_string(key) +
                            ") at step " + std::to_string(i) +
                            ": expected=" + std::to_string(expected) +
                            " actual=" + std::to_string(actual)};
                }
                break;
            }
            default:
                break;
        }
    }

    for (int k = -key_range; k <= key_range; ++k) {
        bool expected = reference.count(k) > 0;
        bool actual = hash_set.contains(k);
        if (expected != actual) {
            return {false, seed,
                    "Final state mismatch for key=" + std::to_string(k) +
                    ": expected=" + std::to_string(expected) +
                    " actual=" + std::to_string(actual)};
        }
    }

    return {true, seed, "ok"};
}

} // namespace

bool run_stress_tests(int argc, char** argv) {
    std::cout << "=== Stress tests ===\n";

    unsigned long long base_seed = argc > 1
        ? std::strtoull(argv[1], nullptr, 10)
        : static_cast<unsigned long long>(
              std::chrono::steady_clock::now().time_since_epoch().count());

    int iterations = argc > 2 ? std::atoi(argv[2]) : 1;
    const int op_count = 5000;
    const int key_range = 50;

    if (argc > 1 && base_seed != 0) {
        iterations = 1;
    }

    std::mt19937_64 seed_gen(base_seed == 0 ? std::random_device{}() : base_seed);

    for (int it = 0; it < iterations; ++it) {
        unsigned long long seed = (argc > 1 && base_seed != 0) ? base_seed : seed_gen();
        StressResult result = run_single_stress_test(seed, op_count, key_range);

        if (!result.ok) {
            std::cerr << "FAILED on seed=" << result.seed << "\n"
                      << "Reason: " << result.message << "\n"
                      << "Repro: ./test_hashset " << result.seed << "\n";
            return false;
        }

        std::cout << "[" << (it + 1) << "/" << iterations << "] seed=" << seed
                  << " -> OK\n";
    }

    std::cout << "All " << iterations << " iteration(s) passed.\n";
    return true;
}


int main(int argc, char** argv) {
    bool unit_ok = run_unit_tests();
    bool stress_ok = run_stress_tests(argc, argv);

    if (!unit_ok || !stress_ok) {
        std::cerr << "\nSome tests FAILED.\n";
        return 1;
    }

    std::cout << "\nAll tests passed.\n";
    return 0;
}
