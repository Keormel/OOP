#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

// Абстрактный класс - "интерфейс" контейнера. Объект такого класса создать нельзя
// (есть чисто виртуальные методы "= 0"), можно только унаследоваться и всё реализовать.
// Зато через указатель abstract_data_t* можно работать с любым наследником,
// а нужная версия метода выберется во время работы программы (виртуальные функции).
class abstract_data_t {
public:
    // методы из предыдущих лабораторных работ как чисто виртуальные
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual ~abstract_data_t() = 0;

    virtual int& front() = 0;                          // первый элемент
    virtual int& back() = 0;                           // последний элемент
    virtual void push(int) = 0;                        // добавить элемент
    virtual void pop() = 0;                            // удалить элемент
    virtual void extend(const abstract_data_t&) = 0;   // добавить все элементы другого контейнера

    // нужны проверочному main: (*v)[i] и *w = *v вызываются через указатель на базовый класс.
    // Оператор присваивания обязательно виртуальный: иначе *w = *v вызвал бы
    // присваивание abstract_data_t, которое ничего не знает о данных наследника.
    virtual int& operator[](size_t) = 0;
    virtual const int& operator[](size_t) const = 0;
    virtual abstract_data_t& operator=(const abstract_data_t&) = 0;

    // Сравнения не виртуальные: их можно написать один раз через size() и [],
    // и они будут работать для любого наследника.

    // равны, если одинаковой длины и элементы на одних и тех же местах совпадают
    friend bool operator==(const abstract_data_t& a, const abstract_data_t& b) {
        if (a.size() != b.size()) {
            return false;
        }
        for (size_t i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const abstract_data_t& a, const abstract_data_t& b) {
        return !(a == b);
    }

    // сравнение "как в словаре": идём по элементам до первого различия;
    // если все общие элементы совпали - меньше тот контейнер, что короче
    friend bool operator<(const abstract_data_t& a, const abstract_data_t& b) {
        size_t common = a.size() < b.size() ? a.size() : b.size();
        for (size_t i = 0; i < common; i++) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return a.size() < b.size();
    }

    // остальные сравнения выражаем через <
    friend bool operator>(const abstract_data_t& a, const abstract_data_t& b) {
        return b < a;
    }

    friend bool operator<=(const abstract_data_t& a, const abstract_data_t& b) {
        return !(b < a);
    }

    friend bool operator>=(const abstract_data_t& a, const abstract_data_t& b) {
        return !(a < b);
    }

    // вывод: std::cout << *v; печатает элементы в том порядке, как они лежат в массиве
    friend std::ostream& operator<<(std::ostream& out, const abstract_data_t& c) {
        for (size_t i = 0; i < c.size(); i++) {
            out << c[i] << " ";
        }
        out << std::endl;
        return out;
    }
};

// Чисто виртуальный деструктор всё равно должен иметь тело:
// при удалении наследника деструктор базового класса вызывается всегда.
inline abstract_data_t::~abstract_data_t() {}

// Итератор по элементам очереди (из лабы 4). Внутри - обычный указатель на int.
class iterator {
public:
    // "паспорт" итератора - эти имена ищут стандартные алгоритмы и std::reverse_iterator
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int*;
    using reference = int&;

    // деструктор - память не наша, освобождать нечего
    ~iterator() {
    }

    // конструктор по умолчанию - итератор никуда не указывает
    iterator() {
        ptr = nullptr;
    }

    // конструктор копирования
    iterator(const iterator& other) {
        ptr = other.ptr;
    }

    // конструктор из указателя
    iterator(int* p) {
        ptr = p;
    }

    iterator& operator=(const iterator& other) {
        ptr = other.ptr;
        return *this;
    }

    // ++it
    iterator& operator++() {
        ++ptr;
        return *this;
    }

    // it++ (возвращает копию "как было до сдвига")
    iterator operator++(int) {
        iterator old(*this);
        ++ptr;
        return old;
    }

    // --it
    iterator& operator--() {
        --ptr;
        return *this;
    }

    // it--
    iterator operator--(int) {
        iterator old(*this);
        --ptr;
        return old;
    }

    int& operator*() const {
        return *ptr;
    }

    int* operator->() const {
        return ptr;
    }

    friend bool operator==(const iterator& a, const iterator& b) {
        return a.ptr == b.ptr;
    }

    friend bool operator!=(const iterator& a, const iterator& b) {
        return !(a == b);
    }

private:
    int* ptr; // на какой элемент сейчас указываем
};

// Очередь с приоритетом из лаб 2-4 (min-куча: сверху самое маленькое число),
// теперь она наследник абстрактного класса abstract_data_t.
// В задании этот класс называется container.
//   front() - верх кучи, т.е. самый маленький элемент
//   back()  - последний элемент массива кучи
//   push()  - добавляет элемент в кучу
//   pop()   - удаляет самый маленький элемент (верх кучи)
class PriorityQueue : public abstract_data_t {
public:
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

