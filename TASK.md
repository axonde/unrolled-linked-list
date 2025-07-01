# Лабораторная работа 7

Unrolled Linked List. STL - совместивый контейнер

## Задача

Реализовать STL-совместимый контейнер для [UnrolledLinkedList](https://en.wikipedia.org/wiki/Unrolled_linked_list).

## Требования

Контейнер должен предоставлять из себя шаблон, праметрезируемый типом хранимых объетов, максимальным количеством элементов в ноде и аллокатором, а так же частично(см ниже) удовлетворять следующим требованиям к stl - совместимым контейнерам:

  - [контейнера](https://en.cppreference.com/w/cpp/named_req/Container)
  - [последовательный контейнера](https://en.cppreference.com/w/cpp/named_req/SequenceContainer)
     - ~~emplace~~
     - ~~assign_range~~
     - ~~emplace_front~~
     - ~~emplace_back~~
     - ~~prepend_range~~
     - ~~operator[]~~
  - [контейнера с обратным итератором](https://en.cppreference.com/w/cpp/named_req/ReversibleContainer)
  - [контейнера поддерживающие аллокатор](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)
  - [oбладать двунаправленным итератом](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator)


Помимое этого обладать следующими методами 

| Метод     |  Алгоримическая сложность        | Гарантии исключений |
| --------  | -------                          | -------             |
| insert    |  O(1) для 1 элемента, O(M) для M |  strong             |
| erase     |  O(1) для 1 элемента, O(M) для M |  noexcept           |
| clear     |  O(N)                            |  noexcept           |
| push_back |  O(1)                            |  strong             |
| pop_back  |  O(1)                            |  noexcept           |
| push_front|  O(1)                            |  strong             |
| pop_front |  O(1)                            |  noexcept           |


## Тесты

Все вышеуказанные требования должны быть покрыты тестами, с помощью фреймворка [Google Test](http://google.github.io/googletest).

Часть тестов предоставляется заранее. 

## Ограничения

- Запрещено использовать стандартные контейнеры

## ТеорМин

* аллокаторы
* адаптеры
* исключения

## Deadline

1. 18.03.25 23:59 0.8 
2. 25.03.25 23:59 0.65
3. 01.04.25 23:59 0.5

Максимальное количество баллов - 10
