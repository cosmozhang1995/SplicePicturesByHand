#include "splicepicturesimageitem.h"
#include "rational.h"

#include <QImage>
#include <QPixmap>
#include <QString>
#include <QDebug>
#include <QMatrix>
#include <QSize>
#include <QRect>

SplicePicturesImageItem::SplicePicturesImageItem()
    : row(0), col(0), image(NULL), pixmap(NULL), width(0), height(0), x(0), y(0), path(""), rotation(0), zoom(1)
{
}
SplicePicturesImageItem::SplicePicturesImageItem(const SplicePicturesImageItem &item)
    : row(item.row), col(item.col), width(item.width), height(item.height), x(item.x), y(item.y), path(item.path), rotation(item.rotation), zoom(item.zoom)
{
    if (item.image) image = new QImage(*item.image);
    else image = NULL;
    if (item.pixmap) pixmap = new QPixmap(*item.pixmap);
    else pixmap = NULL;
}
SplicePicturesImageItem::SplicePicturesImageItem(QJsonObject &json) {
    row = json.take("row").toInt();
    col = json.take("col").toInt();
    path = json.take("path").toString();
    x = json.take("x").toInt();
    y = json.take("y").toInt();
    rotation = Rational(json.take("rotation").toString());
    zoom = Rational(json.take("zoom").toString());
    loadFromPath(path);
}

SplicePicturesImageItem::SplicePicturesImageItem(int row, int col, QString path)
    : row(row), col(col), x(0), y(0), path(path), rotation(0), zoom(1)
{
    loadFromPath(path);
}
//SplicePicturesImageItem::SplicePicturesImageItem(int row, int col, QImage *image, QString path)
//    : row(row), col(col), image(image), x(0), y(0), path(path), rotation(0), zoom(1)
//{
//    QPixmap pixmap = QPixmap::fromImage(*image);
//    int pix_w = image->size().width();
//    int pix_h = image->size().height();
//    this->pixmap = new QPixmap(pixmap);
//    this->width = pix_w;
//    this->height = pix_h;
//    qDebug() << "init " << this->width << " " << this->height;
//    qDebug() << "pointer 1 " << this;
//}
//SplicePicturesImageItem::SplicePicturesImageItem(int row, int col, QImage *image, QPixmap pixmap, QString path, int width, int height, int x, int y)
//    : row(row), col(col), image(image), pixmap(pixmap), path(path), width(width), height(height), x(x), y(y), rotation(0), zoom(1)
//{
//}
SplicePicturesImageItem::~SplicePicturesImageItem() {
    if (pixmap) delete pixmap;
    if (image) delete image;
}

int SplicePicturesImageItem::getRow() { return row; }
int SplicePicturesImageItem::getCol() { return col; }
QImage * SplicePicturesImageItem::getImage() { return image; }
QPixmap * SplicePicturesImageItem::getPixmap() { return pixmap; }
int SplicePicturesImageItem::getWidth() { return width; }
int SplicePicturesImageItem::getHeight() { return height; }
int SplicePicturesImageItem::getX() { return x; }
int SplicePicturesImageItem::getY() { return y; }
QString SplicePicturesImageItem::getPath() const { return path; }
Rational SplicePicturesImageItem::getRotation() const { return rotation; }
Rational SplicePicturesImageItem::getZoom() const { return zoom; }

void SplicePicturesImageItem::setRow(int row) { this->row = row; }
void SplicePicturesImageItem::setCol(int col) { this->col = col; }
void SplicePicturesImageItem::setImage(QImage *image) { if (image == this->image) return; if (this->image) delete this->image; this->image = image; }
void SplicePicturesImageItem::setPixmap(QPixmap *pixmap) { if (pixmap == this->pixmap) return; if (this->pixmap) delete this->pixmap; this->pixmap = pixmap; }
void SplicePicturesImageItem::setWidth(int width) { this->width = width; }
void SplicePicturesImageItem::setHeight(int height) { this->height = height; }
void SplicePicturesImageItem::setX(int x) { this->x = x; }
void SplicePicturesImageItem::setY(int y) { this->y = y; }
void SplicePicturesImageItem::setPath(QString path) { this->path = path; }
void SplicePicturesImageItem::setRotation(Rational rotation) { this->rotation = rotation; }
void SplicePicturesImageItem::setZoom(Rational zoom) { this->zoom = zoom; }

void SplicePicturesImageItem::loadFromPath(QString path) {
    QImage *image = new QImage(path);
    QPixmap pixmap = QPixmap::fromImage(*image);
    int pix_w = image->size().width();
    int pix_h = image->size().height();
    int cut = 0;
    image = new QImage(image->copy(cut, cut, pix_w - 2*cut, pix_h - 2*cut));
    pixmap = QPixmap::fromImage(*image);
    this->image = image;
    this->pixmap = new QPixmap(pixmap);
    this->width = pix_w;
    this->height = pix_h;
    this->path = path;
}

QJsonObject SplicePicturesImageItem::toJsonObject() {
    QJsonObject json;
    json.insert("row", this->row);
    json.insert("col", this->col);
    json.insert("x", this->x);
    json.insert("y", this->y);
    json.insert("rotation", this->rotation.toString());
    json.insert("zoom", this->zoom.toString());
    json.insert("path", this->path);
    return json;
}

QImage SplicePicturesImageItem::getTransformedImage(TransformFlag flag) {
    QMatrix matrix;
    int flagint = (int)flag;
    if (flagint & TRANS_SCALE) matrix.scale((double)getZoom(), (double)getZoom());
    if (flagint & TRANS_ROTATE) matrix.rotate((double)getRotation());
//    if (flagint & TRANS_MOVE) matrix.translate(getX(), getY());
    return getImage()->transformed(matrix);
}
QPixmap SplicePicturesImageItem::getTransformedPixmap(TransformFlag flag) {
    QMatrix matrix;
    int flagint = (int)flag;
    if (flagint & TRANS_SCALE) matrix.scale((double)getZoom(), (double)getZoom());
    if (flagint & TRANS_ROTATE) matrix.rotate((double)getRotation());
//    if (flagint & TRANS_MOVE) matrix.translate(getX(), getY());
    return getPixmap()->transformed(matrix);
}
QImage SplicePicturesImageItem::getTransformedImage(QRect range, TransformFlag flag) {
    return getTransformedImage(flag).copy(range);
}
QPixmap SplicePicturesImageItem::getTransformedPixmap(QRect range, TransformFlag flag) {
    return getTransformedPixmap(flag).copy(range);
}
QImage SplicePicturesImageItem::getTransformedImage(QSize size, TransformFlag flag) {
    QSize imageSize = getImage()->size();
    QRect range((imageSize.width() - size.width())/2, (imageSize.height() - size.height())/2, size.width(), size.height());
    return getTransformedImage(range, flag);
}
QPixmap SplicePicturesImageItem::getTransformedPixmap(QSize size, TransformFlag flag) {
    QSize imageSize = getImage()->size();
    QRect range((imageSize.width() - size.width())/2, (imageSize.height() - size.height())/2, size.width(), size.height());
    return getTransformedPixmap(range, flag);
}
