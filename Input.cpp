#ifndef INPUT_H
#define INPUT_H

// Целое число в диапазоне
int readInt(const char* prompt, int minValue, int maxValue);

// Вещественное число
double readDouble(const char* prompt);

// Непустая строка
void readText(const char* prompt, char* buffer, int size);

#endif
