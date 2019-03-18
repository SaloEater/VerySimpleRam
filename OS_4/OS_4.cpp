// OS_4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>  

using namespace std;

class Interval {

private:
    int start,
        length,
        end;

public:

    Interval(int start_, int length_)
    {
        start = start_;
        length = length_;
        end = start_ + length_;
    }

    int getStart()
    {
        return start;
    }

    int getEnd()
    {
        return end;
    }

    int getLength()
    {
        return length;
    }

    void setStart(int start_)
    {
        start = start_;
    }

    void setLength(int length_)
    {
        length = length_;
        end = start + length;
    }

    void addLength(int some)
    {
        length += some;
        end += some;
    }

    void moveStartOn(int some)
    {
        start += some;
        length -= some;
    }
};

class FreeInterval : public Interval {
public:
    FreeInterval(int start_, int length_) : Interval(start_, length_)
    {

    }
};

class BusyInterval : public Interval {
public:
    BusyInterval(int start_, int length_) : Interval(start_, length_)
    {

    }
};

class RAM {

private:
    vector<FreeInterval*> freeMemory;
    vector<BusyInterval*> busyMemory;

    int size;

    template <typename T, typename A>
    vector<T, A> sortByVar(vector<T, A> items)
    {
        int size = items.size();
        for (int i = 0; i < size - 1; i++) {
            for (int j = size-1; j > i; j--) {
                if (items.at(j - 1)->getStart() > items.at(j)->getStart()) {
                    T interval = items.at(j - 1);
                    items.at(j - 1) = items.at(j);
                    items.at(j) = interval;
                }
            }
        }
        return items;
    }

    bool freeIntervalAt(int position)
    {
        BusyInterval* interval = busyMemory.at(position);

        //Попытаемся найти интервал для склейки
        int start = interval->getStart();
        int size = freeMemory.size();
        bool glued = false;

        for (int i = 0; i < size; i++) {
            if (freeMemory.at(i)->getEnd() == start) {
                freeMemory.at(i)->addLength(interval->getLength());
                glued = true;
                break;
            }
        }

        if (!glued) {
            FreeInterval* newInterval = new FreeInterval(interval->getStart(), interval->getLength());
            freeMemory.push_back(newInterval);
        }

        freeMemory = sortByVar(freeMemory);
        size = freeMemory.size();

        for (int i = 0; i < size - 1; i++) {
            if (freeMemory.at(i)->getEnd() == freeMemory.at(i+1)->getStart()) {
                freeMemory.at(i)->addLength(freeMemory.at(i+1)->getLength());
                freeMemory.erase(freeMemory.begin() + i+1);
                i--;
                size--;
            }
        }

        busyMemory.erase(busyMemory.begin() + position);
        return true;
    }

    bool takeInterval(int length)
    {
        int size = freeMemory.size();
        int closest = 0,
            closestIndex = -1;
        for (int i = 0; i < size; i++) {
            int len_ = freeMemory.at(i)->getLength();
            if (len_ >= length && (len_ < closest || closestIndex == -1)) {
                closest = len_;
                closestIndex = i;
            }
        }
        if (closestIndex == -1) {
            return false;
        }

        FreeInterval* suitableInterval = freeMemory.at(closestIndex);

        int start = suitableInterval->getStart();

        BusyInterval* busyInterval = new BusyInterval(start, length);

        suitableInterval->moveStartOn(length);

        busyMemory.push_back(busyInterval);

        freeMemory = sortByVar(freeMemory);
        busyMemory = sortByVar(busyMemory);
        return true;
    }

    int printMenu()
    {
        cout << "1) Состояние памяти" << endl
            << "2) Выделить память" << endl
            << "3) Освободить память" << endl
            << "0) Выход" << endl
            << "Ваш выбор: ";
        int menuPoint = -1;
        cin >> menuPoint;
        return menuPoint;
    }

