#ifndef PLANEFIGURE_H
#define PLANEFIGURE_H

#include "Base.h"

// Плоская фигура: тип, размеры, площадь, рисунок на экране
// Типы: круг, прямоугольник, квадрат, треугольник
class PlaneFigure : public Base {
private:
    int typeIndex;

protected:
    void validate() const override;
    void applyLoaded(const char* typeName, const double* values, int count) override;

public:
    enum Type { CIRCLE = 0, RECTANGLE, SQUARE, TRIANGLE, TYPES_COUNT };

    // Справочная информация о типах
    static int typesCount();
    static const char* typeName(int typeIdx);
    static int paramCount(int typeIdx);
    static const char* paramLabel(int typeIdx, int paramIdx);
    static int findType(const char* name); // -1, если типа нет

    PlaneFigure(); // квадрат со стороной 1
    PlaneFigure(int typeIdx, const double* values, int count);
    PlaneFigure(const PlaneFigure& other);
    ~PlaneFigure() override;

    PlaneFigure& operator=(const PlaneFigure& other);

    int getTypeIndex() const;
    void setFigure(int typeIdx, const double* values, int count); // сменить тип и размеры
    double getArea() const;

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
