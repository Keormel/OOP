#include <iostream>
#include <initializer_list>
#include <string>

class PriorityQueue {
private:
    int* data;     // массив
    int count;     // сколько чисел в очереди
    int capacity;  // на сколько чисел выделена память

    // увеличивает выделенную память, если места не хватает
    void grow() {
        int newCapacity;
        if (capacity == 0) {
            newCapacity = 4;
        } else {
            newCapacity = capacity * 2;
        }

        int* newData = new int[newCapacity];
        for (int i = 0; i < count; i++) {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

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
    // деструктор - освобождает память
    ~PriorityQueue() {
        delete[] data;
    }

    // конструктор по умолчанию - пустая очередь
    PriorityQueue() {
        data = nullptr;
        count = 0;
        capacity = 0;
    }

    // конструктор с размером - пустая очередь, но память сразу выделена под n элементов
    PriorityQueue(int n) {
        count = 0;
        capacity = n;
        data = new int[capacity];
    }

    // конструктор из списка {5, 1, 3} - кладём элементы по одному
    PriorityQueue(std::initializer_list<int> list) {
        count = 0;
        capacity = list.size();
        data = new int[capacity];
        for (int value : list) {
            push(value);
        }
    }

    // конструктор из обычного массива + его размера
    PriorityQueue(int arr[], int n) {
        count = 0;
        capacity = n;
        data = new int[capacity];
        for (int i = 0; i < n; i++) {
            push(arr[i]);
        }
    }

    // конструктор копирования - делаем свою отдельную копию данных
    PriorityQueue(const PriorityQueue& other) {
        count = other.count;
        capacity = other.capacity;
        data = new int[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    // конструктор переноса - просто забираем указатель у другого объекта
    PriorityQueue(PriorityQueue&& other) {
        data = other.data;
        count = other.count;
        capacity = other.capacity;

        other.data = nullptr;
        other.count = 0;
        other.capacity = 0;
    }

    // очищает очередь
    void clear() {
        delete[] data;
        data = nullptr;
        count = 0;
        capacity = 0;
    }

    // добавляет элемент в очередь
    void push(int value) {
        if (count == capacity) {
            grow();
        }
        data[count] = value;
        count++;
        siftUp(count - 1);
    }

    // возвращает самый большой элемент (0, если очередь пустая)
    int top() {
        if (count == 0) {
            return 0;
        }
        return data[0];
    }

    // удаляет самый большой элемент
    void pop() {
        if (count == 0) {
            return;
        }
        data[0] = data[count - 1]; // последний элемент ставим на место корня
        count--;
        siftDown(0);
    }

    // проверяет, пустая ли очередь
    bool empty() {
        return count == 0;
    }

    // возвращает количество элементов
    int size() {
        return count;
    }

    // меняет местами содержимое двух очередей
    void swap(PriorityQueue& other) {
        int* tempData = data;
        int tempCount = count;
        int tempCapacity = capacity;

        data = other.data;
        count = other.count;
        capacity = other.capacity;

        other.data = tempData;
        other.count = tempCount;
        other.capacity = tempCapacity;
    }

    // сравнивает две очереди на равенство (статический метод)
    // одинаковые элементы могут лежать в куче в разном порядке,
    // поэтому сравниваем копии, доставая элементы по одному сверху
    static bool is_equal(PriorityQueue& a, PriorityQueue& b) {
        if (a.count != b.count) {
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
