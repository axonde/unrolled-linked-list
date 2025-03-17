#include <iostream>
#include <vector>

#include <unrolled_list.h>

template<typename T>
class Allocator {
public:
    using value_type = T;
    T* allocate(size_t size) {
        T* ptr = static_cast<T*>(malloc(size * sizeof(T)));
        std::cout << "allocate new item: " << ptr << "; size: " << size << '\n';
        return ptr;
    }
    void deallocate(T* ptr, size_t size) {
        std::cout << "deallocate memory: " << ptr << "\n";
        free(ptr);
    }
};

int main(int argc, char** argv) {
    std::cout << "Hello, world!" << std::endl;
    // unrolled_list<int, 1, Allocator<int>> list2 = {1, 2};
    // list2.insert(++list2.begin(), {8, 9, 10, 11, 12, 13});
    unrolled_list<int, 5, Allocator<int>> list;
    for (int i = 0; i < 1; ++i) {
        list.push_back(i);
        list.pop_front();
    }
    // list.pop_front();
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
        std::cout << *iter << ' ';
    }
    std::cout << '\n' << list.size() << '\n';
    return 0;
}