    void printMemoryState()
    {
        cout << "Графическое изображение" << endl;
        int freeIndex = 0,
            busyIndex = 0,
            freeSize = freeMemory.size(),
            busySize = busyMemory.size();

        Interval* freeI = NULL;
        Interval* busyI = NULL;
        cout << "(Начало, длина, конец)" << endl;
        do {
            if (busyIndex == busySize) {
                printInterval(freeMemory[freeIndex++], 0);
            }
            else if (freeIndex == freeSize) {
                printInterval(busyMemory[busyIndex++], 1);
            }
            else {
                freeI = freeMemory.at(freeIndex);
                busyI = busyMemory.at(busyIndex);
                if (freeI->getStart() > busyI->getStart()) {

                    printInterval(busyI, 1);
                    busyIndex++;
                }
                else {
                    printInterval(freeI, 0);
                    freeIndex++;
                }
            }

        } while (freeIndex != freeSize && freeIndex != busySize);
        /*printFreeMemoryState();
        printBusyMemoryState();*/
    }

    void printInterval(Interval* interval, int subType) {
        cout << setw(6) << (subType ? "Занятый " : "Свободный ")
            << '(' << interval->getStart() << ", "
            << interval->getLength() << ", " 
            << interval->getEnd() << ')' << endl;
    }

    void printVisual()
    {
        
    }

    void printFreeMemoryState()
    {
        if (freeMemory.size() == 0) {
            cout << "В памяти не остальнось свободного места!" << endl;
            return;
        }
        cout << "Свободная память: " << endl;
        cout << "№) " << "начало " <<  "длина" << endl;
        int size = freeMemory.size();
        for (int i = 0; i < size; i++) {
            cout << i + 1 << ") " << setw(6) << freeMemory.at(i)->getStart() 
                << " " << freeMemory.at(i)->getLength() << endl;
        }
        setw(0);
    }

    void printBusyMemoryState()
    {
        if (busyMemory.size() == 0) {
            cout << "Память пуста!" << endl;
            return;
        }
        cout << "Занятая память: " << endl;
        cout << "№) " << "начало " << "длина" << endl;
        int size = busyMemory.size();
        for (int i = 0; i < size; i++) {
            cout << i + 1 << ") " << setw(6) << busyMemory.at(i)->getStart() 
                << " " << busyMemory.at(i)->getLength() << endl;
        }
        setw(0);
    }

public:

    RAM(int size_)
    {
        size = size_;
    }

    ~RAM()
    {
        freeMemory.clear();
        busyMemory.clear();
    }

    void turnOn()
    {
        int menuPoint;

        freeMemory.push_back(new FreeInterval(0, size));

        bool stop = false;

        do {
            system("cls");
            menuPoint = printMenu();
            switch (menuPoint) {
                case 1: {
                    system("cls");
                    printMemoryState();
                    system("pause");
                } 
                break;

                case 2: {
                    system("cls");
                    printFreeMemoryState();
                    int length;
                    do {
                        cout << "Введите длину участка: ", cin >> length;
                    } while (length <= 0);
                    if (!takeInterval(length)) {
                        cout << "Произошла ошибка, длина участка слишком большая!" << endl;
                        system("pause");
                    }
                    else {
                        cout << "Память успешно выделена!" << endl;
                        system("pause");
                    }
                }
                break;

                case 3: {
                    system("cls");
                    printBusyMemoryState();
                    int amount = busyMemory.size();
                    int index;
                    do {
                        cout << "Выберите участок для очищения: ", cin >> index;
                    } while (index <= 0 || index > amount);
                    if (!freeIntervalAt(index - 1)) {
                        cout << "Произошла ошибка!" << endl;
                        system("pause");
                    }
                    else {
                        cout << "Участок освобожден!" << endl;
                        system("pause");
                    }
                }
                break;

                case 0: {
                    system("cls");
                    cout << "Выключение...";
                    for(auto interval : freeMemory) {
                        free(interval);
                    }
                    freeMemory.clear();
                    for (auto interval : busyMemory) {
                        free(interval);
                    }
                    busyMemory.clear();
                    return;
                }
                break;

                default:
                    system("cls");
                    break;
            }
        } while (!stop);
    }
};

int main()
{
    system("chcp 1251");
    int size;
    cout << "Укажите размер памяти: ", cin >> size;
    RAM ram(size);
    ram.turnOn();
    
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
