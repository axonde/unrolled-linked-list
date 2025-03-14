#include <cstddef>
#include <compare>
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
        sentinel_node* next;
        sentinel_node* prev;
        const bool is_sentinel = true;
        virtual ~sentinel_node() {}
    };
    struct node : public sentinel_node {
        using sentinel_node::next;
        using sentinel_node::prev;
        node() : sentinel_node(false) {}
        node(const node& n) : node() {
            next = prev = {};
            data = n.data;
            count = n.count;
        }
        T data[NodeMaxSize];
        size_t count = 0;
    };

    sentinel_node* begin_;
    sentinel_node* end_;
    size_type size_ = 0;

    template <bool isConst>
    struct list_iterator {
        using iterator_category = std::bidirectional_iterator_tag;

        list_iterator() = default;
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

        conditional<isConst, const_reference, reference>::type operator*() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator"); }
            return static_cast<struct node*>(node)->data[index]; 
        }
        conditional<isConst, const_pointer, pointer>::type operator->() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator");}
            return &static_cast<struct node*>(node)->data[index];
        }

        list_iterator& operator++() {
            if (node->is_sentinel) { throw std::out_of_range("cannot increment the end (rend) iterator"); }
            if (++index == static_cast<struct node*>(node)->count) {
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
                index = static_cast<struct node*>(node)->count - 1;
            }
            return *this;
        }
        list_iterator operator--(int) {
            list_iterator iter = *this;
            --(*this);
            return iter;
        }

        bool operator==(const list_iterator& rhs) const noexcept {
            if (node != rhs.node) { return false; }
            if (node->is_sentinel && rhs.node->is_sentinel) { return true; }
            if (node->is_sentinel || rhs.node->is_sentinel) { return false; }
            return index == rhs.index && static_cast<struct node*>(node)->data[index] == static_cast<struct node*>(rhs.node)->data[rhs.index];
        }
        bool operator!=(const list_iterator& rhs) const noexcept {
            return !(*this == rhs);
        }
    private:
        sentinel_node* node;
        size_t index;
    };

    using sentinel_node_allocator_type = std::allocator_traits<Allocator>::template rebind_alloc<sentinel_node>;
    sentinel_node_allocator_type sentinel_node_allocator;
    using node_allocator_type = std::allocator_traits<Allocator>::template rebind_alloc<node>;
    node_allocator_type node_allocator;

    /// @brief copy to a clear object
    /// @warning be sure, that you copy into a clear object
    void initialize_copy(const unrolled_list& ul) {
        if (ul.begin_->is_sentinel) { return; }
        const sentinel_node* other = ul.begin_;
        begin_ = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, begin_, *other);
        begin_->prev = end_;
        sentinel_node* curr;
        sentinel_node* curr_prev = begin_;
        while (!other->next->is_sentinel) {
            curr = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
            std::allocator_traits<node_allocator_type>::construct(node_allocator, curr, *other->next);
            curr_prev->next = curr;
            curr->prev = curr_prev;
            curr_prev = curr;
            other = other->next;
        }
    }

public:
    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;

    unrolled_list() {
        begin_ = end_ = std::allocator_traits<sentinel_node_allocator_type>::allocate(sentinel_node_allocator, 1);
        std::allocator_traits<sentinel_node_allocator_type>::construct(sentinel_node_allocator, begin_);
        begin_->prev = begin_->next = end_->prev = end_->next = begin_;
    }
    unrolled_list(const unrolled_list& ul) : unrolled_list() { initialize_copy(ul); }
    unrolled_list& operator=(const unrolled_list& rhs) {
        if (this == &rhs) { return *this; }
        clear();
        initialize_copy(rhs);
        return *this;
    }

    ~unrolled_list() {
        clear();
        std::allocator_traits<sentinel_node_allocator_type>::destroy(sentinel_node_allocator, end_);
        std::allocator_traits<sentinel_node_allocator_type>::deallocate(sentinel_node_allocator, end_, 1);
    }

    iterator begin() { return {begin_, 0}; }
    const_iterator cbegin() const { return {begin_, 0}; }
    iterator end() { return {end_, 0}; }
    const_iterator cend() const { return {end_, 0}; }

    bool operator==(const unrolled_list& rhs) const {
        if (size_ != rhs.size_) { return false; }
        unrolled_list::const_iterator curr_lhs = cbegin();
        unrolled_list::const_iterator curr_rhs = rhs.cbegin();
        while (curr_lhs != cend()) {
            if (*curr_lhs++ != *curr_rhs++) { return false; }
        }
        return true;
    }
    bool operator!=(const unrolled_list& rhs) const { return !(*this == rhs); }

    void swap(unrolled_list& rhs) {
        unrolled_list tmp(rhs);
        *this = rhs;
        rhs = tmp;
    }
    inline static void swap(unrolled_list& lhs, unrolled_list& rhs) { lhs.swap(rhs); }

    size_type size() const { return size_; }
    size_type max_size() const { return std::allocator_traits<node_allocator_type>::max_size(node_allocator) * NodeMaxSize; }

    bool empty() const { return size_ == 0; }

    void clear() {
        sentinel_node* del;
        while (!begin_->is_sentinel) {
            del = begin_;
            begin_ = begin_->next;
            std::allocator_traits<sentinel_node_allocator_type>::destroy(sentinel_node_allocator, del);
            std::allocator_traits<sentinel_node_allocator_type>::deallocate(sentinel_node_allocator, del, 1);
        }
        size_ = 0;
        begin_ = begin_->next = begin_->prev = end_->prev = end_;
    }
};
