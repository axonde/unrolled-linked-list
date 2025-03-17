#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <iostream>

#include <vector>

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
    static_assert(NodeMaxSize > 0, "NodeMaxSize must be greater than zero");

    template<bool B, typename U, typename F>
    struct conditional { using type = U; };

    template<typename U, typename F>
    struct conditional<false, U, F> { using type = F; };

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
        // T data[NodeMaxSize];
        alignas(T) unsigned char data[sizeof(T) * NodeMaxSize];
        size_t count = 0;
    };

    sentinel_node* begin_;
    sentinel_node* end_;
    size_type size_ = 0;

    template <bool isConst>
    struct list_iterator {
        using value_type = unrolled_list::value_type;
        using difference_type = unrolled_list::difference_type;
        using reference = unrolled_list::reference;
        using pointer = unrolled_list::pointer;
        using iterator_category = std::bidirectional_iterator_tag;
        friend unrolled_list;

        list_iterator() = default;
        template <bool OtherConst, typename = std::enable_if_t<isConst || !OtherConst>>
        list_iterator(const list_iterator<OtherConst>& other) : node(other.node), index(other.index) {}
    private:
        list_iterator(sentinel_node* n, size_t i) : node(n), index(i) {}

    public:
        conditional<isConst, const_reference, reference>::type operator*() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator"); }
            return reinterpret_cast<T*>(static_cast<struct node*>(node)->data)[index];
        }
        conditional<isConst, const_pointer, pointer>::type operator->() const {
            if (node->is_sentinel) { throw std::invalid_argument("cannot dereference a no-value iterator"); }
            return reinterpret_cast<T*>(static_cast<struct node*>(node)->data) + index;
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
            } else { --index; }
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

    using sentinel_node_allocator_type = std::allocator<sentinel_node>;
    using node_allocator_type = std::allocator_traits<Allocator>::template rebind_alloc<node>;
    sentinel_node_allocator_type sentinel_node_allocator;
    node_allocator_type node_allocator;
    allocator_type allocator;

    void initialize_copy(const unrolled_list& ul) {
        for (auto iter = ul.begin(); iter != ul.end(); ++iter) { push_back(*iter); }
    }
    template<typename ForwardIterator>
    void initialize_copy(ForwardIterator begin, ForwardIterator end) {
        while (begin != end) { push_back(*begin++); }
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
    unrolled_list(const unrolled_list& ul, const allocator_type& alloc) : unrolled_list(ul) { allocator = alloc; }
    unrolled_list(const allocator_type& alloc) : unrolled_list() { allocator = alloc; }
    unrolled_list(size_type n, value_type el) : unrolled_list() {
        for (size_type i = 0; i != n; ++i) { push_back(el); }
    }
    template<typename ForwardIterator>
    unrolled_list(ForwardIterator begin, ForwardIterator end) : unrolled_list() { initialize_copy(begin, end); }
    template<typename ForwardIterator>
    unrolled_list(ForwardIterator begin, ForwardIterator end, Allocator& alloc) : unrolled_list(begin, end) { allocator = alloc; }
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
    size_type max_size() const { return std::allocator_traits<node_allocator_type>::max_size(node_allocator) * NodeMaxSize / sizeof(T); }

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
            new(&reinterpret_cast<T*>(new_node->data)[i]) T(reinterpret_cast<T*>(static_cast<node*>(iter.node)->data)[iter.index + i]);
            reinterpret_cast<T*>(static_cast<node*>(iter.node)->data)[iter.index + i].~T();
        }
        new_node->count = NodeMaxSize - iter.index;
        if (new_node->count == 0) {
            iter.node = new_node;
            iter.index = 0;
        }
        static_cast<node*>(iter.node)->count = iter.index;
    }
    void deallocate_node(iterator& iter) {
        iter.node->prev->next = iter.node->next;
        iter.node->next->prev = iter.node->prev;
        sentinel_node* next_node = iter.node->next; iter.index = 0;
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator, static_cast<node*>(iter.node), 1);
        iter.node = next_node;
    }

public:
    iterator insert(const_iterator const_iter, const T& value) {
        iterator iter(const_iter.node, const_iter.index);
        if (iter.node->is_sentinel) {
            iter.node = iter.node->prev;
            iter.index = static_cast<node*>(iter.node)->count;
        }
        if (static_cast<node*>(iter.node)->count == NodeMaxSize) {
            split(iter);
        }
        node* iter_node = static_cast<node*>(iter.node);
        for (size_type i = 0; i != iter_node->count - iter.index; ++i) {
            new (&reinterpret_cast<T*>(iter_node->data)[iter_node->count - i]) T(reinterpret_cast<T*>(iter_node->data)[iter_node->count - i - 1]);
            reinterpret_cast<T*>(iter_node->data)[iter_node->count - i - 1].~T();
        }
        *iter = value;
        ++iter_node->count;
        ++size_;
        return iter;
    }
    iterator insert(const_iterator const_iter, size_type n, const T& value) {
        iterator iter = {const_iter.node, const_iter.index};
        iterator copy = iter;
        for (size_type i = 0; i != n; ++i) {
            iter = insert(iter, value); ++iter;
        }
        return copy;
    }
    template <typename InputIterator, typename = std::enable_if_t<
        !std::is_integral<InputIterator>::value &&
        std::is_same_v<
            typename std::iterator_traits<InputIterator>::iterator_category,
            std::input_iterator_tag
        >
    >>
    iterator insert(const_iterator const_iter, InputIterator begin, InputIterator end) {
        iterator iter = {const_iter.node, const_iter.index};
        iterator copy = iter;
        if (begin == end) { return copy; }
        iter = copy = insert(iter, *begin++);
        while (begin != end) { iter = insert(iter, *begin++); ++iter; }
        return copy;
    }
    iterator insert(const_iterator const_iter, std::initializer_list<T> il) {
        iterator iter = {const_iter.node, const_iter.index};
        iterator copy = iter;
        for (auto i = il.begin(); i != il.end(); ++i) {
            iter = insert(iter, *i);
            if (i == il.begin()) { copy = iter; }
            ++iter;
        }
        return copy;
    }

    iterator erase(const_iterator const_iter) {
        if (const_iter.node->is_sentinel || (const_iter.node == begin_ && size_ == 0)) {
            return end();
        }
        iterator iter = {const_iter.node, const_iter.index};
        node* casted_node = static_cast<node*>(iter.node);
        for (size_type i = iter.index; i != casted_node->count; ++i) {
            if (i != iter.index) {
                new (&reinterpret_cast<T*>(casted_node->data)[i - 1]) T(reinterpret_cast<T*>(casted_node->data)[i]);
            }
            reinterpret_cast<T*>(casted_node->data)[i].~T();
        }
        if (--casted_node->count == 0) { deallocate_node(iter); }
        --size_;
        return iter;
    }
    iterator erase(const_iterator begin, const_iterator end) {
        iterator iter = {begin.node, begin.index};
        while (iter != end && iter != end_) {
            iter = erase(iter);
        }
        return iter;
    }

    template<typename InputIterator>
    void assign(InputIterator begin, InputIterator end) {
        clear();
        initialize_copy(begin, end);
    }
    void assing(std::initializer_list<T> il) { assing(il.begin(), il.end()); }
    void assing(size_type n, const T& value) {
        clear();
        for (size_type i = 0; i != n; ++i) { push_back(value); }
    }

    void push_back(const T& value) { insert(end(), value); }
    void push_front(const T& value) { insert(begin(), value); }

    void pop_front() { erase(begin()); }
    void pop_back() { erase(--end()); }  // trouble!
};
