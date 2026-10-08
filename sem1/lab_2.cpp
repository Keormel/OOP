#include <iostream>
#include <initializer_list>
#include <string>
#include <vector>

class PriorityQueue {
private:
    std::vector<int> data; // элементы кучи; память вектор выделяет и освобождает сам

    // поднимает элемент вверх, пока он больше своего родителя
    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (data[index] <= data[parent]) {
                break;
            }
            int temp = data[index];
            data[index] = data[parent];
            data[parent] = temp;
            index = parent;
        }
    }

    // опускает элемент вниз, пока он меньше кого-то из детей
    void siftDown(int index) {
        int count = data.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < count && data[left] > data[largest]) {
                largest = left;
            }
            if (right < count && data[right] > data[largest]) {
                largest = right;
            }
            if (largest == index) {
                break;
            }

            int temp = data[index];
            data[index] = data[largest];
            data[largest] = temp;
            index = largest;
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

    // возвращает самый большой элемент (0, если очередь пустая)
    int top() {
        if (data.empty()) {
            return 0;
        }
        return data[0];
    }

    // удаляет самый большой элемент
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

    // меняет местами содержимое двух очередей (вектора обмениваются памятью, без копирования)
    void swap(PriorityQueue& other) {
        data.swap(other.data);
    }


    static bool is_equal(PriorityQueue& a, PriorityQueue& b) {
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

    // печатает элементы от большего к меньшему (саму очередь не меняет)
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
};

int main() {
    std::cout << "1) Конструктор по умолчанию" << std::endl;
    PriorityQueue a;
    a.print("a");

    std::cout << "\n2) Конструктор с размером" << std::endl;
    PriorityQueue b(5);
    b.print("b (память под 5 элементов, но очередь пустая)");

    std::cout << "\n3) Конструктор из списка" << std::endl;
    PriorityQueue c{3, 1, 4, 1, 5};
    c.print("c");

    std::cout << "\n4) Конструктор из массива" << std::endl;
    int rawArr[] = {20, 10, 30};
    PriorityQueue d(rawArr, 3);
    d.print("d");

    std::cout << "\n5) Конструктор копирования" << std::endl;
    PriorityQueue e(c);
    e.push(99);
    c.print("c (не изменилась)");
    e.print("e (копия, добавили 99)");

    std::cout << "\n6) Конструктор переноса" << std::endl;
    PriorityQueue f(std::move(d));
    f.print("f (получила данные d)");
    d.print("d (теперь пустая)");

    std::cout << "\n7) Методы push() и top()" << std::endl;
    PriorityQueue g;
    g.push(7);
    g.push(2);
    g.push(9);
    g.print("g после push(7), push(2), push(9)");
    std::cout << "g.top() = " << g.top() << " (самый большой)" << std::endl;

    std::cout << "\n8) Метод pop()" << std::endl;
    g.pop();
    g.print("g после pop()");
    std::cout << "g.top() = " << g.top() << std::endl;

    std::cout << "\n9) Метод empty()" << std::endl;
    PriorityQueue emptyQueue;
    std::cout << "emptyQueue.empty() = " << emptyQueue.empty() << std::endl;
    std::cout << "g.empty() = " << g.empty() << std::endl;

    std::cout << "\n10) Метод swap()" << std::endl;
    PriorityQueue s1{1, 1, 1};
    PriorityQueue s2{2, 2, 2, 2};
    s1.print("s1 до swap");
    s2.print("s2 до swap");
    s1.swap(s2);
    s1.print("s1 после swap");
    s2.print("s2 после swap");

    std::cout << "\n11) Метод size()" << std::endl;
    std::cout << "s1.size() = " << s1.size() << std::endl;

    std::cout << "\n12) Метод is_equal()" << std::endl;
    PriorityQueue k1{1, 2, 3};
    PriorityQueue k2{3, 1, 2}; // те же элементы, но добавлены в другом порядке
    PriorityQueue k3{1, 2, 4};
    std::cout << "is_equal(k1, k2) = " << PriorityQueue::is_equal(k1, k2) << std::endl;
    std::cout << "is_equal(k1, k3) = " << PriorityQueue::is_equal(k1, k3) << std::endl;

    std::cout << "\n13) Метод clear()" << std::endl;
    k1.clear();
    k1.print("k1 после clear()");

    return 0;
}
