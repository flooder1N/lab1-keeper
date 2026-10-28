#include <iostream>
#include <cstdio>
#include <cstring>
#include "Keeper.h"
#include "PlaneFigure.h"
#include "VolumeFigure.h"
#include "Exceptions.h"
#include "Input.h"

#ifdef _WIN32
#include <windows.h>
#endif

using std::cout;

namespace {

    const int PATH_SIZE = 256;

    void printMenu(const Keeper& keeper) {
        cout << "\nХранилище фигур\n";
        cout << "Фигур в хранилище: " << keeper.getCount() << "\n";
        cout << " 1. Добавить плоскую фигуру\n";
        cout << " 2. Добавить объёмную фигуру\n";
        cout << " 3. Удалить фигуру\n";
        cout << " 4. Показать все фигуры\n";
        cout << " 5. Показать одну фигуру (с рисунком)\n";
        cout << " 6. Изменить фигуру (размер / все размеры / тип)\n";
        cout << " 7. Создать копию фигуры (конструктор копирования)\n";
        cout << " 8. Сравнить две фигуры (операторы ==, <, >)\n";
        cout << " 9. Сохранить хранилище в файл\n";
        cout << "10. Загрузить хранилище из файла\n";
        cout << "11. Очистить хранилище\n";
        cout << "12. Создать копию всего хранилища (копирование и оператор =)\n";
        cout << " 0. Выход\n";
    }

    // Запрос номера фигуры у пользователя 
    int readFigureIndex(const Keeper& keeper, const char* prompt) {
        if (keeper.getCount() == 0) {
            throw IndexException("Хранилище пусто: нет фигур для выбора");
        }
        char text[100];
        std::snprintf(text, sizeof text, "%s (1..%d): ", prompt, keeper.getCount());
        return readInt(text, 1, keeper.getCount()) - 1;
    }

    // Ввод размеров для типа typeIdx фигуры
    template <class F>
    double* readSizes(int typeIdx, int& n) {
        n = F::paramCount(typeIdx);
        double* values = new double[n];
        try {
            for (int i = 0; i < n; ++i) {
                char prompt[100];
                std::snprintf(prompt, sizeof prompt, "  %s: ", F::paramLabel(typeIdx, i));
                values[i] = readDouble(prompt);
            }
        }
        catch (...) {
            delete[] values;
            throw;
        }
        return values;
    }

    // Создание фигуры класса F по данным, введённым пользователем
    template <class F>
    F* inputFigure() {
        cout << "\nДоступные типы:\n";
        for (int i = 0; i < F::typesCount(); ++i) {
            cout << "  " << (i + 1) << ". " << F::typeName(i) << "\n";
        }
        int typeIdx = readInt("Выберите тип: ", 1, F::typesCount()) - 1;
        cout << "Введите размеры (тип \"" << F::typeName(typeIdx) << "\"):\n";

        int n = 0;
        double* values = readSizes<F>(typeIdx, n);
        F* figure = nullptr;
        try {
            figure = new F(typeIdx, values, n);
        }
        catch (...) {
            delete[] values;
            throw;
        }
        delete[] values;
        return figure;
    }

    void addPlane(Keeper& keeper) {
        PlaneFigure* figure = inputFigure<PlaneFigure>();
        try {
            keeper += figure;
        }
        catch (...) {
            delete figure;
            throw;
        }
        cout << "Плоская фигура добавлена. Номер в хранилище: " << keeper.getCount() << "\n";
    }

    void addVolume(Keeper& keeper) {
        VolumeFigure* figure = inputFigure<VolumeFigure>();
        try {
            keeper += figure;
        }
        catch (...) {
            delete figure;
            throw;
        }
        cout << "Объёмная фигура добавлена. Номер в хранилище: " << keeper.getCount() << "\n";
    }

    void removeFigure(Keeper& keeper) {
        int idx = readFigureIndex(keeper, "Номер удаляемой фигуры");
        keeper -= idx;
        cout << "Фигура удалена.\n";
    }

    void showAll(const Keeper& keeper) {
        if (keeper.getCount() == 0) {
            cout << "Хранилище пусто.\n";
            return;
        }
        cout << "\n" << keeper;

        int planes = 0, volumes = 0;
        double totalArea = 0.0, totalVolume = 0.0;
        for (int i = 0; i < keeper.getCount(); ++i) {
            if (std::strcmp(keeper[i].getTag(), "PLANE") == 0) {
                ++planes;
                totalArea += keeper[i].getMeasure();
            }
            else {
                ++volumes;
                totalVolume += keeper[i].getMeasure();
            }
        }
        cout << "\nИтого: плоских - " << planes << " (суммарная площадь " << totalArea
            << "), объёмных - " << volumes << " (суммарный объём " << totalVolume << ")\n";
    }

    void showOne(const Keeper& keeper) {
        int idx = readFigureIndex(keeper, "Номер фигуры");
        cout << "\n" << keeper[idx];
        keeper[idx].draw();
    }

