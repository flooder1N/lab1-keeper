#include "Painter.h"
#include "VolumeFigure.h"
#include "Exceptions.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

    const double PI = 3.14159265358979323846;
    const double DEFAULT_SIZE = 1.0;

    struct TypeInfo {
        const char* name;
        int params;
        const char* labels[3];
        const char* projection; // плоская фигура, которой рисуется объёмная
    };

    const TypeInfo TYPES[] = {
        {"Куб", 1, {"ребро a"}, "Квадрат"},
        {"Параллелепипед", 3, {"длина a", "ширина b", "высота c"}, "Прямоугольник (вид спереди a x c)"},
        {"Шар", 1, {"радиус"}, "Круг"},
        {"Цилиндр", 2, {"радиус", "высота"}, "Прямоугольник (осевое сечение)"},
        {"Конус", 2, {"радиус", "высота"}, "Треугольник (осевое сечение)"}
    };
    const int TYPES_N = 5;

    void checkTypeIndex(int idx) {
        if (idx < 0 || idx >= TYPES_N) {
            char msg[120];
            std::snprintf(msg, sizeof msg, "Неизвестный номер типа объёмной фигуры: %d", idx);
            throw IndexException(msg);
        }
    }

}

// справочные функции 

int VolumeFigure::typesCount() {
    return TYPES_N;
}

const char* VolumeFigure::typeName(int typeIdx) {
    checkTypeIndex(typeIdx);
    return TYPES[typeIdx].name;
}

int VolumeFigure::paramCount(int typeIdx) {
    checkTypeIndex(typeIdx);
    return TYPES[typeIdx].params;
}

const char* VolumeFigure::paramLabel(int typeIdx, int paramIdx) {
    checkTypeIndex(typeIdx);
    if (paramIdx < 0 || paramIdx >= TYPES[typeIdx].params) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Номер параметра %d вне диапазона для типа \"%s\"", paramIdx, TYPES[typeIdx].name);
        throw IndexException(msg);
    }
    return TYPES[typeIdx].labels[paramIdx];
}

const char* VolumeFigure::projectionName(int typeIdx) {
    checkTypeIndex(typeIdx);
    return TYPES[typeIdx].projection;
}

int VolumeFigure::findType(const char* name) {
    for (int i = 0; i < TYPES_N; ++i) {
        if (std::strcmp(TYPES[i].name, name) == 0) return i;
    }
    return -1;
}

// конструкторы и деструктор

VolumeFigure::VolumeFigure()
    : Base(typeName(CUBE), &DEFAULT_SIZE, 1), typeIndex(CUBE) {
    std::cout << "[Конструктор] VolumeFigure: без параметров (" << type << ")\n";
}

VolumeFigure::VolumeFigure(int typeIdx, const double* values, int count)
    : Base(typeName(typeIdx), values, count), typeIndex(typeIdx) {
    std::cout << "[Конструктор] VolumeFigure: с параметрами (" << type << ")\n";
    if (count != paramCount(typeIdx)) {
        char msg[160];
        std::snprintf(msg, sizeof msg, "Для фигуры \"%s\" нужно размеров: %d, передано: %d",
            typeName(typeIdx), paramCount(typeIdx), count);
        throw InvalidParamException(msg);
    }
    validate();
}

VolumeFigure::VolumeFigure(const VolumeFigure& other)
    : Base(other), typeIndex(other.typeIndex) {
    std::cout << "[Конструктор копирования] VolumeFigure (" << type << ")\n";
}

VolumeFigure::~VolumeFigure() {
    std::cout << "[Деструктор] VolumeFigure (" << type << ")\n";
}

VolumeFigure& VolumeFigure::operator=(const VolumeFigure& other) {
    if (this != &other) {
        Base::operator=(other);
        typeIndex = other.typeIndex;
    }
    return *this;
}

// get и set

int VolumeFigure::getTypeIndex() const {
    return typeIndex;
}

void VolumeFigure::setFigure(int typeIdx, const double* values, int count) {
    VolumeFigure tmp(typeIdx, values, count);
    *this = tmp;
}

double VolumeFigure::getVolume() const {
    switch (typeIndex) {
    case CUBE:
        return sizes[0] * sizes[0] * sizes[0];
    case BOX:
        return sizes[0] * sizes[1] * sizes[2];
    case SPHERE:
        return 4.0 / 3.0 * PI * sizes[0] * sizes[0] * sizes[0];
    case CYLINDER:
        return PI * sizes[0] * sizes[0] * sizes[1];
    case CONE:
        return PI * sizes[0] * sizes[0] * sizes[1] / 3.0;
    }
    return 0.0;
}

// переопределённые методы

void VolumeFigure::validate() const {
    checkPositiveSizes();
}

void VolumeFigure::applyLoaded(const char* typeNameInFile, const double* values, int count) {
    int idx = findType(typeNameInFile);
    if (idx < 0) {
        throw FileException("В файле указан неизвестный тип объёмной фигуры");
    }
    setFigure(idx, values, count);
}

const char* VolumeFigure::getKind() const {
    return "Объёмная";
}

const char* VolumeFigure::getTag() const {
    return "VOLUME";
}

const char* VolumeFigure::getMeasureName() const {
    return "Объём";
}

double VolumeFigure::getMeasure() const {
    return getVolume();
}

const char* VolumeFigure::getParamLabel(int index) const {
    return paramLabel(typeIndex, index);
}

int VolumeFigure::getTypesCount() const {
    return typesCount();
}

const char* VolumeFigure::getTypeNameAt(int typeIdx) const {
    return typeName(typeIdx);
}

int VolumeFigure::getParamCountOf(int typeIdx) const {
    return paramCount(typeIdx);
}

void VolumeFigure::changeType(int typeIdx, const double* values, int count) {
    setFigure(typeIdx, values, count);
}

void VolumeFigure::draw() const {
    std::cout << "Рисунок (" << type << " в виде плоской фигуры: " << projectionName(typeIndex) << "):\n";
    switch (typeIndex) {
    case CUBE:
        Painter::drawRectangle(sizes[0], sizes[0]);
        break;
    case BOX:
        Painter::drawRectangle(sizes[0], sizes[2]);
        break;
    case SPHERE:
        Painter::drawCircle();
        break;
    case CYLINDER:
        Painter::drawRectangle(2.0 * sizes[0], sizes[1]);
        break;
    case CONE:
        Painter::drawTriangle(2.0 * sizes[0], sizes[1]);
        break;
    }
}

Base* VolumeFigure::clone() const {
    return new VolumeFigure(*this);
}
