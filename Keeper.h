#ifndef KEEPER_H
#define KEEPER_H

#include "Base.h"
#include <iosfwd>

//  онтейнер дл€ хранени€ и обработки наследников Base
class Keeper {
private:
    Base** items;
    int count;
    int capacity;

    void grow();
    void checkIndex(int index) const;

public:
    Keeper();
    explicit Keeper(int initialCapacity);
    Keeper(const Keeper& other);
    ~Keeper();

    Keeper& operator=(const Keeper& other);
    Base& operator[](int index);
    const Base& operator[](int index) const;
    Keeper& operator+=(Base* figure); // то же, что add
    Keeper& operator-=(int index); // то же, что remove

    void add(Base* figure);
    void remove(int index);
    void clear();

    int getCount() const;
    int getCapacity() const;
    Base& get(int index);
    const Base& get(int index) const;

    // ѕолное сохранение и восстановление из текстового файла
    void saveToFile(const char* path) const;
    void loadFromFile(const char* path);
};

std::ostream& operator<<(std::ostream& out, const Keeper& keeper);

#endif