    void editFigure(Keeper& keeper) {
        int idx = readFigureIndex(keeper, "Номер изменяемой фигуры");
        Base& figure = keeper[idx];
        cout << "\nТекущая фигура:\n" << figure;
        cout << "\n1. Изменить один размер\n2. Изменить все размеры\n3. Изменить тип фигуры\n0. Отмена\n";
        int choice = readInt("Выбор: ", 0, 3);

        if (choice == 1) {
            for (int i = 0; i < figure.getSizesCount(); ++i) {
                cout << "  " << (i + 1) << ". " << figure.getParamLabel(i) << " (сейчас " << figure.getSize(i) << ")\n";
            }
            int p = readInt("Какой размер изменить: ", 1, figure.getSizesCount()) - 1;
            double value = readDouble("Новое значение: ");
            figure.setSize(p, value);
            cout << "Размер изменён.\n";
        }
        else if (choice == 2) {
            int n = figure.getSizesCount();
            double* values = new double[n];
            try {
                for (int i = 0; i < n; ++i) {
                    char prompt[100];
                    std::snprintf(prompt, sizeof prompt, "  %s: ", figure.getParamLabel(i));
                    values[i] = readDouble(prompt);
                }
                figure.setSizes(values, n);
            }
            catch (...) {
                delete[] values;
                throw;
            }
            delete[] values;
            cout << "Размеры изменены.\n";
        }
        else if (choice == 3) {
            cout << "Доступные типы:\n";
            for (int i = 0; i < figure.getTypesCount(); ++i) {
                cout << "  " << (i + 1) << ". " << figure.getTypeNameAt(i) << "\n";
            }
            int t = readInt("Новый тип: ", 1, figure.getTypesCount()) - 1;
            int n = figure.getParamCountOf(t);
            double* values = new double[n];
            try {
                cout << "Введите размеры (тип \"" << figure.getTypeNameAt(t) << "\"):\n";
                for (int i = 0; i < n; ++i) {
                    char prompt[100];
                    std::snprintf(prompt, sizeof prompt, "  размер %d: ", i + 1);
                    values[i] = readDouble(prompt);
                }
                figure.changeType(t, values, n);
            }
            catch (...) {
                delete[] values;
                throw;
            }
            delete[] values;
            cout << "Тип фигуры изменён.\n";
        }
        else {
            cout << "Изменение отменено.\n";
        }
    }

    void copyFigure(Keeper& keeper) {
        int idx = readFigureIndex(keeper, "Номер копируемой фигуры");
        Base* copy = keeper[idx].clone(); // вызывает конструктор копирования наследника
        try {
            keeper += copy;
        }
        catch (...) {
            delete copy;
            throw;
        }
        cout << "Копия добавлена. Номер копии в хранилище: " << keeper.getCount() << "\n";
    }

    void compareFigures(const Keeper& keeper) {
        int i = readFigureIndex(keeper, "Номер первой фигуры");
        int j = readFigureIndex(keeper, "Номер второй фигуры");
        const Base& a = keeper[i];
        const Base& b = keeper[j];

        cout << "Фигура №" << (i + 1) << " == фигура №" << (j + 1) << ": " << ((a == b) ? "да" : "нет") << "\n";
        if (a < b) {
            cout << "Фигура №" << (i + 1) << " меньше фигуры №" << (j + 1) << " (по " << a.getMeasureName() << ")\n";
        }
        else if (a > b) {
            cout << "Фигура №" << (i + 1) << " больше фигуры №" << (j + 1) << " (по " << a.getMeasureName() << ")\n";
        }
        else {
            cout << "По величине (" << a.getMeasureName() << ") фигуры равны\n";
        }
    }

    void saveKeeper(const Keeper& keeper) {
        char path[PATH_SIZE];
        readText("Имя файла для сохранения: ", path, PATH_SIZE);
        keeper.saveToFile(path);
        cout << "Сохранено фигур: " << keeper.getCount() << " (файл " << path << ")\n";
    }

    void loadKeeper(Keeper& keeper) {
        char path[PATH_SIZE];
        readText("Имя файла для загрузки: ", path, PATH_SIZE);
        keeper.loadFromFile(path);
        cout << "Загружено фигур: " << keeper.getCount() << "\n";
    }

    void clearKeeper(Keeper& keeper) {
        int n = keeper.getCount();
        keeper.clear();
        cout << "Хранилище очищено (удалено фигур: " << n << ").\n";
    }

    void copyKeeper(const Keeper& keeper) {
        cout << "\nСоздаём копию хранилища:\n";
        Keeper copy(keeper);
        cout << "\nСоздаём пустое хранилище и присваиваем ему копию (оператор =):\n";
        Keeper assigned;
        assigned = copy;
        cout << "\nВ копии фигур: " << copy.getCount() << ", в присвоенном хранилище: " << assigned.getCount() << "\n";
        cout << "Копии удаляются при выходе из функции:\n";
    }

    void setupConsole() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
    }

}

int main() {
    setupConsole();

    Keeper keeper;
    bool running = true;

    while (running) {
        printMenu(keeper);
        try {
            int choice = readInt("Ваш выбор: ", 0, 12);
            switch (choice) {
            case 1:  addPlane(keeper); break;
            case 2:  addVolume(keeper); break;
            case 3:  removeFigure(keeper); break;
            case 4:  showAll(keeper); break;
            case 5:  showOne(keeper); break;
            case 6:  editFigure(keeper); break;
            case 7:  copyFigure(keeper); break;
            case 8:  compareFigures(keeper); break;
            case 9:  saveKeeper(keeper); break;
            case 10: loadKeeper(keeper); break;
            case 11: clearKeeper(keeper); break;
            case 12: copyKeeper(keeper); break;
            case 0:  running = false; break;
            }
        }
        catch (const InputException& e) {
            cout << "Ошибка ввода: " << e.what() << "\n";
            if (std::cin.eof()) running = false;
        }
        catch (const FigureException& e) {
            cout << "Ошибка: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            cout << "Системная ошибка: " << e.what() << "\n";
        }
    }

    cout << "Завершение работы программы.\n";
    return 0;
}