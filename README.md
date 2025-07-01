# Unrolled Linked List

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

A high-performance, STL-compatible container implementing an unrolled linked list with bidirectional iterators.

## What is an Unrolled Linked List?

An unrolled linked list is a data structure that combines the advantages of arrays and linked lists. Unlike a traditional linked list where each node contains a single element, an unrolled linked list stores multiple elements in each node (typically in a small array). This design offers several benefits:

- Improved cache locality compared to traditional linked lists
- Reduced memory overhead for pointers
- Faster traversal while maintaining O(1) insertion and deletion
- Better memory usage patterns for modern CPU architectures

```
+---------------------------------------------------+
|                Unrolled Linked List               |
+---------------------------------------------------+

+---------------+        +---------------+        +---------------+
| Node 1        |        | Node 2        |        | Node 3        |
|               |        |               |        |               |
| +-----------+ | next   | +-----------+ | next   | +-----------+ |
| | [5][7][9] | |------->| | [2][4][8] | |------->| | [1][3][6] | |
| +-----------+ |        | +-----------+ |        | +-----------+ |
|               |<-------|               |<-------|               |
|     prev      | prev   |     prev      | prev   |     prev      |
+---------------+        +---------------+        +---------------+
```

## Features

- **STL-Compatible**: Works seamlessly with STL algorithms and containers
- **Bidirectional Iterators**: Full support for forward and backward traversal
- **Optimized Performance**: Better cache utilization than standard linked lists
- **Modern C++**: Implements C++17 features and best practices
- **Header-Only**: Easy to include in your projects
- **Comprehensive Testing**: Thoroughly tested with various use cases

## Installation

This is a header-only library. Simply include the header file in your project:

```cpp
#include "unrolled_list.hpp"
```

### Requirements

- C++17 compatible compiler
- CMake 3.14 or higher (for building tests and examples)

### Building Examples and Tests

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```cpp
#include <iostream>
#include "unrolled_list.hpp"

int main() {
    // Create an unrolled list of integers
    axonde::unrolled_list<int> list;
    
    // Add elements
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    // Insert elements
    auto it = list.begin();
    ++it;
    list.insert(it, 10);  // Insert 10 after the first element
    
    // Iterate and print
    for (const auto& value : list) {
        std::cout << value << " ";
    }
    // Output: 1 10 2 3
    
    return 0;
}
```

## API Overview

The `unrolled_list<T>` class template provides an interface similar to `std::list`:

### Member Types

- `value_type`: The type of elements stored
- `iterator`: Bidirectional iterator type
- `const_iterator`: Constant bidirectional iterator type
- `reference`, `const_reference`: References to elements
- `size_type`: Unsigned integer type for sizes

### Methods

#### Element Access

- `front()`: Returns a reference to the first element
- `back()`: Returns a reference to the last element

#### Iterators

- `begin()`, `cbegin()`: Returns an iterator to the beginning
- `end()`, `cend()`: Returns an iterator to the end
- `rbegin()`, `crbegin()`: Returns a reverse iterator to the beginning
- `rend()`, `crend()`: Returns a reverse iterator to the end

#### Capacity

- `empty()`: Checks if the container is empty
- `size()`: Returns the number of elements

#### Modifiers

- `clear()`: Clears the contents
- `insert()`: Inserts elements
- `emplace()`: Constructs elements in-place
- `erase()`: Erases elements
- `push_back()`, `emplace_back()`: Adds an element to the end
- `push_front()`, `emplace_front()`: Adds an element to the beginning
- `pop_back()`: Removes the last element
- `pop_front()`: Removes the first element
- `resize()`: Changes the number of elements stored
- `swap()`: Swaps the contents

## Performance

Unrolled linked lists generally outperform traditional linked lists for traversal operations while maintaining comparable performance for insertions and deletions. The specific performance characteristics depend on the node size and the nature of operations.

| Operation | Time Complexity | Speed vs. `std::list` |
|-----------|----------------|------------------------|
| Access    | O(n)           | Faster                 |
| Insert    | O(1)*          | Comparable             |
| Delete    | O(1)*          | Comparable             |
| Iterate   | O(n)           | Much faster            |

\* Amortized time complexity assuming the location is known
`n` - count of elements;

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by research on cache-efficient data structures
- Made with ❤️ by [axonde](https://github.com/axonde)
