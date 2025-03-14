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
    unrolled_list<int, 10, Allocator<int>> list = unrolled_list<int, 10, Allocator<int>>();
    list.clear();
    // std::cout << (list == list4) << ' ' << list.empty() << ' ' << list.max_size() << '\n';

    // unrolled_list<int, 10, Allocator<int>> list5 = {1, 2, 3, 4};
    return 0;
}