    // деструктор - вектор сам освобождает свою память, делать ничего не нужно.
    // override - он переопределяет виртуальный деструктор базового класса,
    // поэтому delete через указатель abstract_data_t* вызовет именно его
    ~PriorityQueue() override {
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
    void push(int value) override {
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
    void pop() override {
        if (data.empty()) {
            return;
        }
        data[0] = data.back(); // последний элемент ставим на место корня
        data.pop_back();
        siftDown(0);
    }

    // проверяет, пустая ли очередь
    bool empty() const override {
        return data.empty();
    }

    // возвращает количество элементов
    size_t size() const override {
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
    // (сравнения и вывод << теперь в базовом классе abstract_data_t)

    // количество элементов (то же, что size())
    size_t length() const {
        return data.size();
    }

    // присваивание из такой же очереди: a = b
    PriorityQueue& operator=(const PriorityQueue& other) {
        if (this == &other) {
            return *this; // a = a - ничего делать не нужно
        }
        data = other.data; // вектор сам освобождает старую память и копирует элементы
        return *this;      // возвращаем себя, чтобы работало a = b = c
    }

    // доступ к элементу массива кучи по индексу (ссылка - можно и менять: w[0] = 0)
    // если записать сюда число, порядок кучи может нарушиться
    int& operator[](size_t index) override {
        return data[index];
    }

    // то же для константной очереди - только чтение
    const int& operator[](size_t index) const override {
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

    // ввод: ss >> a; выбрасывает старое содержимое и читает числа до конца потока
    friend std::istream& operator>>(std::istream& in, PriorityQueue& q) {
        q.data.clear();
        int value;
        while (in >> value) {
            q.push(value);
        }
        return in;
    }

    // ---------- методы из лабораторной работы 4 ----------

    // конструктор из пары итераторов [first, last)
    PriorityQueue(iterator first, iterator last) {
        assign(first, last);
    }

    // то же самое для обратных итераторов
    PriorityQueue(reverse_iterator first, reverse_iterator last) {
        assign(first, last);
    }

    iterator begin() {
        return iterator(data.data());
    }

    iterator end() {
        return iterator(data.data() + data.size());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    // заменяет содержимое очереди элементами из [first, last).
    // Сначала копируем во временный вектор - итераторы могут указывать на эту же очередь.
    void assign(iterator first, iterator last) {
        std::vector<int> values;
        for (iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        fillFrom(values);
    }

    void assign(reverse_iterator first, reverse_iterator last) {
        std::vector<int> values;
        for (reverse_iterator it = first; it != last; ++it) {
            values.push_back(*it);
        }
        fillFrom(values);
    }

    // ---------- новое в лабораторной работе 5 ----------

    // первый элемент = верх кучи (самый маленький).
    // Возвращаем ссылку, поэтому для пустой очереди вернуть нечего - бросаем исключение.
    int& front() override {
        if (data.empty()) {
            throw std::out_of_range("front() у пустой очереди");
        }
        return data.front();
    }

    // последний элемент массива кучи
    int& back() override {
        if (data.empty()) {
            throw std::out_of_range("back() у пустой очереди");
        }
        return data.back();
    }

    // добавляет в очередь все элементы другого контейнера (любого наследника abstract_data_t).
    // Сначала копируем их во временный вектор: other может быть этой же очередью
    // (v->extend(*v)), а push() меняет её размер и порядок элементов.
    void extend(const abstract_data_t& other) override {
        std::vector<int> values;
        for (size_t i = 0; i < other.size(); i++) {
            values.push_back(other[i]);
        }
        for (size_t i = 0; i < values.size(); i++) {
            push(values[i]);
        }
    }

    // присваивание из любого наследника abstract_data_t: *w = *v.
    // Копируем элементы в том же порядке, поэтому после присваивания *w == *v.
    PriorityQueue& operator=(const abstract_data_t& other) override {
        if (this == &other) {
            return *this;
        }
        std::vector<int> values;
        values.reserve(other.size());
        for (size_t i = 0; i < other.size(); i++) {
            values.push_back(other[i]);
        }
        data.swap(values); // старые данные уйдут в values и освободятся
        return *this;
    }
};

// код для проверки правильности выполнения задания
// (container заменён на PriorityQueue)
int main() {
    abstract_data_t *v = new PriorityQueue({2, 3, 5, 7});
    assert(!v->empty());
    v->push(11);
    assert(11 == v->back());
    assert(11 == (*v)[v->size() - 1]);
    v->extend(PriorityQueue({13, 17, 19}));
    assert(8 == v->size());
    abstract_data_t *w = new PriorityQueue();
    assert(w->empty());
    *w = *v;
    assert(*w == *v);
    (*w)[0] = 0;
    assert(0 == w->front());
    w->pop();
    assert(17 == w->back());
    assert(7 == w->size());

    // освобождаем память (через виртуальный деструктор вызовется ~PriorityQueue)
    delete v;
    delete w;
}
