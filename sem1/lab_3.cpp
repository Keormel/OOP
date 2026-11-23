#define LAB_2_NO_MAIN
#include "lab_2.cpp"
#undef LAB_2_NO_MAIN

#include <cassert>
#include <sstream>

class Vector {
private:
    int* elements = nullptr;
    size_t length = 0;
    size_t capacity = 0;

public:
    Vector() = default;

    Vector(std::initializer_list<int> values) {
        reserve(values.size());
        for (int value : values) {
            push_back(value);
        }
    }

    Vector(const Vector& other) {
        reserve(other.length);
        for (size_t index = 0; index < other.length; ++index) {
            elements[index] = other.elements[index];
        }
        length = other.length;
    }

    Vector(Vector&& other) noexcept {
        swap(other);
    }

    ~Vector() {
        delete[] elements;
    }

    Vector& operator=(Vector other) noexcept {
        swap(other);
        return *this;
    }

    void reserve(size_t count) {
        if (count <= capacity) {
            return;
        }
        int* replacement = new int[count];
        for (size_t index = 0; index < length; ++index) {
            replacement[index] = elements[index];
        }
        delete[] elements;
        elements = replacement;
        capacity = count;
    }

    void push_back(int value) {
        if (length == capacity) {
            reserve(capacity == 0 ? 1 : capacity * 2);
        }
        elements[length++] = value;
    }

    void pop_back() {
        if (!empty()) {
            --length;
        }
    }

    void clear() {
        length = 0;
    }

    size_t size() const {
        return length;
    }

    bool empty() const {
        return length == 0;
    }

    int& operator[](size_t index) {
        return elements[index];
    }

    const int& operator[](size_t index) const {
        return elements[index];
    }

    int& back() {
        return elements[length - 1];
    }

    int* data() {
        return elements;
    }

    void swap(Vector& other) noexcept {
        std::swap(elements, other.elements);
        std::swap(length, other.length);
        std::swap(capacity, other.capacity);
    }
};

namespace lab3 {

class PriorityQueue : public lab2::PriorityQueue<Vector, true> {
public:
    using lab2::PriorityQueue<Vector, true>::PriorityQueue;

    PriorityQueue() = default;
    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue(PriorityQueue&&) noexcept = default;

    size_t length() const {
        return data.size();
    }

    // оператор присваивания копированием: a = b
    PriorityQueue& operator=(const PriorityQueue& other) {
        if (this == &other) {
            return *this; // a = a - ничего делать не нужно
        }
        data = other.data; // вектор сам освобождает старую память и копирует элементы
        return *this;      // возвращаем себя, чтобы работало a = b = c
    }

    // доступ к элементу массива кучи по индексу.
    // возвращает ССЫЛКУ, поэтому можно и читать, и писать: b[4] = 0
    // (если записать сюда число, порядок кучи может нарушиться)
    int& operator[](size_t index) {
        return data[index];
    }

    const int& operator[](size_t index) const {
        return data[index];
    }

    // есть ли число в очереди
    bool contains(int value) const {
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] == value) {
                return true;
            }
        }
        return false;
    }

    // сколько раз число встречается в очереди
    size_t count(int value) const {
        size_t result = 0;
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] == value) {
                result++;
            }
        }
        return result;
    }

    // равны, если одинаковой длины и элементы на одних и тех же местах совпадают
    friend bool operator==(const PriorityQueue& a, const PriorityQueue& b) {
        if (a.data.size() != b.data.size()) {
            return false;
        }
        for (size_t i = 0; i < a.data.size(); i++) {
            if (a.data[i] != b.data[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const PriorityQueue& a, const PriorityQueue& b) {
        return !(a == b);
    }

    // сравнение "как в словаре": идём по элементам до первого различия;
    // если все общие элементы совпали - меньше та очередь, что короче
    friend bool operator<(const PriorityQueue& a, const PriorityQueue& b) {
        size_t common = a.data.size() < b.data.size() ? a.data.size() : b.data.size();
        for (size_t i = 0; i < common; i++) {
            if (a.data[i] != b.data[i]) {
                return a.data[i] < b.data[i];
            }
        }
        return a.data.size() < b.data.size();
    }

    // остальные сравнения выражаем через <
    friend bool operator>(const PriorityQueue& a, const PriorityQueue& b) {
        return b < a;
    }

    friend bool operator<=(const PriorityQueue& a, const PriorityQueue& b) {
        return !(b < a);
    }

    friend bool operator>=(const PriorityQueue& a, const PriorityQueue& b) {
        return !(a < b);
    }

    // вывод: std::cout << a; печатает элементы в том порядке, как они лежат в массиве
    // возвращает поток, поэтому можно писать std::cout << a << b;
    friend std::ostream& operator<<(std::ostream& out, const PriorityQueue& q) {
        for (size_t i = 0; i < q.data.size(); i++) {
            out << q.data[i] << " ";
        }
        out << std::endl;
        return out;
    }

    // ввод: ss >> a; выбрасывает старое содержимое и читает числа до конца потока
    friend std::istream& operator>>(std::istream& in, PriorityQueue& q) {
        q.data.clear(); // память, выделенная заранее (reserve), остаётся
        int value;
        while (in >> value) {
            q.push(value);
        }
        return in;
    }
};

}

#ifndef LAB_3_NO_MAIN
using PriorityQueue = lab3::PriorityQueue;

int main() {
    std::stringstream ss{"1 3 5 7 9"};
    Vector values{1, 3, 5, 7, 9};
    PriorityQueue a(values);
    assert(5 == a.length());
    assert(1 == a.top());
    ss >> a;
    assert(5 == a.length());
    assert(1 == a[0]);
    assert(9 == a[4]);
    std::cout << a;
    PriorityQueue b{a};
    assert(a == b);
    assert(3 == b[1]);
    assert(7 == b[3]);
    b[4] = 0;
    assert(0 == b[4]);
    assert(!b.contains(9));
    assert(b < a);
    assert(a > b);
    std::cout << b;
    PriorityQueue c;
    assert(0 == c.length());
    c = b;
    assert(b == c);
    c[1] = c[2] = 7;
    assert(7 == c[1]);
    assert(7 == c[2]);
    assert(3 == c.count(7));
    std::cout << c;
}
#endif
