#ifndef SPLICEPICTURESCALIBRATIONITEM_H
#define SPLICEPICTURESCALIBRATIONITEM_H

#include "rational.h"

class QJsonObject;

class SplicePicturesCalibrationItem
{
public:
    SplicePicturesCalibrationItem();
    SplicePicturesCalibrationItem(int row, int col, Rational rotation, Rational zoom);
    SplicePicturesCalibrationItem(QJsonObject &json);
    QJsonObject toJsonObject();

    Rational getRotation() const;
    Rational getZoom() const;
    int getRow();
    int getCol();
    void setRotation(Rational rotation);
    void setZoom(Rational zoom);
    void setRow(int row);
    void setCol(int col);
private:
    int row;
    int col;
    Rational rotation;
    Rational zoom;
};

#endif // SPLICEPICTURESCALIBRATIONITEM_H
