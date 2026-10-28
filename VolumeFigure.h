#ifndef VOLUMEFIGURE_H
#define VOLUMEFIGURE_H

#include "Base.h"

// Объёмная фигура: тип, размеры, объём, рисунок на экране в виде плоской фигуры
// Типы: куб, параллелепипед, шар, цилиндр, конус
class VolumeFigure : public Base {
private:
    int typeIndex;

protected:
    void validate() const override;
    void applyLoaded(const char* typeName, const double* values, int count) override;

public:
    enum Type { CUBE = 0, BOX, SPHERE, CYLINDER, CONE, TYPES_COUNT };

    static int typesCount();
    static const char* typeName(int typeIdx);
    static int paramCount(int typeIdx);
    static const char* paramLabel(int typeIdx, int paramIdx);
    static const char* projectionName(int typeIdx); // какой плоской фигурой рисуется
    static int findType(const char* name);

    VolumeFigure(); // куб с ребром 1
    VolumeFigure(int typeIdx, const double* values, int count);
    VolumeFigure(const VolumeFigure& other);
    ~VolumeFigure() override;

    VolumeFigure& operator=(const VolumeFigure& other);

    int getTypeIndex() const;
    void setFigure(int typeIdx, const double* values, int count);
    double getVolume() const;

    const char* getKind() const override;
    const char* getTag() const override;
    const char* getMeasureName() const override;
    double getMeasure() const override;
    const char* getParamLabel(int index) const override;
    int getTypesCount() const override;
    const char* getTypeNameAt(int typeIdx) const override;
    int getParamCountOf(int typeIdx) const override;
    void changeType(int typeIdx, const double* values, int count) override;
    void draw() const override;
    Base* clone() const override;
};

#endif
