#include "Exceptions.h"
#include "StringUtils.h"
#include <iostream>

FigureException::FigureException(const char* msg) : message(copyString(msg)) {
    std::cout << "[Конструктор] FigureException: создано исключение\n";
}

FigureException::FigureException(const FigureException& other)
    : std::exception(other), message(copyString(other.message)) {
    std::cout << "[Конструктор копирования] FigureException\n";
}

FigureException& FigureException::operator=(const FigureException& other) {
    if (this != &other) {
        char* copy = copyString(other.message);
        delete[] message;
        message = copy;
    }
    return *this;
}

FigureException::~FigureException() noexcept {
    std::cout << "[Деструктор] FigureException: исключение уничтожено\n";
    delete[] message;
}

const char* FigureException::what() const noexcept {
    return message;
}
