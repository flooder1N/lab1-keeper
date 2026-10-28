#include "PlaneFigure.h"
#include "Painter.h"
#include "Exceptions.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace {

const double PI = 3.14159265358979323846;
const double DEFAULT_SIZE = 1.0;

struct TypeInfo {
    const char* name;
    int params;
    const char* labels[3];
};

const TypeInfo TYPES[] = {
    {"Круг",          1, {"радиус"}},
    {"Прямоугольник", 2, {"сторона a", "сторона b"}},
    {"Квадрат",       1, {"сторона a"}},
    {"Треугольник",   3, {"сторона a", "сторона b", "сторона c"}}
};
const int TYPES_N = 4;

void checkTypeIndex(int idx) {
    if (idx < 0 || idx >= TYPES_N) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Неизвестный номер типа плоской фигуры: %d", idx);
        throw IndexException(msg);
    }
}

}

// справочные функции

int PlaneFigure::typesCount() {
    return TYPES_N;
}

const char* PlaneFigure::typeName(int typeIdx) {
    checkTypeIndex(typeIdx);
    return TYPES[typeIdx].name;
}

int PlaneFigure::paramCount(int typeIdx) {
    checkTypeIndex(typeIdx);
    return TYPES[typeIdx].params;
}

const char* PlaneFigure::paramLabel(int typeIdx, int paramIdx) {
    checkTypeIndex(typeIdx);
    if (paramIdx < 0 || paramIdx >= TYPES[typeIdx].params) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Номер параметра %d вне диапазона для типа \"%s\"", paramIdx, TYPES[typeIdx].name);
        throw IndexException(msg);
    }
    return TYPES[typeIdx].labels[paramIdx];
}

int PlaneFigure::findType(const char* name) {
    for (int i = 0; i < TYPES_N; ++i) {
        if (std::strcmp(TYPES[i].name, name) == 0) return i;
    }
    return -1;
}

// конструкторы и деструктор

PlaneFigure::PlaneFigure()
    : Base(typeName(SQUARE), &DEFAULT_SIZE, 1), typeIndex(SQUARE) {
    std::cout << "[Конструктор] PlaneFigure: без параметров (" << type << ")\n";
}

PlaneFigure::PlaneFigure(int typeIdx, const double* values, int count)
    : Base(typeName(typeIdx), values, count), typeIndex(typeIdx) {
    std::cout << "[Конструктор] PlaneFigure: с параметрами (" << type << ")\n";
    if (count != paramCount(typeIdx)) {
        char msg[160];
        std::snprintf(msg, sizeof msg, "Для фигуры \"%s\" нужно размеров: %d, передано: %d",
            typeName(typeIdx), paramCount(typeIdx), count);
        throw InvalidParamException(msg);
    }
    validate();
}

PlaneFigure::PlaneFigure(const PlaneFigure& other)
    : Base(other), typeIndex(other.typeIndex) {
    std::cout << "[Конструктор копирования] PlaneFigure (" << type << ")\n";
}

PlaneFigure::~PlaneFigure() {
    std::cout << "[Деструктор] PlaneFigure (" << type << ")\n";
}

PlaneFigure& PlaneFigure::operator=(const PlaneFigure& other) {
    if (this != &other) {
        Base::operator=(other);
        typeIndex = other.typeIndex;
    }
    return *this;
}

// get и set 

int PlaneFigure::getTypeIndex() const {
    return typeIndex;
}

void PlaneFigure::setFigure(int typeIdx, const double* values, int count) {
    PlaneFigure tmp(typeIdx, values, count);   // проверка параметров, при ошибке объект не меняется
    *this = tmp;
}

double PlaneFigure::getArea() const {
    switch (typeIndex) {
    case CIRCLE:
        return PI * sizes[0] * sizes[0];
    case RECTANGLE:
        return sizes[0] * sizes[1];
    case SQUARE:
        return sizes[0] * sizes[0];
    case TRIANGLE: {
        double p = (sizes[0] + sizes[1] + sizes[2]) / 2.0;
        double s = p * (p - sizes[0]) * (p - sizes[1]) * (p - sizes[2]);
        return std::sqrt(std::max(0.0, s));
    }
    }
    return 0.0;
}

// переопределённые методы

void PlaneFigure::validate() const {
    checkPositiveSizes();
    if (typeIndex == TRIANGLE) {
        double a = sizes[0], b = sizes[1], c = sizes[2];
        if (!(a + b > c && a + c > b && b + c > a)) {
            throw InvalidParamException("Треугольник не существует: сумма двух сторон должна быть больше третьей");
        }
    }
}

void PlaneFigure::applyLoaded(const char* typeNameInFile, const double* values, int count) {
    int idx = findType(typeNameInFile);
    if (idx < 0) {
        throw FileException("В файле указан неизвестный тип плоской фигуры");
    }
    setFigure(idx, values, count);
}

const char* PlaneFigure::getKind() const {
    return "Плоская";
}

const char* PlaneFigure::getTag() const {
    return "PLANE";
}

const char* PlaneFigure::getMeasureName() const {
    return "Площадь";
}

double PlaneFigure::getMeasure() const {
    return getArea();
}

const char* PlaneFigure::getParamLabel(int index) const {
    return paramLabel(typeIndex, index);
}

int PlaneFigure::getTypesCount() const {
    return typesCount();
}

const char* PlaneFigure::getTypeNameAt(int typeIdx) const {
    return typeName(typeIdx);
}

int PlaneFigure::getParamCountOf(int typeIdx) const {
    return paramCount(typeIdx);
}

void PlaneFigure::changeType(int typeIdx, const double* values, int count) {
    setFigure(typeIdx, values, count);
}

void PlaneFigure::draw() const {
    std::cout << "Рисунок (" << type << "):\n";
    switch (typeIndex) {
    case CIRCLE:
        Painter::drawCircle();
        break;
    case RECTANGLE:
        Painter::drawRectangle(sizes[0], sizes[1]);
        break;
    case SQUARE:
        Painter::drawRectangle(sizes[0], sizes[0]);
        break;
    case TRIANGLE:
        // основание это сторона c, высота выражается через площадь
        Painter::drawTriangle(sizes[2], 2.0 * getArea() / sizes[2]);
        break;
    }
}

Base* PlaneFigure::clone() const {
    return new PlaneFigure(*this);
}
