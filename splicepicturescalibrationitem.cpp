#include "splicepicturescalibrationitem.h"

#include <QJsonObject>

SplicePicturesCalibrationItem::SplicePicturesCalibrationItem()
{
    rotation = Rational(0);
    zoom = Rational(1);
}

SplicePicturesCalibrationItem::SplicePicturesCalibrationItem(int row, int col, Rational rotation, Rational zoom)
    : row(row), col(col), rotation(rotation), zoom(zoom)
{
}

SplicePicturesCalibrationItem::SplicePicturesCalibrationItem(QJsonObject &json) {
    row = json.take("row").toInt();
    col = json.take("col").toInt();
    rotation = Rational(json.take("rotation").toString());
    zoom = Rational(json.take("zoom").toString());
}

QJsonObject SplicePicturesCalibrationItem::toJsonObject() {
    QJsonObject json;
    json.insert("row", this->row);
    json.insert("col", this->col);
    json.insert("rotation", this->rotation.toString());
    json.insert("zoom", this->zoom.toString());
    return json;
}

void SplicePicturesCalibrationItem::setRotation(Rational rotation) { this->rotation = rotation; }
void SplicePicturesCalibrationItem::setZoom(Rational zoom) { this->zoom = zoom; }
void SplicePicturesCalibrationItem::setRow(int row) { this->row = row; }
void SplicePicturesCalibrationItem::setCol(int col) { this->col = col; }
Rational SplicePicturesCalibrationItem::getRotation() const { return rotation; }
Rational SplicePicturesCalibrationItem::getZoom() const { return zoom; }
int SplicePicturesCalibrationItem::getRow() { return row; }
int SplicePicturesCalibrationItem::getCol() { return col; }
