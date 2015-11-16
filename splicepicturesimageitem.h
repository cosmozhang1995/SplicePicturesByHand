#ifndef SPLICEPICTURESIMAGEITEM_H
#define SPLICEPICTURESIMAGEITEM_H

#include "rational.h"

#include <QString>
#include <QJsonObject>
#include <QRect>
#include <QSize>

class QImage;
class QPixmap;
class Rational;

class SplicePicturesImageItem
{
public:
    enum TransformFlag {
        TRANS_SCALE = 0x01,
        TRANS_ROTATE = 0x02,
        TRANS_MOVE = 0x04
    };

    SplicePicturesImageItem();
    SplicePicturesImageItem(const SplicePicturesImageItem &item);
    SplicePicturesImageItem(QJsonObject &json);
    SplicePicturesImageItem(int row, int col, QString path);
    ~SplicePicturesImageItem();
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

    QImage getTransformedImage(TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
    QPixmap getTransformedPixmap(TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
    QImage getTransformedImage(QSize size, TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
    QPixmap getTransformedPixmap(QSize size, TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
    QImage getTransformedImage(QRect range, TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
    QPixmap getTransformedPixmap(QRect range, TransformFlag flag = (TransformFlag)(TRANS_SCALE | TRANS_ROTATE | TRANS_MOVE));
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
