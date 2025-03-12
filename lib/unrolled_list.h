#include <cstddef>
#include <iterator>
#include <memory>
#include <list>
#include <stdexcept>

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
    using size_type = size_t;

private:
    struct sentinel_node {
        sentinel_node(bool is_s = true) : is_sentinel(is_s) {}
        std::shared_ptr<sentinel_node> next;
        std::shared_ptr<sentinel_node> prev;
        const bool is_sentinel = true;
    };
    struct node : public sentinel_node {
        using sentinel_node::next;
        using sentinel_node::prev;
        node() : sentinel_node(false) {}
        node(const node& n) : node() {
            next = prev = std::shared_ptr<sentinel_node>();
            data = n.data;
            count = n.count;
        }
        T data[NodeMaxSize];
        size_t count = 0;
    };

    std::shared_ptr<sentinel_node> begin_;
    std::shared_ptr<sentinel_node> end_;
    size_t size_ = 0;

    template <bool isConst>
    struct list_iterator {
        using iterator_category = std::bidirectional_iterator_tag;

        list_iterator() = default;
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

        conditional<isConst, const_reference, reference>::type operator*() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator");}
            return node->data[index]; 
        }
        conditional<isConst, const_pointer, pointer>::type operator->() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator");}
            return &node->data[index];
        }

        list_iterator& operator++() {
            if (node->is_sentinel) { throw std::out_of_range("cannot increment the end (rend) iterator"); }
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

        list_iterator& operator--() {
            if (node->prev->is_sentinel) { throw std::out_of_range("cannot decrement the begin (rbegin) iterator"); }
            if (index == 0) {
                node = node->prev;
                index = node->count - 1;
            }
            return *this;
        }
        list_iterator operator--(int) {
            list_iterator iter = *this;
            --(*this);
            return iter;
        }

        bool operator!=(list_iterator& rhs) const noexcept {
            if (node != rhs.node) { return false; }
            return index != rhs.index;
        }
        bool operator==(list_iterator& rhs) const noexcept {
            return !(*this != rhs);
        }
    private:
        sentinel_node* node;
        size_t index;
    };

public:
    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;

    unrolled_list() {
        begin_ = end_ = std::make_shared<sentinel_node>();
    }
    unrolled_list(const unrolled_list& ul) : unrolled_list() {
        if (ul.begin_.is_sentinel) { return; }
        const sentinel_node& other = ul.begin_;
        begin_.reset(); begin_ = std::make_shared<node>();
        // begin_->prev = end_;
        // sentinel_node& curr = begin_;
        // do {
        //     curr = ???
        // }
        // while (!other.is_sentinel) {
        //     curr = 
        //     curr.next = std::make_shared<node>(other);
        //     curr.next->prev = std::make_shared<node>(&curr);
        // }
    }

    ~unrolled_list() {
        
    }
};
