#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(Erases, EraseAllListOneNode) {
    unrolled_list<int, 5> list = {1, 2, 3, 4, 5};
    for (auto iter = list.begin(); iter != list.end();) {
        ASSERT_NO_THROW(iter = list.erase(iter));
    }
    EXPECT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
}

TEST(Erases, EraseOneNodeFront) {
    unrolled_list<int, 5> list = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i != 5; ++i) {
        ASSERT_NO_THROW(list.erase(list.begin()));
    }
    EXPECT_FALSE(list.empty());
    ASSERT_EQ(list.size(), 1);
}

TEST(Erases, EraseAdvancedOneNodeFront) {
    unrolled_list<int, 5> list = {1, 2, 3, 4, 5, 6};
    for (int i = 1; i != 7; ++i) {
        ASSERT_EQ(i, *list.begin());
        ASSERT_NO_THROW(list.erase(list.begin()));
    }
    EXPECT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
}

TEST(Erases, EraseEmptyContainer) {
    unrolled_list<int, 1> list;
    ASSERT_EQ(list.erase(list.begin()), list.end());
}
