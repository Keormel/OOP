#include <cassert>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include <string>
#include <vector>

// Очередь с приоритетом. Сверху всегда лежит самое маленькое число (min-куча),
// поэтому числа, добавленные по возрастанию, лежат в массиве в том же порядке.
class PriorityQueue {
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

public:

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
    int top() {
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
    bool empty() {
        return data.empty();
    }

    // возвращает количество элементов
    int size() {
        return data.size();
    }

    // то же самое, что size() - так называется в проверочном main
    int length() {
        return data.size();
    }

    // меняет местами содержимое двух очередей (вектора обмениваются памятью, без копирования)
    void swap(PriorityQueue& other) {
        data.swap(other.data);
    }

    // одинаковые элементы могут лежать в куче в разном порядке
    static bool is_equal(PriorityQueue& a, PriorityQueue& b) {
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
    void print(std::string label) {
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

    // оператор присваивания копированием: a = b
    PriorityQueue& operator=(const PriorityQueue& other) {
        data = other.data; // вектор сам освобождает старую память и копирует элементы
        return *this; // возвращаем себя, чтобы работало a = b = c
    }

    // доступ к элементу массива кучи по индексу (можно и менять: a[0] = 5)
    int& operator[](size_t index) {
        return data[index];
    }

    // есть ли число в очереди
    bool contains(int value) {
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] == value) {
                return true;
            }
        }
        return false;
    }

    // сколько раз число встречается в очереди
    size_t count(int value) {
        size_t result = 0;
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] == value) {
                result++;
            }
        }
        return result;
    }

    // равны, если одинаковой длины и все элементы на своих местах совпадают
    friend bool operator==(const PriorityQueue& a, const PriorityQueue& b) {
        return a.data == b.data;
    }

    friend bool operator!=(const PriorityQueue& a, const PriorityQueue& b) {
        return !(a == b);
    }

    // сравнение как у слов в словаре: идём по элементам до первого различия,
    // если все общие совпали - меньше та, что короче (так сравнивает сам vector)
    friend bool operator<(const PriorityQueue& a, const PriorityQueue& b) {
        return a.data < b.data;
    }

    // остальные сравнения выражаем через < и ==
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

    // ввод: std::cin >> a; читает числа до конца потока и кладёт их в очередь
    friend std::istream& operator>>(std::istream& in, PriorityQueue& q) {
        q.data.clear(); // старое содержимое выбрасываем, память оставляем
        int value;
        while (in >> value) {
            q.push(value);
        }
        return in;
    }
};

// код для проверки правильности выполнения задания:
int main() {
    std::stringstream ss{"1 3 5 7 9"};
    PriorityQueue a(5);
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
