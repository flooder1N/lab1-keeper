#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

// Базовое исключение. Хранит подсказку в динамической памяти.
class FigureException : public std::exception {
protected:
    char* message;

public:
    explicit FigureException(const char* msg = "Неизвестная ошибка");
    FigureException(const FigureException& other);
    FigureException& operator=(const FigureException& other);
    virtual ~FigureException() noexcept;

    virtual const char* what() const noexcept;
};

// Недопустимые параметры фигуры
class InvalidParamException : public FigureException {
public:
    explicit InvalidParamException(const char* msg) : FigureException(msg) {}
};

// Ошибки с индексами
class IndexException : public FigureException {
public:
    explicit IndexException(const char* msg) : FigureException(msg) {}
};

// Ошибки с файлом
class FileException : public FigureException {
public:
    explicit FileException(const char* msg) : FigureException(msg) {}
};

// Ошибки пользователя в вводе 
class InputException : public FigureException {
public:
    explicit InputException(const char* msg) : FigureException(msg) {}
};

#endif
