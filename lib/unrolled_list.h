#include <cstddef>
#include <compare>
#include <iterator>
#include <memory>
#include <stdexcept>

#include <iostream>

template<bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
    static_assert(NodeMaxSize > 0, "NodeMaxSize must be greater than zero");
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = const T*;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;
    using allocator_type = Allocator;

private:
    struct sentinel_node {
        sentinel_node(bool is_s = true) : is_sentinel(is_s) {}
        sentinel_node* next;
        sentinel_node* prev;
        const bool is_sentinel = true;
    };
    struct node : sentinel_node {
        using sentinel_node::next;
        using sentinel_node::prev;
        node() : sentinel_node(false) {}
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
    private:
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

    public:
        conditional<isConst, const_reference, reference>::type operator*() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator"); }
            return static_cast<struct node*>(node)->data[index]; 
        }
        conditional<isConst, const_pointer, pointer>::type operator->() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator"); }
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
        friend unrolled_list;
    private:
        sentinel_node* node;
        size_t index;
    };

    using sentinel_node_allocator_type = std::allocator_traits<Allocator>::template rebind_alloc<sentinel_node>;
    sentinel_node_allocator_type sentinel_node_allocator;
    using node_allocator_type = std::allocator_traits<Allocator>::template rebind_alloc<node>;
    node_allocator_type node_allocator;
    allocator_type allocator;

    void initialize_copy(const unrolled_list& ul) {
        for (auto iter = ul.begin(); iter != ul.end(); ++iter) {
            this->push_back(*iter);
        }
    }
    template<typename ForwardIterator>
    void initialize_copy(ForwardIterator begin, ForwardIterator end) {
        while (begin != end) { this->push_back(*begin++); }
    }

public:
    using iterator = list_iterator<false>;
    using const_iterator = list_iterator<true>;

    /// @brief we allocate to node:  <node [begin]> -- <sentinel_node [end]>
    /// and we will be always work between begin and end:  <node [begin]> -- {inserting} -- <sentinel_node [end]>
    unrolled_list() {
        begin_ = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, static_cast<node*>(begin_));
        end_ = std::allocator_traits<sentinel_node_allocator_type>::allocate(sentinel_node_allocator, 1);
        std::allocator_traits<sentinel_node_allocator_type>::construct(sentinel_node_allocator, end_);
        begin_->prev = begin_->next = end_->next = end_;
        end_->prev = begin_;
    }
    unrolled_list(const unrolled_list& ul) : unrolled_list() { initialize_copy(ul); }
    unrolled_list(const unrolled_list& ul, const allocator_type& alloc) : unrolled_list(ul), allocator(alloc) {}
    unrolled_list(const allocator_type& alloc) : unrolled_list(), allocator(alloc) {}
    unrolled_list(size_type n, value_type el) : unrolled_list() {
        for (size_type i = 0; i != n; ++i) { this->push_back(el); }
    }
    template<typename ForwardIterator>
    unrolled_list(ForwardIterator begin, ForwardIterator end) : unrolled_list() { initialize_copy(begin, end); }
    unrolled_list(std::initializer_list<T> il) : unrolled_list() { initialize_copy(il.begin(), il.end()); }
    unrolled_list& operator=(const unrolled_list& rhs) {
        if (this == &rhs) { return *this; }
        clear();
        initialize_copy(rhs);
        return *this;
    }
    unrolled_list& operator=(std::initializer_list<T> il) {
        clear();
        initialize_copy(il.begin(), il.end());
    }

    ~unrolled_list() {
        clear();
        std::allocator_traits<node_allocator_type>::destroy(node_allocator, static_cast<node*>(begin_));
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator, static_cast<node*>(begin_), 1);
        std::allocator_traits<sentinel_node_allocator_type>::destroy(sentinel_node_allocator, end_);
        std::allocator_traits<sentinel_node_allocator_type>::deallocate(sentinel_node_allocator, end_, 1);
    }

    iterator begin() { return {begin_, 0}; }
    const_iterator begin() const { return {begin_, 0}; }
    const_iterator cbegin() const { return {begin_, 0}; }
    iterator end() { return {end_, 0}; }
    const_iterator end() const { return {end_, 0}; }
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
        sentinel_node* t_begin = begin_;
        sentinel_node* t_end = end_;
        size_t t_size = size_;
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        size_ = rhs.size_;
        rhs.begin_ = t_begin;
        rhs.end_ = t_end;
        rhs.size_ = t_size;
    }
    inline static void swap(unrolled_list& lhs, unrolled_list& rhs) { lhs.swap(rhs); }

    size_type size() const { return size_; }
    size_type max_size() const { return std::allocator_traits<node_allocator_type>::max_size(node_allocator) * NodeMaxSize; }

    allocator_type get_allocator() const { return allocator; }

    bool empty() const { return size_ == 0; }

    void clear() noexcept {
        sentinel_node* del;
        sentinel_node* curr_node = begin_->next;
        while (!curr_node->is_sentinel) {
            del = curr_node;
            curr_node = curr_node->next;
            std::allocator_traits<node_allocator_type>::destroy(node_allocator, static_cast<node*>(del));
            std::allocator_traits<node_allocator_type>::deallocate(node_allocator, static_cast<node*>(del), 1);
        }
        size_ = 0;
        static_cast<node*>(begin_)->count = 0;
        begin_->next = end_;
        end_->prev = begin_;
    }

private:
    void split(iterator& iter) {
        node* new_node = std::allocator_traits<node_allocator_type>::allocate(node_allocator, 1);
        std::allocator_traits<node_allocator_type>::construct(node_allocator, new_node);
        new_node->next = iter.node->next;
        new_node->prev = iter.node;
        new_node->next->prev = new_node;
        iter.node->next = new_node;
        for (size_t i = 0; i != NodeMaxSize - iter.index; ++i) {
            new_node->data[i] = static_cast<node*>(iter.node)->data[iter.index + i];
        }
        if (new_node->count = NodeMaxSize - iter.index; new_node->count == 0) {
            iter.node = new_node;
            iter.index = 0;
        }
        static_cast<node*>(iter.node)->count = iter.index;
    }

public:
    iterator insert(iterator iter, const T& value) {
        if (iter.node->is_sentinel) {
            iter.node = iter.node->prev;
            iter.index = static_cast<node*>(iter.node)->count;
        }
        if (static_cast<node*>(iter.node)->count == NodeMaxSize) {
            split(iter);
        }
        node* iter_node = static_cast<node*>(iter.node);
        for (size_type i = 0; i != iter_node->count - iter.index; ++i) {
            iter_node->data[iter_node->count - i] = iter_node->data[iter_node->count - i - 1];
        }
        *iter = value;
        ++iter_node->count;
        return iter;
    }
    iterator insert(iterator iter, size_type n, const T& value) {
        for (size_type i = 0; i != n; ++i) {
            iter = insert(iter, value);
        }
        return iter;
    }
    void push_back(const T& value) {
        insert(end(), value);
    }
};
