#define LAB_3_NO_MAIN
#include "lab_3.cpp"
#undef LAB_3_NO_MAIN

#include <algorithm>
#include <iterator>

namespace lab4 {

class iterator {
public:
    // "паспорт" итератора - эти имена ищут стандартные алгоритмы (std::equal)
    // и std::reverse_iterator, чтобы понять, что умеет итератор
    using iterator_category = std::bidirectional_iterator_tag; // умеет ходить вперёд и назад
    using value_type = int;                                     // на что указывает
    using difference_type = std::ptrdiff_t;                     // тип расстояния между итераторами
    using pointer = int*;                                       // что возвращает ->
    using reference = int&;                                     // что возвращает *

    // деструктор - память не наша, освобождать нечего
    ~iterator() {
    }

    // конструктор по умолчанию - итератор никуда не указывает
    iterator() {
        ptr = nullptr;
    }

    // конструктор копирования - указываем туда же, куда и другой итератор
    iterator(const iterator& other) {
        ptr = other.ptr;
    }

    // конструктор из указателя
    iterator(int* p) {
        ptr = p;
    }

    // присваивание: it1 = it2
    iterator& operator=(const iterator& other) {
        ptr = other.ptr;
        return *this;
    }

    // префиксный ++it: сдвигаемся на следующий элемент и возвращаем себя
    iterator& operator++() {
        ++ptr;
        return *this;
    }

    // постфиксный it++: сдвигаемся, но возвращаем копию "как было до сдвига"
    // (int в скобках ничего не значит - так компилятор отличает it++ от ++it)
    iterator operator++(int) {
        iterator old(*this);
        ++ptr;
        return old;
    }

    // префиксный --it: сдвигаемся на предыдущий элемент
    iterator& operator--() {
        --ptr;
        return *this;
    }

    // постфиксный it--
    iterator operator--(int) {
        iterator old(*this);
        --ptr;
        return old;
    }

    // *it - сам элемент (ссылка, поэтому можно и менять: *it = 5)
    int& operator*() const {
        return *ptr;
    }

    // it-> - указатель на элемент
    int* operator->() const {
        return ptr;
    }

    // итераторы равны, если указывают на одно и то же место
    friend bool operator==(const iterator& a, const iterator& b) {
        return a.ptr == b.ptr;
    }

    friend bool operator!=(const iterator& a, const iterator& b) {
        return !(a == b);
    }

private:
    int* ptr; // на какой элемент сейчас указываем
};

class PriorityQueue : public lab3::PriorityQueue {
public:
    using lab3::PriorityQueue::PriorityQueue;
    using reverse_iterator = std::reverse_iterator<iterator>;

    // конструктор из пары итераторов [first, last): кладём все элементы между ними
    PriorityQueue(iterator first, iterator last) {
        assign(first, last);
    }

    // то же самое для обратных итераторов: PriorityQueue d(c.rbegin(), c.rend());
    PriorityQueue(reverse_iterator first, reverse_iterator last) {
        assign(first, last);
    }

    // итератор на первый элемент массива кучи
    iterator begin() {
        return iterator(data.data());
    }

    // итератор на место ПОСЛЕ последнего элемента (сам элемент там не лежит)
    iterator end() {
        return empty() ? begin() : iterator(data.data() + data.size());
    }

    // обратный итератор на последний элемент
    // (внутри хранит end(), а при * берёт элемент перед ним)
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    // обратный итератор на место ПЕРЕД первым элементом
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    // заменяет содержимое очереди элементами из [first, last).
    // Сначала копируем числа во временный вектор: итераторы могут указывать
    // на эту же очередь (q.assign(q.begin(), q.end())), а clear() их бы испортил.
    void assign(iterator first, iterator last) {
        Vector values;
        for (iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        lab3::PriorityQueue replacement(std::move(values));
        lab3::PriorityQueue::swap(replacement);
    }

    // то же самое для обратных итераторов
    void assign(reverse_iterator first, reverse_iterator last) {
        Vector values;
        for (reverse_iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        lab3::PriorityQueue replacement(std::move(values));
        lab3::PriorityQueue::swap(replacement);
    }
};

}

#ifndef LAB_4_NO_MAIN
using PriorityQueue = lab4::PriorityQueue;

int main() {
    Vector values{19, 47, 74, 91};

    PriorityQueue a(values);
    for (auto it = a.begin(); it != a.end(); ++it) std::cout << *it << " ";

    PriorityQueue b(a.begin(), a.end());
    assert(PriorityQueue::is_equal(a, b));
    for (auto &&it : b) std::cout << it << " ";

    PriorityQueue c(b.begin(), b.end());
    assert(std::equal(c.begin(), c.end(), b.begin()));
    for (auto it = c.rbegin(); it != c.rend(); ++it) std::cout << *it << " ";

    PriorityQueue d(c.rbegin(), c.rend());
    for (auto &&it : d) std::cout << it << " ";
}
#endif
