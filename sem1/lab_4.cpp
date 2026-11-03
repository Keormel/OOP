#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

// Итератор по элементам очереди. Внутри - обычный указатель на int в массиве кучи.
// Итератор ничего не владеет: память принадлежит очереди, поэтому деструктор пустой.
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

// Очередь с приоритетом из лабы 3 (min-куча: сверху самое маленькое число),
// к которой добавлены итераторы.
class PriorityQueue {
public:
    // обратный итератор берём из стандартной библиотеки: он обёртывает наш iterator
    // и при ++ вызывает у него --, поэтому идёт от конца к началу
    using reverse_iterator = std::reverse_iterator<iterator>;

private:
    std::vector<int> data; // элементы кучи; память вектор выделяет и освобождает сам

    // поднимает элемент вверх, пока он меньше своего родителя
    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (data[index] >= data[parent]) {
                break;
            }
            int temp = data[index];
            data[index] = data[parent];
            data[parent] = temp;
            index = parent;
        }
    }

    // опускает элемент вниз, пока он больше кого-то из детей
    void siftDown(int index) {
        int len = data.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int smallest = index;

            if (left < len && data[left] < data[smallest]) {
                smallest = left;
            }
            if (right < len && data[right] < data[smallest]) {
                smallest = right;
            }
            if (smallest == index) {
                break;
            }

            int temp = data[index];
            data[index] = data[smallest];
            data[smallest] = temp;
            index = smallest;
        }
    }

    // заменяет содержимое очереди числами из values
    void fillFrom(const std::vector<int>& values) {
        data.clear();
        data.reserve(values.size());
        for (size_t i = 0; i < values.size(); i++) {
            push(values[i]);
        }
    }

public:
    // ---------- методы из лабораторной работы 2 ----------

    // деструктор - вектор сам освобождает свою память, делать ничего не нужно
    ~PriorityQueue() {
    }

    // конструктор по умолчанию - пустая очередь
    PriorityQueue() {
    }

    // конструктор с размером - пустая очередь, но память сразу выделена под n элементов
    PriorityQueue(int n) {
        data.reserve(n);
    }

    // конструктор из списка {5, 1, 3} - кладём элементы по одному
    PriorityQueue(std::initializer_list<int> list) {
        data.reserve(list.size());
        for (int value : list) {
            push(value);
        }
    }

    // конструктор из обычного массива + его размера
    PriorityQueue(int arr[], int n) {
        data.reserve(n);
        for (int i = 0; i < n; i++) {
            push(arr[i]);
        }
    }

    // конструктор копирования - вектор копирует все элементы в свою память
    PriorityQueue(const PriorityQueue& other) {
        data = other.data;
    }

    // конструктор переноса - забираем память вектора у другого объекта
    PriorityQueue(PriorityQueue&& other) {
        data = std::move(other.data);
        other.data.clear();
    }

    // очищает очередь
    void clear() {
        data.clear();
    }

    // добавляет элемент в очередь
    void push(int value) {
        data.push_back(value);
        siftUp(data.size() - 1);
    }

    // возвращает самый маленький элемент (0, если очередь пустая)
    int top() const {
        if (data.empty()) {
            return 0;
        }
        return data[0];
    }

    // удаляет самый маленький элемент
    void pop() {
        if (data.empty()) {
            return;
        }
        data[0] = data.back(); // последний элемент ставим на место корня
        data.pop_back();
        siftDown(0);
    }

    // проверяет, пустая ли очередь
    bool empty() const {
        return data.empty();
    }

    // возвращает количество элементов
    int size() const {
        return data.size();
    }

    // меняет местами содержимое двух очередей (вектора обмениваются памятью, без копирования)
    void swap(PriorityQueue& other) {
        data.swap(other.data);
    }

    // сравнивает очереди как наборы чисел: одинаковые элементы могут лежать
    // в куче в разном порядке, поэтому достаём из копий по одному сверху
    static bool is_equal(const PriorityQueue& a, const PriorityQueue& b) {
        if (a.size() != b.size()) {
            return false;
        }
        PriorityQueue copyA(a);
        PriorityQueue copyB(b);
        while (!copyA.empty()) {
            if (copyA.top() != copyB.top()) {
                return false;
            }
            copyA.pop();
            copyB.pop();
        }
        return true;
    }

    // печатает элементы от меньшего к большему (саму очередь не меняет)
    void print(std::string label) const {
        std::cout << label << ": ";
        if (empty()) {
            std::cout << "(пусто)";
        } else {
            PriorityQueue copy(*this);
            while (!copy.empty()) {
                std::cout << copy.top() << " ";
                copy.pop();
            }
        }
        std::cout << std::endl;
    }

    // ---------- методы из лабораторной работы 3 ----------

    // количество элементов (то же, что size())
    int length() const {
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

    // доступ к элементу массива кучи по индексу (ссылка - можно и менять: b[4] = 0)
    // если записать сюда число, порядок кучи может нарушиться
    int& operator[](size_t index) {
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

    // ---------- новое в лабораторной работе 4 ----------

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
        return iterator(data.data() + data.size());
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
        std::vector<int> values;
        for (iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        fillFrom(values);
    }

    // то же самое для обратных итераторов
    void assign(reverse_iterator first, reverse_iterator last) {
        std::vector<int> values;
        for (reverse_iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        fillFrom(values);
    }
};

// код для проверки правильности выполнения задания
// (abstract_data_t заменён на PriorityQueue)
int main() {
    int array[] = {19, 47, 74, 91};

    PriorityQueue a(array, 4);
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
