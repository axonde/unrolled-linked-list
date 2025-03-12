#include <cstddef>
#include <iterator>
#include <memory>
#include <list>

template<bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = const T*;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;

private:
    struct sentinel_node {
        sentinel_node* next = nullptr;
        sentinel_node* prev = nullptr;
    };
    struct node : private sentinel_node {
        T data[NodeMaxSize];
        size_t count = 0;
    };

    sentinel_node* begin_ = nullptr;
    sentinel_node* end_ = nullptr;
    size_t size_ = 0;

    template <bool isConst>
    struct list_iterator {
        using iterator_category = std::bidirectional_iterator_tag;

        list_iterator() = default;
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

        conditional<isConst, const_reference, reference>::type operator*() const { return node->data[index]; }  // *r
        conditional<isConst, const_pointer, pointer>::type operator->() const { return &node->data[index]; }
        value_type operator*() {  // *r++
            value_type tmp = **this;
            ++*this;
            return tmp;
        }

        list_iterator& operator++() {
            if (index + 1 == node->count) {
                index = 0;
                node = node->next;
            } else {
                ++index;
            }
            return *this;
        }
        list_iterator operator++(int) {
            list_iterator iter = *this;
            ++(*this);
            return iter;
        }
        bool operator!=(list_iterator& rhs) const {
            if (node != rhs.node) { return false; }
            return index != rhs.index;
        }
    private:
        sentinel_node* node;
        size_t index;
    };

public:
    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;
};
