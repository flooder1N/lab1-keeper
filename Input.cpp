#include "Input.h"
#include "Exceptions.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cmath>
#include <cctype>

namespace {

    const int LINE_SIZE = 256;

    void readRawLine(const char* prompt, char* buffer, int size) {
        std::cout << prompt;
        std::cout.flush();
        if (!std::cin.getline(buffer, size)) {
            if (std::cin.eof()) {
                throw InputException("Достигнут конец ввода");
            }
            std::cin.clear();
            std::cin.ignore(100000, '\n');
            throw InputException("Слишком длинная строка ввода");
        }
    }

    bool onlySpaces(const char* p) {
        while (*p) {
            if (!std::isspace(static_cast<unsigned char>(*p))) return false;
            ++p;
        }
        return true;
    }

}

int readInt(const char* prompt, int minValue, int maxValue) {
    char buffer[LINE_SIZE];
    readRawLine(prompt, buffer, LINE_SIZE);

    char* end = nullptr;
    errno = 0;
    long value = std::strtol(buffer, &end, 10);
    if (end == buffer || errno == ERANGE) {
        throw InputException("Введено не целое число");
    }
    if (!onlySpaces(end)) {
        throw InputException("После числа обнаружены лишние символы");
    }
    if (value < minValue || value > maxValue) {
        char msg[120];
        std::snprintf(msg, sizeof msg, "Число должно быть в диапазоне от %d до %d", minValue, maxValue);
        throw InputException(msg);
    }
    return static_cast<int>(value);
}

double readDouble(const char* prompt) {
    char buffer[LINE_SIZE];
    readRawLine(prompt, buffer, LINE_SIZE);

    for (char* p = buffer; *p; ++p) {
        if (*p == ',') *p = '.';
    }

    char* end = nullptr;
    errno = 0;
    double value = std::strtod(buffer, &end);
    if (end == buffer || errno == ERANGE) {
        throw InputException("Введено не число");
    }
    if (!onlySpaces(end)) {
        throw InputException("После числа обнаружены лишние символы");
    }
    if (!std::isfinite(value)) {
        throw InputException("Число должно быть конечным");
    }
    return value;
}

void readText(const char* prompt, char* buffer, int size) {
    char line[LINE_SIZE];
    readRawLine(prompt, line, LINE_SIZE);

    // обрезаем пробелы по краям
    int start = 0;
    while (line[start] && std::isspace(static_cast<unsigned char>(line[start]))) ++start;
    int len = 0;
    while (line[start + len]) ++len;
    while (len > 0 && std::isspace(static_cast<unsigned char>(line[start + len - 1]))) --len;

    if (len == 0) {
        throw InputException("Строка не должна быть пустой");
    }
    if (len >= size) {
        throw InputException("Строка слишком длинная");
    }
    for (int i = 0; i < len; ++i) buffer[i] = line[start + i];
    buffer[len] = '\0';
}
