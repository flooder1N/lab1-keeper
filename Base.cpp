#include "Base.h"
#include "Exceptions.h"
#include "StringUtils.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cmath>

Base::Base() : type(nullptr), sizes(nullptr), sizesCount(0) {
    type = copyString("Не задан");
    std::cout << "[Конструктор] Base: без параметров\n";
}

Base::Base(const char* typeName, const double* values, int count)
    : type(nullptr), sizes(nullptr), sizesCount(0) {
    try {
        assignType(typeName);
        assignSizes(values, count);
    }
    catch (...) {
        delete[] type;
        delete[] sizes;
        throw;
    }
    std::cout << "[Конструктор] Base: с параметрами (" << type << ")\n";
}

Base::Base(const Base& other) : type(nullptr), sizes(nullptr), sizesCount(0) {
    try {
        assignType(other.type);
        assignSizes(other.sizes, other.sizesCount);
    }
    catch (...) {
        delete[] type;
        delete[] sizes;
        throw;
    }
    std::cout << "[Конструктор копирования] Base (" << type << ")\n";
}

Base::~Base() {
    std::cout << "[Деструктор] Base (" << type << ")\n";
    delete[] type;
    delete[] sizes;
}

Base& Base::operator=(const Base& other) {
    if (this != &other) {
        assignType(other.type);
        assignSizes(other.sizes, other.sizesCount);
    }
    return *this;
}

bool Base::operator==(const Base& other) const {
    if (std::strcmp(getTag(), other.getTag()) != 0) return false;
    if (std::strcmp(type, other.type) != 0) return false;
    if (sizesCount != other.sizesCount) return false;
    for (int i = 0; i < sizesCount; ++i) {
        if (sizes[i] != other.sizes[i]) return false;
    }
    return true;
}

bool Base::operator!=(const Base& other) const {
    return !(*this == other);
}

bool Base::operator<(const Base& other) const {
    if (std::strcmp(getTag(), other.getTag()) != 0) {
        throw InvalidParamException("Нельзя сравнивать плоскую фигуру с объёмной (разные величины: площадь и объём)");
    }
    return getMeasure() < other.getMeasure();
}

bool Base::operator>(const Base& other) const {
    return other < *this;
}

void Base::assignType(const char* newType) {
    char* copy = copyString(newType);
    delete[] type;
    type = copy;
}

void Base::assignSizes(const double* newSizes, int count) {
    if (count < 0 || (count > 0 && !newSizes)) {
        throw InvalidParamException("Некорректный набор размеров фигуры");
    }
    double* copy = (count > 0) ? new double[count] : nullptr;
    for (int i = 0; i < count; ++i) copy[i] = newSizes[i];
    delete[] sizes;
    sizes = copy;
    sizesCount = count;
}

void Base::checkPositiveSizes() const {
    for (int i = 0; i < sizesCount; ++i) {
        if (!(sizes[i] > 0.0) || !std::isfinite(sizes[i]) || sizes[i] > 1e9) {
            char msg[220];
            std::snprintf(msg, sizeof msg,
                "Размер \"%s\" должен быть числом в диапазоне (0; 1e9], получено: %g",
                getParamLabel(i), sizes[i]);
            throw InvalidParamException(msg);
        }
    }
}

const char* Base::getType() const {
    return type;
}

int Base::getSizesCount() const {
    return sizesCount;
}

double Base::getSize(int index) const {
    if (index < 0 || index >= sizesCount) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Номер размера %d вне диапазона [0, %d]", index, sizesCount - 1);
        throw IndexException(msg);
    }
    return sizes[index];
}

void Base::setSize(int index, double value) {
    if (index < 0 || index >= sizesCount) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Номер размера %d вне диапазона [0, %d]", index, sizesCount - 1);
        throw IndexException(msg);
    }
    double old = sizes[index];
    sizes[index] = value;
    try {
        validate();
    }
    catch (...) {
        sizes[index] = old;   // откат при ошибке
        throw;
    }
}

void Base::setSizes(const double* values, int count) {
    if (count != sizesCount) {
        char msg[160];
        std::snprintf(msg, sizeof msg, "Для этой фигуры нужно ровно %d размеров, передано: %d", sizesCount, count);
        throw InvalidParamException(msg);
    }
    if (!values) {
        throw InvalidParamException("Не передан массив размеров");
    }
    double* backup = new double[count];
    for (int i = 0; i < count; ++i) {
        backup[i] = sizes[i];
        sizes[i] = values[i];
    }
    try {
        validate();
    }
    catch (...) {
        for (int i = 0; i < count; ++i) sizes[i] = backup[i];   // откат при ошибке
        delete[] backup;
        throw;
    }
    delete[] backup;
}

void Base::print(std::ostream& out) const {
    out << "Вид фигуры : " << getKind() << "\n";
    out << "Тип        : " << type << "\n";
    for (int i = 0; i < sizesCount; ++i) {
        out << "  " << getParamLabel(i) << " = " << sizes[i] << "\n";
    }
    out << getMeasureName() << " = " << getMeasure() << "\n";
}

void Base::save(std::ostream& out) const {
    std::streamsize oldPrecision = out.precision(17);
    out << type << ' ' << sizesCount;
    for (int i = 0; i < sizesCount; ++i) {
        out << ' ' << sizes[i];
    }
    out.precision(oldPrecision);
}

void Base::load(std::istream& in) {
    char typeName[64];
    int count = 0;
    if (!(in >> std::setw(64) >> typeName >> count)) {
        throw FileException("Не удалось прочитать тип и количество размеров фигуры");
    }
    if (count < 1 || count > 16) {
        throw FileException("Недопустимое количество размеров фигуры в файле");
    }
    double* values = new double[count];
    try {
        for (int i = 0; i < count; ++i) {
            if (!(in >> values[i])) {
                throw FileException("Не удалось прочитать размер фигуры (ожидалось число)");
            }
        }
        applyLoaded(typeName, values, count);
    }
    catch (...) {
        delete[] values;
        throw;
    }
    delete[] values;
}

std::ostream& operator<<(std::ostream& out, const Base& figure) {
    figure.print(out);
    return out;
}
