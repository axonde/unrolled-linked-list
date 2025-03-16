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
    unrolled_list<int, 1, Allocator<int>> list2 = {1, 2, 3, 4, 5};
    list2.insert(++list2.begin(), {8, 9, 10, 11, 12, 13});
    for (auto i = list2.begin(); i != list2.end(); ++i) {
        std::cout << *i << ' ';
    }
    std::cout << '\n';

    // auto iter_const = list2.begin();
    // unrolled_list<int, 1, Allocator<int>>::const_iterator iter = iter_const;
    return 0;
}
