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
        sentinel_node(bool is_s = true) : is_sentinel(is_s) {}
        sentinel_node* next = nullptr;
        sentinel_node* prev = nullptr;
    private:
        bool is_sentinel = true;
    };
    struct node : public sentinel_node {
        node() : sentinel_node(false) {}
        T data[NodeMaxSize];
        size_t count = 0;
    };

    sentinel_node* begin_;
    sentinel_node* end_;
    size_t size_ = 0;

    template <bool isConst>
    struct list_iterator {
        using iterator_category = std::bidirectional_iterator_tag;

        list_iterator() = default;
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

        conditional<isConst, const_reference, reference>::type operator*() const { return node->data[index]; }
        conditional<isConst, const_pointer, pointer>::type operator->() const { return &node->data[index]; }

        list_iterator& operator++() {
            if (node->is_sentinel) { return *this; }
            if (++index == node->count) {
                index = 0;
                node = node->next;
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
        bool operator==(list_iterator& rhs) const {
            return !(*this != rhs);
        }
    private:
        sentinel_node* node;
        size_t index;
    };

public:
    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;
};
