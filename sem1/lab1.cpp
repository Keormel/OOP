#include <iostream>
#include <string>
#include <cassert>

using namespace std;

class Student
{
private:
    string name;
    int grade;

public:
    
    Student()
    {
        name = "Unknown";
        grade = 0;
    }

    
    void setData(string studentName, int studentGrade)
    {
        name = studentName;
        grade = studentGrade;
    }

    string getName()
    {
        return name;
    }

    int getGrade()
    {
        return grade;
    }

    void print()
    {
        cout << "Student: " << name << endl;
        cout << "Grade: " << grade << endl;
    }

    void clear()
    {
        name = "Unknown";
        grade = 0;
    }
};

int main()
{
    Student student;

    assert(student.getName() == "Unknown");
    assert(student.getGrade() == 0);

    student.setData("Ivan", 5);

    assert(student.getName() == "Ivan");
    assert(student.getGrade() == 5);

    student.print();

    student.clear();

    assert(student.getName() == "Unknown");
    assert(student.getGrade() == 0);

    cout << "All operations work correctly!" << endl;

    return 0;
}