// Лабораторная работа 2. Конструкторы, деструктор.
// Класс MyArray - простой динамический массив целых чисел.

#include <iostream>
#include <initializer_list>

class MyArray {
private:
    int* data;   // указатель на массив в куче
    int count;   // сколько элементов сейчас в массиве

public:
    // деструктор - освобождает память
    ~MyArray() {
        delete[] data;
    }

    // конструктор по умолчанию - пустой массив
    MyArray() {
        data = nullptr;
        count = 0;
    }

    // конструктор с размером - создаёт массив из n нулей
    MyArray(int n) {
        count = n;
        data = new int[count];
        for (int i = 0; i < count; i++) {
            data[i] = 0;
        }
    }

    // конструктор из списка {1, 2, 3}
    MyArray(std::initializer_list<int> list) {
        count = list.size();
        data = new int[count];
        int i = 0;
        for (int value : list) {
            data[i] = value;
            i++;
        }
    }

    // конструктор из обычного массива + его размера
    MyArray(int arr[], int n) {
        count = n;
        data = new int[count];
        for (int i = 0; i < count; i++) {
            data[i] = arr[i];
        }
    }

    // конструктор копирования - делаем свою отдельную копию данных
    MyArray(const MyArray& other) {
        count = other.count;
        data = new int[count];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    // конструктор переноса - просто забираем указатель у другого объекта
    MyArray(MyArray&& other) {
        data = other.data;
        count = other.count;

        other.data = nullptr;
        other.count = 0;
    }

    // очищает массив
    void clear() {
        delete[] data;
        data = nullptr;
        count = 0;
    }

    // возвращает элемент по индексу (индекс может быть и отрицательным)
    int at(int index) {
        if (count == 0) {
            return 0;
        }

        if (index < 0) {
            index = count + index; // переводим отрицательный индекс в обычный
        }

        if (index < 0) {
            index = 0; // всё ещё меньше нуля - берём первый элемент
        }

        if (index >= count) {
            index = count - 1; // слишком большой индекс - берём последний элемент
        }

        return data[index];
    }

    // меняет размер массива
    void resize(int newCount) {
        int* newData = new int[newCount];

        for (int i = 0; i < newCount; i++) {
            if (i < count) {
                newData[i] = data[i]; // старые значения переносим
            } else {
                newData[i] = 0; // новые места заполняем нулями
            }
        }

        delete[] data;
        data = newData;
        count = newCount;
    }

    // заменяет содержимое на count одинаковых элементов
    void assign(int newCount, int value) {
        delete[] data;
        count = newCount;
        data = new int[count];
        for (int i = 0; i < count; i++) {
            data[i] = value;
        }
    }

    // проверяет, пустой ли массив
    bool empty() {
        return count == 0;
    }

    // меняет местами содержимое двух массивов
    void swap(MyArray& other) {
        int* tempData = data;
        int tempCount = count;

        data = other.data;
        count = other.count;

        other.data = tempData;
        other.count = tempCount;
    }

    // возвращает размер массива
    int size() {
        return count;
    }

    // сравнивает два массива на равенство (статический метод)
    static bool is_equal(MyArray& a, MyArray& b) {
        if (a.count != b.count) {
            return false;
        }
        for (int i = 0; i < a.count; i++) {
            if (a.data[i] != b.data[i]) {
                return false;
            }
        }
        return true;
    }

    // печатает содержимое массива (для наглядности в main)
    void print(std::string label) {
        std::cout << label << ": ";
        if (empty()) {
            std::cout << "(пусто)";
        } else {
            for (int i = 0; i < count; i++) {
                std::cout << data[i] << " ";
            }
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "1) Конструктор по умолчанию" << std::endl;
    MyArray a;
    a.print("a");

    std::cout << "\n2) Конструктор с размером" << std::endl;
    MyArray b(5);
    b.print("b");

    std::cout << "\n3) Конструктор из списка" << std::endl;
    MyArray c{1, 2, 3, 4, 5};
    c.print("c");

    std::cout << "\n4) Конструктор из массива" << std::endl;
    int rawArr[] = {10, 20, 30};
    MyArray d(rawArr, 3);
    d.print("d");

    std::cout << "\n5) Конструктор копирования" << std::endl;
    MyArray e(c);
    e.assign(3, 99);
    c.print("c (не изменился)");
    e.print("e (копия, изменена)");

    std::cout << "\n6) Конструктор переноса" << std::endl;
    MyArray f(std::move(d));
    f.print("f (получил данные d)");
    d.print("d (теперь пустой)");

    std::cout << "\n7) Метод at()" << std::endl;
    std::cout << "c.at(0) = " << c.at(0) << std::endl;
    std::cout << "c.at(-1) = " << c.at(-1) << " (последний)" << std::endl;
    std::cout << "c.at(100) = " << c.at(100) << " (прижали к последнему)" << std::endl;

    std::cout << "\n8) Метод resize()" << std::endl;
    MyArray g{1, 2, 3};
    g.print("g до resize(5)");
    g.resize(5);
    g.print("g после resize(5)");

    std::cout << "\n9) Метод assign()" << std::endl;
    MyArray h;
    h.assign(4, 7);
    h.print("h после assign(4, 7)");

    std::cout << "\n10) Метод empty()" << std::endl;
    MyArray emptyArr;
    std::cout << "emptyArr.empty() = " << emptyArr.empty() << std::endl;
    std::cout << "h.empty() = " << h.empty() << std::endl;

    std::cout << "\n11) Метод swap()" << std::endl;
    MyArray s1{1, 1, 1};
    MyArray s2{2, 2, 2, 2};
    s1.print("s1 до swap");
    s2.print("s2 до swap");
    s1.swap(s2);
    s1.print("s1 после swap");
    s2.print("s2 после swap");

    std::cout << "\n12) Метод size()" << std::endl;
    std::cout << "s1.size() = " << s1.size() << std::endl;

    std::cout << "\n13) Метод is_equal()" << std::endl;
    MyArray k1{1, 2, 3};
    MyArray k2{1, 2, 3};
    MyArray k3{1, 2, 4};
    std::cout << "is_equal(k1, k2) = " << MyArray::is_equal(k1, k2) << std::endl;
    std::cout << "is_equal(k1, k3) = " << MyArray::is_equal(k1, k3) << std::endl;

    std::cout << "\n14) Метод clear()" << std::endl;
    k1.clear();
    k1.print("k1 после clear()");

    return 0;
}