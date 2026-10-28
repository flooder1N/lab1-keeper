#include "Keeper.h"
#include "PlaneFigure.h"
#include "VolumeFigure.h"
#include "Exceptions.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstring>

namespace {
    const int DEFAULT_CAPACITY = 4;
    const int MAX_FILE_FIGURES = 1000000;
}

Keeper::Keeper() : items(nullptr), count(0), capacity(DEFAULT_CAPACITY) {
    items = new Base * [capacity];
    std::cout << "[Конструктор] Keeper: без параметров\n";
}

Keeper::Keeper(int initialCapacity) : items(nullptr), count(0), capacity(initialCapacity) {
    if (initialCapacity < 1) {
        throw InvalidParamException("Начальная вместимость хранилища должна быть не меньше 1");
    }
    items = new Base * [capacity];
    std::cout << "[Конструктор] Keeper: с параметрами (вместимость " << capacity << ")\n";
}

Keeper::Keeper(const Keeper& other)
    : items(nullptr), count(0), capacity(other.capacity) {
    items = new Base * [capacity];
    try {
        for (int i = 0; i < other.count; ++i) {
            items[i] = other.items[i]->clone();
            ++count;
        }
    }
    catch (...) {
        for (int i = 0; i < count; ++i) delete items[i];
        delete[] items;
        throw;
    }
    std::cout << "[Конструктор копирования] Keeper (скопировано фигур: " << count << ")\n";
}

Keeper::~Keeper() {
    std::cout << "[Деструктор] Keeper (в хранилище фигур: " << count << ")\n";
    clear();
    delete[] items;
}

Keeper& Keeper::operator=(const Keeper& other) {
    if (this == &other) return *this;

    Base** copy = new Base * [other.capacity];
    int copied = 0;
    try {
        for (; copied < other.count; ++copied) {
            copy[copied] = other.items[copied]->clone();
        }
    }
    catch (...) {
        for (int i = 0; i < copied; ++i) delete copy[i];
        delete[] copy;
        throw;
    }

    clear();
    delete[] items;
    items = copy;
    count = copied;
    capacity = other.capacity;
    return *this;
}

void Keeper::checkIndex(int index) const {
    if (index < 0 || index >= count) {
        char msg[160];
        if (count == 0) {
            std::snprintf(msg, sizeof msg, "Обращение к элементу %d, но хранилище пусто", index);
        }
        else {
            std::snprintf(msg, sizeof msg, "Индекс %d вне допустимого диапазона [0, %d]", index, count - 1);
        }
        throw IndexException(msg);
    }
}

Base& Keeper::operator[](int index) {
    checkIndex(index);
    return *items[index];
}

const Base& Keeper::operator[](int index) const {
    checkIndex(index);
    return *items[index];
}

Keeper& Keeper::operator+=(Base* figure) {
    add(figure);
    return *this;
}

Keeper& Keeper::operator-=(int index) {
    remove(index);
    return *this;
}

void Keeper::grow() {
    int newCapacity = capacity * 2;
    Base** bigger = new Base * [newCapacity];
    for (int i = 0; i < count; ++i) bigger[i] = items[i];
    delete[] items;
    items = bigger;
    capacity = newCapacity;
}

void Keeper::add(Base* figure) {
    if (!figure) {
        throw InvalidParamException("Нельзя добавить в хранилище пустой указатель");
    }
    if (count == capacity) grow();
    items[count++] = figure;
}

void Keeper::remove(int index) {
    checkIndex(index);
    delete items[index];
    for (int i = index; i < count - 1; ++i) items[i] = items[i + 1];
    --count;
}

void Keeper::clear() {
    for (int i = 0; i < count; ++i) delete items[i];
    count = 0;
}

int Keeper::getCount() const {
    return count;
}

int Keeper::getCapacity() const {
    return capacity;
}

Base& Keeper::get(int index) {
    return (*this)[index];
}

const Base& Keeper::get(int index) const {
    return (*this)[index];
}

void Keeper::saveToFile(const char* path) const {
    if (!path || !*path) {
        throw FileException("Не указано имя файла");
    }
    std::ofstream out(path);
    if (!out) {
        throw FileException("Не удалось открыть файл для записи");
    }
    out << "KEEPER 1\n" << count << '\n';
    for (int i = 0; i < count; ++i) {
        out << items[i]->getTag() << ' ';
        items[i]->save(out);
        out << '\n';
    }
    out.flush();
    if (!out) {
        throw FileException("Ошибка записи в файл");
    }
}

void Keeper::loadFromFile(const char* path) {
    if (!path || !*path) {
        throw FileException("Не указано имя файла");
    }
    std::ifstream in(path);
    if (!in) {
        throw FileException("Не удалось открыть файл для чтения (файл не найден?)");
    }

    char header[16];
    int version = 0;
    int n = 0;
    if (!(in >> std::setw(16) >> header >> version) || std::strcmp(header, "KEEPER") != 0 || version != 1) {
        throw FileException("Неверный формат файла: нет заголовка \"KEEPER 1\"");
    }
    if (!(in >> n) || n < 0 || n > MAX_FILE_FIGURES) {
        throw FileException("Неверный формат файла: некорректное количество фигур");
    }

    // если файл повреждён, текущее содержимое хранилища не теряется
    Base** loaded = new Base * [n > 0 ? n : 1];
    int loadedCount = 0;
    try {
        while (loadedCount < n) {
            char tag[16];
            if (!(in >> std::setw(16) >> tag)) {
                throw FileException("Неожиданный конец файла: фигур меньше, чем указано");
            }
            Base* figure = nullptr;
            if (std::strcmp(tag, "PLANE") == 0) {
                figure = new PlaneFigure();
            }
            else if (std::strcmp(tag, "VOLUME") == 0) {
                figure = new VolumeFigure();
            }
            else {
                throw FileException("В файле встретилась неизвестная метка вида фигуры");
            }
            try {
                figure->load(in);
            }
            catch (const FileException&) {
                delete figure;
                throw;
            }
            catch (const FigureException& e) {
                delete figure;
                char msg[300];
                std::snprintf(msg, sizeof msg, "Некорректные данные фигуры в файле: %s", e.what());
                throw FileException(msg);
            }
            catch (...) {
                delete figure;
                throw;
            }
            loaded[loadedCount++] = figure;
        }
    }
    catch (...) {
        for (int i = 0; i < loadedCount; ++i) delete loaded[i];
        delete[] loaded;
        throw;
    }

    clear();
    delete[] items;
    items = loaded;
    count = loadedCount;
    capacity = (n > 0) ? n : 1;
}

std::ostream& operator<<(std::ostream& out, const Keeper& keeper) {
    for (int i = 0; i < keeper.getCount(); ++i) {
        out << "--- Фигура №" << (i + 1) << " ---\n" << keeper[i];
    }
    return out;
}
