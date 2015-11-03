#ifndef SPLICEPICTURESIMAGEITEM_H
#define SPLICEPICTURESIMAGEITEM_H

#include "rational.h"

#include <QString>
#include <QJsonObject>

class QImage;
class QPixmap;
class Rational;

class SplicePicturesImageItem
{
public:
    SplicePicturesImageItem();
    SplicePicturesImageItem(const SplicePicturesImageItem &item);
    SplicePicturesImageItem(QJsonObject &json);
    SplicePicturesImageItem(int row, int col, QString path);
//    SplicePicturesImageItem(int row, int col, QImage *image, QString path);
//    SplicePicturesImageItem(int row, int col, QImage *image, QPixmap pixmap, QString path, int width, int height, int x, int y);

    QJsonObject toJsonObject();

    int getRow();
    int getCol();
    QImage *getImage();
    QPixmap *getPixmap();
    int getWidth();
    int getHeight();
    int getX();
    int getY();
    QString getPath() const;
    Rational getRotation() const;
    Rational getZoom() const;

    void setRow(int row);
    void setCol(int col);
    void setImage(QImage *image);
    void setPixmap(QPixmap *pixmap);
    void setWidth(int width);
    void setHeight(int height);
    void setX(int x);
    void setY(int y);
    void setPath(QString path);
    void setRotation(Rational rotation);
    void setZoom(Rational zoom);
private:
    int row;
    int col;
    QImage *image;
    QPixmap *pixmap;
    int width;
    int height;
    int x;
    int y;
    QString path;
    Rational rotation;
    Rational zoom;

    void loadFromPath(QString path);
};

#endif // SPLICEPICTURESIMAGEITEM_H
