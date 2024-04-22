#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using namespace std;

class StudentDecorator { 
public:
    virtual string getDescription() const = 0;
    virtual ~StudentDecorator() {}
};

class Student {
public:
    string lastName;
    string firstName;
    string group;
    int studentID;
    StudentDecorator* decorator = nullptr;

    Student() : lastName(""), firstName(""), group(""), studentID(0) {}

    Student(const string& lastName, const string& firstName, const string& group, int studentID)
        : lastName(lastName), firstName(firstName), group(group), studentID(studentID) {}

    string getFullName() const {
        return firstName + " " + lastName;
    }

    void setDecorator(StudentDecorator* newDecorator) {
        if (decorator) {
            delete decorator;
            decorator = nullptr; 
        }
        decorator = newDecorator;
    }

    string getDescription() const {
        if (decorator) {
            return decorator->getDescription();
        }
        return "No decorator";
    }
};


class FreshmanDecorator : public StudentDecorator { 
private:
    const Student& student;

public:
    FreshmanDecorator(const Student& student) : student(student) {}

    string getDescription() const override {
        return student.getFullName() + " - первокурсник " + student.group;
    }
};

class SophomoreDecorator : public StudentDecorator { 
private:
    const Student& student;

public:
    SophomoreDecorator(const Student& student) : student(student) {}

    string getDescription() const override {
        return student.getFullName() + " - второкурсник " + student.group;
    }
};

class BinaryTree { 
private:
    struct Node {
        Student data;
        Node* left;
        Node* right;

        Node(const Student& student) : data(student), left(nullptr), right(nullptr) {}
        ~Node() {
            delete left;
            delete right;
        }
    };

    Node* root;

    void addNode(Node*& node, const Student& student) {
        if (node == nullptr) {
            node = new Node(student);
        }
        else {
            if (student.lastName + to_string(student.studentID) < node->data.lastName + to_string(node->data.studentID)) {
                addNode(node->left, student);
            }
            else {
                addNode(node->right, student);
            }
        }
    }

    void deleteNode(Node*& node, const string& lastName, int studentID) {
        if (node == nullptr) {
            throw invalid_argument("Student not found");
        }
        if (lastName + to_string(studentID) < node->data.lastName + to_string(node->data.studentID)) {
            deleteNode(node->left, lastName, studentID);
        }
        else if (lastName + to_string(studentID) > node->data.lastName + to_string(node->data.studentID)) {
            deleteNode(node->right, lastName, studentID);
        }
        else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                node = nullptr;
            }
            else if (node->left == nullptr) {
                Node* temp = node;
                node = node->right;
                delete temp;
            }
            else if (node->right == nullptr) {
                Node* temp = node;
                node = node->left;
                delete temp;
            }
            else {
                Node* temp = findMin(node->right);
                node->data = temp->data;
                deleteNode(node->right, temp->data.lastName, temp->data.studentID);
            }
        }
    }

    Node* findMin(Node* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    bool searchNode(Node* node, const string& lastName, int studentID) {
        if (node == nullptr) {
            return false;
        }
        if (lastName + to_string(studentID) < node->data.lastName + to_string(node->data.studentID)) {
            return searchNode(node->left, lastName, studentID);
        }
        else if (lastName + to_string(studentID) > node->data.lastName + to_string(node->data.studentID)) {
            return searchNode(node->right, lastName, studentID);
        }
        else {
            return true;
        }
    }

    void getAllStudents(Node* node, vector<Student>& students) {
        if (node != nullptr) {
            getAllStudents(node->left, students);
            students.push_back(node->data);
            getAllStudents(node->right, students);
        }
    }

public:
    BinaryTree() : root(nullptr) {}

    void addStudent(const Student& student) {
        addNode(root, student);
    }

    void removeStudent(const string& lastName, int studentID) {
        try {
            deleteNode(root, lastName, studentID);
        }
        catch (const invalid_argument& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    bool searchStudent(const string& lastName, int studentID) {
        return searchNode(root, lastName, studentID);
    }

    vector<Student> getAllStudents() {
        vector<Student> students;
        getAllStudents(root, students);
        return students;
    }

    ~BinaryTree() {
        clear(root);
    }

private:
    void clear(Node* node) {
        if (node == nullptr) {
            return;
        }

        clear(node->left);
        clear(node->right);
        delete node;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    BinaryTree studentTree;

    studentTree.addStudent(Student("Ivanov", "Ivan", "TMsp-100", 1234));
    studentTree.addStudent(Student("Petrov", "Egor", "TRsp-322", 4321));
    studentTree.addStudent(Student("Sidorov", "Sidor", "TMsp-100", 9876));
    studentTree.addStudent(Student("Ivanov", "Petr", "PKsp-121", 2468));

    while (true) {
        cout << "Выберите операцию:\n"
            << "1. Добавить студента\n"
            << "2. Удалить студента\n"
            << "3. Проверить наличие студента\n"
            << "4. Вывести список всех студентов\n"
            << "0. Выйти\n";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            string lastName, firstName, group;
            int studentID;

            cout << "Введите фамилию: ";
            cin >> lastName;
            cout << "Введите имя: ";
            cin >> firstName;
            cout << "Введите группу: ";
            cin >> group;
            cout << "Введите номер зачетной книжки: ";
            cin >> studentID;

            Student newStudent(lastName, firstName, group, studentID);

            cout << "Выберите категорию для студента:\n"
                << "a. Первокурсник\n"
                << "b. Второкурсник\n";
            char category;
            cin >> category;

            switch (category) {
            case 'a':
                newStudent.setDecorator(new FreshmanDecorator(newStudent));
                break;
            case 'b':
                newStudent.setDecorator(new SophomoreDecorator(newStudent));
                break;
            default:
                cout << "Некорректная категория. Студент добавлен без декоратора.\n";
            }

            studentTree.addStudent(newStudent);
            cout << "Студент добавлен.\n";
            break;
        }
        case 2: {
            string lastName;
            int studentID;
            cout << "Введите фамилию и номер зачетной книжки студента для удаления: ";
            cin >> lastName >> studentID;
            studentTree.removeStudent(lastName, studentID);
            cout << "Студент удален.\n";
            break;
        }
        case 3: {
            string lastName;
            int studentID;
            cout << "Введите фамилию и номер зачетной книжки студента для проверки наличия: ";
            cin >> lastName >> studentID;
            if (studentTree.searchStudent(lastName, studentID)) {
                cout << "Студент найден!\n";
            }
            else {
                cout << "Студент не найден.\n";
            }
            break;
        }
        case 4: {
            vector<Student> allStudents = studentTree.getAllStudents();
            cout << "Список всех студентов:\n";
            for (const auto& student : allStudents) {
                cout << "Фамилия: " << student.lastName << ", Имя: " << student.firstName << ", Группа: " << student.group
                    << ", Зачетка: " << student.studentID << ", Описание: ";
                if (student.decorator) {
                    cout << student.getDescription() << endl;
                }
                else {
                    cout << "Нет информации о курсе" << endl;
                }
            }
            break;
        }
        case 0:
            cout << "Выход.\n";
            return 0;
        default:
            cout << "Некорректный ввод. Пожалуйста, выберите существующую операцию.\n";
        }
    }

    return 0;
}
