#include <stdexcept>
#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(Iterators, EmptyListEnumeration) {
    unrolled_list<int, 4> list;
    int count = 0;
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        ++count;
        ASSERT_LT(count, 1);
    }
}
TEST(Iterators, SimpleForwardEnumeration) {
    unrolled_list<int, 10> list = {1, 2, 3, 4, 5, 6, 100, -11};  // total 8 elements
    int count = 0;
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        ++count;
        ASSERT_LT(count, 9);
    }
}
TEST(Iterators, SimpleBackwardEnumaration) {
    unrolled_list<int> list = {1, 2, 3, 4, 5, 6, 100, -11};  // total 8 elements
    int count = 0;
    for (auto iter = --list.end(); iter != list.begin(); --iter) {
        ++count;
        ASSERT_LT(count, 8);
    }
}
TEST(Iterators, DecrementBegin) {
    unrolled_list<int> list;
    ASSERT_THROW(--list.begin(), std::out_of_range);
}
TEST(Iterators, IncrementEnd) {
    unrolled_list<int> list;
    ASSERT_THROW(++list.end(), std::out_of_range);
}
TEST(Iterators, BeginEqEndEmptyContainer) {
    unrolled_list<char> list;
    ASSERT_EQ(list.begin(), list.end());
    list.push_back(1);
    ASSERT_NE(list.begin(), list.end());
}
TEST(Iterators, CanDecrementEmptyContainerEnd) {
    unrolled_list<char> list;
    ASSERT_NO_THROW(--list.end());
    ASSERT_EQ(--list.end(), list.begin());
}
TEST(Iterators, DecrementEndAndGetValue) {
    unrolled_list<char> list{1};
    ASSERT_NO_THROW(*--list.end());
}
