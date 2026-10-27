#ifndef BASE_H
#define BASE_H

#include <iosfwd>

class Base {
protected:
    char* type;
    double* sizes;
    int sizesCount;

    void assignType(const char* newType);
    void assignSizes(const double* newSizes, int count);
    void checkPositiveSizes() const;

    virtual void validate() const = 0;

    virtual void applyLoaded(const char* typeName, const double* values, int count) = 0;

public:
    Base();
    Base(const char* typeName, const double* values, int count);
    Base(const Base& other);
    virtual ~Base();

    Base& operator=(const Base& other);
    bool operator==(const Base& other) const;
    bool operator!=(const Base& other) const;
    bool operator<(const Base& other) const;
    bool operator>(const Base& other) const;

    const char* getType() const;
    int getSizesCount() const;
    double getSize(int index) const;
    void setSize(int index, double value);
    void setSizes(const double* values, int count);

    virtual const char* getKind() const = 0;
    virtual const char* getTag() const = 0;
    virtual const char* getMeasureName() const = 0;
    virtual double getMeasure() const = 0;
    virtual const char* getParamLabel(int index) const = 0;
    virtual int getTypesCount() const = 0;
    virtual const char* getTypeNameAt(int typeIndex) const = 0;
    virtual int getParamCountOf(int typeIndex) const = 0;
    virtual void changeType(int typeIndex, const double* values, int count) = 0;
    virtual void draw() const = 0;
    virtual Base* clone() const = 0;

    virtual void print(std::ostream& out) const;

    void save(std::ostream& out) const;
    void load(std::istream& in);
};

std::ostream& operator<<(std::ostream& out, const Base& figure);

#endif