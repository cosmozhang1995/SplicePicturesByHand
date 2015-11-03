#include "splicepicturespanel.h"

#include "rational.h"
#include "backgrounddodging.h"

#include <QDebug>
#include <math.h>
#include <QSize>
#include <QPoint>
#include <QVector>
#include <QPainter>
#include <QPen>
#include <QLine>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMatrix>
#include <QCursor>
#include <QPaintEngine>
#include <QPaintEngine>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>

SplicePicturesPanel::SplicePicturesPanel(int rows, int cols, int uwidth, int uheight, float marginRate, float zoom, QWidget *parent) : QWidget(parent),
    rows(rows), cols(cols), uwidth(uwidth), uheight(uheight), marginRate(marginRate), viewZoom(zoom), imageZoom(0.0f),
    _inDrag(true)
{
    setFocusPolicy(Qt::StrongFocus);
    setZoomCenter(0, 0);
    connect(this, SIGNAL(onDrag(int,int,int,int)), this, SLOT(onDragEvent(int,int,int,int)));
    setCursor(Qt::OpenHandCursor);
}

SplicePicturesPanel::~SplicePicturesPanel() {
}

void SplicePicturesPanel::initialize() {
    return;
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 0, new QImage("/Users/cosmozhang/Desktop/Background/1.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 1, new QImage("/Users/cosmozhang/Desktop/Background/2.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 2, new QImage("/Users/cosmozhang/Desktop/Background/3.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 3, new QImage("/Users/cosmozhang/Desktop/Background/4.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 0, new QImage("/Users/cosmozhang/Desktop/Background/5.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 1, new QImage("/Users/cosmozhang/Desktop/Background/6.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 2, new QImage("/Users/cosmozhang/Desktop/Background/7.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 3, new QImage("/Users/cosmozhang/Desktop/Background/8.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 0, new QImage("/Users/cosmozhang/Desktop/Background/9.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 1, new QImage("/Users/cosmozhang/Desktop/Background/10.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 2, new QImage("/Users/cosmozhang/Desktop/Background/11.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 3, new QImage("/Users/cosmozhang/Desktop/Background/12.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 0, new QImage("/Users/cosmozhang/Desktop/Background/13.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 1, new QImage("/Users/cosmozhang/Desktop/Background/14.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 2, new QImage("/Users/cosmozhang/Desktop/Background/15.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 3, new QImage("/Users/cosmozhang/Desktop/Background/16.tiff")));
}

QSize SplicePicturesPanel::sizeHint() const {
    return QSize(((float)cols + 2 * marginRate) * uwidth,
                 ((float)rows + 2 * marginRate) * uheight);
}
QSize SplicePicturesPanel::minimumSizeHint() const {
    return QSize(((float)cols + 2 * marginRate) * uwidth,
                 ((float)rows + 2 * marginRate) * uheight);
}

void SplicePicturesPanel::paintEvent(QPaintEvent *) {
    int drawUWidth = uwidth * viewZoom;
    int drawUHeight = uheight * viewZoom;
    int drawMarginWidth = marginRate * viewZoom * uwidth;
    int drawMarginHeight = marginRate * viewZoom * uheight;
    int zoomOffsetX = minimumSizeHint().width() / 2 - zoomCenterX;
    int zoomOffsetY = minimumSizeHint().height() / 2 - zoomCenterY;
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor::fromRgb(200,200,200));
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    QVector<QLine> lines;
    for (int i = 0; i <= cols; i++) lines.push_back(QLine(drawUWidth * i + drawMarginWidth + zoomOffsetX, zoomOffsetY, drawUWidth * i + drawMarginWidth + zoomOffsetX, drawUHeight * rows + 2 * drawMarginWidth + zoomOffsetY));
    for (int i = 0; i <= rows; i++) lines.push_back(QLine(zoomOffsetX, drawUHeight * i + drawMarginHeight + zoomOffsetY, drawUWidth * rows + drawMarginWidth * 2 + zoomOffsetX, drawUHeight * i + drawMarginHeight + zoomOffsetY));
    painter.fillRect(drawMarginWidth + zoomOffsetX, drawMarginHeight + zoomOffsetY,
                     cols * drawUWidth, rows * drawUHeight,
                     QColor::fromRgb(255,255,255));
    painter.drawLines(lines);
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int drawWidth = (float)iter->getZoom() * imageZoom * viewZoom * (float)iter->getWidth();
        int drawHeight = (float)iter->getZoom() * imageZoom * viewZoom * (float)iter->getHeight();
        int drawOffsetX = imageZoom * viewZoom * (float)iter->getX() + zoomOffsetX;
        int drawOffsetY = imageZoom * viewZoom * (float)iter->getY() + zoomOffsetY;
        int drawStartX = drawMarginWidth + iter->getCol() * drawUWidth + (drawUWidth - drawWidth) / 2 + drawOffsetX;
        int drawStartY = drawMarginHeight + iter->getRow() * drawUHeight + (drawUHeight - drawHeight) / 2 + drawOffsetY;
        int drawCenterX = drawStartX + drawWidth / 2;
        int drawCenterY = drawStartY + drawHeight / 2;

        QMatrix matrix;
        matrix.scale((double)iter->getZoom(), (double)iter->getZoom());
        matrix.rotate((double)iter->getRotation());
        matrix.translate(iter->getX(), iter->getY());
        double rotationRadius = (double)iter->getRotation() / 180.0 * M_PI;
        int resizedDrawWidth = drawWidth * fabs(cos(rotationRadius)) + drawHeight * fabs(sin(rotationRadius));
        int resizedDrawHeight = drawWidth * fabs(sin(rotationRadius)) + drawHeight * fabs(cos(rotationRadius));
        drawWidth = resizedDrawWidth;
        drawHeight = resizedDrawHeight;
        drawStartX = drawCenterX - drawWidth / 2;
        drawStartY = drawCenterY - drawHeight / 2;
        painter.drawPixmap(drawStartX, drawStartY,
                           drawWidth, drawHeight,
//                           iter->pixmap);
                           iter->getPixmap()->transformed(matrix));

//        painter.translate(drawCenterX, drawCenterY);
//        painter.rotate(-(double)iter->rotation);
//        painter.translate(-drawCenterX, -drawCenterY);
    }
}

void SplicePicturesPanel::drawLayout() {
}

//void SplicePicturesPanel::setGrids(int rows, int cols) {
//    this->rows = rows;
//    this->cols = cols;
//}
//void SplicePicturesPanel::setUnitSize(int uwidth, int uheight) {
//    this->uwidth = uwidth;
//    this->uheight = uheight;
//}
void SplicePicturesPanel::setViewZoom(float zoom) {
    float oldViewZoom = this->viewZoom;
    this->viewZoom = zoom;
    setZoomCenter(zoomCenterX * zoom / oldViewZoom, zoomCenterY * zoom / oldViewZoom);
    update();
}
void SplicePicturesPanel::setZoomCenter(int x, int y) {
    zoomCenterX = std::min(std::max(x, minimumSizeHint().width() / 2), (int)(minimumSizeHint().width() * (viewZoom - 0.5f)));
    zoomCenterY = std::min(std::max(y, minimumSizeHint().height() / 2), (int)(minimumSizeHint().height() * (viewZoom - 0.5f)));
    update();
}

int SplicePicturesPanel::getGridRows() {
    return rows;
}
int SplicePicturesPanel::getGridCols() {
    return cols;
}
int SplicePicturesPanel::getUnitWidth() {
    return uwidth;
}
int SplicePicturesPanel::getUnitHeight() {
    return uheight;
}
float SplicePicturesPanel::getZoom() {
    return viewZoom;
}
QPoint SplicePicturesPanel::getZoomCenter() {
    return QPoint(zoomCenterX, zoomCenterY);
}

QPixmap *SplicePicturesPanel::getFullPixmap() {
    QPixmap *pix = new QPixmap((int)(cols * uwidth / imageZoom), (int)(rows * uheight / imageZoom));
    pix->fill(QColor::fromRgb(255,255,255));
    QPainter *painter = new QPainter(pix);
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int drawWidth = (float)iter->getZoom() * (float)iter->getWidth();
        int drawHeight = (float)iter->getZoom() * (float)iter->getHeight();
        int drawOffsetX = (float)iter->getX();
        int drawOffsetY = (float)iter->getY();
        int drawStartX = iter->getCol() * uwidth / imageZoom + (uwidth / imageZoom - drawWidth) / 2 + drawOffsetX;
        int drawStartY = iter->getRow() * uheight / imageZoom + (uheight / imageZoom - drawHeight) / 2 + drawOffsetY;
        int drawCenterX = drawStartX + drawWidth / 2;
        int drawCenterY = drawStartY + drawHeight / 2;
        QMatrix matrix;
        matrix.scale((double)iter->getZoom(), (double)iter->getZoom());
        matrix.rotate((double)iter->getRotation());
        matrix.translate(iter->getX(), iter->getY());
        double rotationRadius = (double)iter->getRotation() / 180.0 * M_PI;
        int resizedDrawWidth = drawWidth * fabs(cos(rotationRadius)) + drawHeight * fabs(sin(rotationRadius));
        int resizedDrawHeight = drawWidth * fabs(sin(rotationRadius)) + drawHeight * fabs(cos(rotationRadius));
        drawWidth = resizedDrawWidth;
        drawHeight = resizedDrawHeight;
        drawStartX = drawCenterX - drawWidth / 2;
        drawStartY = drawCenterY - drawHeight / 2;
        painter->drawPixmap(drawStartX, drawStartY,
                           drawWidth, drawHeight,
                           iter->getPixmap()->transformed(matrix));
    }
    return pix;
}

QPixmap * SplicePicturesPanel::getTransformedPixmap(int row, int col) {
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            QMatrix matrix;
            matrix.scale((double)iter->getZoom(), (double)iter->getZoom());
            matrix.rotate((double)iter->getRotation());
            matrix.translate(iter->getX(), iter->getY());
            return new QPixmap(iter->getPixmap()->transformed(matrix));
        }
    }
    return NULL;
}

void SplicePicturesPanel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        _inDrag = true;
        int pos_x, pos_y;
        pos_x = event->pos().x();
        pos_y = event->pos().y();
        _dragPosX = pos_x;
        _dragPosY = pos_y;
        setCursor(Qt::ClosedHandCursor);
    }
    return QWidget::mousePressEvent(event);
}
void SplicePicturesPanel::mouseMoveEvent(QMouseEvent *event) {
    int pos_x = event->pos().x();
    int pos_y = event->pos().y();
    if (_inDrag) {
        _didDrag = true;
        emit onDrag(_dragPosX, _dragPosY, pos_x, pos_y);
        _dragPosX = pos_x;
        _dragPosY = pos_y;
    }
}
void SplicePicturesPanel::mouseReleaseEvent(QMouseEvent *event) {
    if (_inDrag) {
        _inDrag = false;
        setCursor(Qt::OpenHandCursor);
        if (!_didDrag) {
            int drawMarginWidth = marginRate * viewZoom * uwidth;
            int drawMarginHeight = marginRate * viewZoom * uheight;
            int zoomOffsetX = minimumSizeHint().width() / 2 - zoomCenterX;
            int zoomOffsetY = minimumSizeHint().height() / 2 - zoomCenterY;
            int pos_x, pos_y;
            pos_x = event->pos().x() - zoomOffsetX;
            pos_y = event->pos().y() - zoomOffsetY;
            int row, col;
            col = (pos_x - drawMarginWidth) / (uwidth * viewZoom);
            row = (pos_y - drawMarginHeight) / (uheight * viewZoom);
            if (col >= 0 && col < cols && row >= 0 && row <= rows)
                emit onUnitClicked(row, col);
        }
    }
    _didDrag = false;
}

void SplicePicturesPanel::wheelEvent(QWheelEvent *event) {
}

void SplicePicturesPanel::onDragEvent(int x1, int y1, int x2, int y2) {
}

void SplicePicturesPanel::loadImage(int row, int col, QString filePath, bool removeIfExists) {
    if (removeIfExists) removeImage(row, col);
    SplicePicturesImageItem item(row, col, filePath);
    QImage *backgroundImage = getBackground(row, col);
    if (backgroundImage != NULL) {
        QImage *dodgedImage = BackgroundDodging::dodgeBackground(*item.getImage(), *backgroundImage);
        item.setImage(dodgedImage);
        item.setPixmap(new QPixmap(QPixmap::fromImage(*dodgedImage)));
        dodgedImage->save(QString("/Users/cosmozhang/Desktop/Background/export_dodged/%1.tiff").arg(row * 4 + col + 1));
    }
    SplicePicturesCalibrationItem *calibrationItem = getCalibrationItem(row, col);
    if (calibrationItem != NULL) {
        item.setRotation(calibrationItem->getRotation());
        item.setZoom(calibrationItem->getZoom());
        item.getPixmap()->save(QString("/Users/cosmozhang/Desktop/Background/export_calibrated/%1.tiff").arg(row * 4 + col + 1));
    }
    imageList.push_back(item);
    if (imageZoom == 0.0f) {
        if ((float)item.getWidth() / (float)item.getHeight() < (float)uwidth / (float)uheight) {
            imageZoom = (float)uwidth / (float)item.getWidth();
        } else {
            imageZoom = (float)uheight / (float)item.getHeight();
        }
    }
    update();
}
bool SplicePicturesPanel::removeImage(int row, int col) {
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            imageList.erase(iter);
            update();
            return true;
        }
    }
    return false;
}
bool SplicePicturesPanel::removeLastImage() {
    if (imageList.empty()) return false;
    imageList.pop_back();
    update();
    return true;
}
bool SplicePicturesPanel::removeAllImages() {
    if (imageList.empty()) return false;
    imageList.clear();
    update();
    return true;
}

QJsonDocument SplicePicturesPanel::getConfiguration() {
    QJsonArray array;
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        array.push_back(iter->toJsonObject());
    }
    QJsonObject json;
    json.insert("zoom", (double)imageZoom);
    json.insert("images", array);
    QJsonDocument doc;
    doc.setObject(json);
    return doc;
}
void SplicePicturesPanel::loadConfiguration(QJsonDocument configuration) {
    imageList.clear();
    QJsonObject json = configuration.object();
    imageZoom = json.take("zoom").toDouble();
    QJsonArray array = json.take("images").toArray();
    foreach (const QJsonValue & value, array) {
        QJsonObject itemJson = value.toObject();
        SplicePicturesImageItem item(itemJson);
        imageList.push_back(item);
    }
    update();
}
QByteArray SplicePicturesPanel::getConfigurationAsData() {
    return getConfiguration().toJson();
}
void SplicePicturesPanel::loadConfigurationFromData(QByteArray configuration) {
    loadConfiguration(QJsonDocument::fromJson(configuration));
}
QString SplicePicturesPanel::getConfigurationAsString() {
    return QString(getConfigurationAsData());
}
void SplicePicturesPanel::loadConfigurationFromString(QString configuration) {
    loadConfigurationFromData(configuration.toUtf8());
}

QJsonDocument SplicePicturesPanel::getCalibration() {
    QJsonArray array;
    for (QVector<SplicePicturesCalibrationItem>::iterator iter = calibrationList.begin(); iter != calibrationList.end(); iter++) {
        array.push_back(iter->toJsonObject());
    }
    QJsonObject json;
    json.insert("images", array);
    QJsonDocument doc;
    doc.setObject(json);
    return doc;
}
void SplicePicturesPanel::loadCalibration(QJsonDocument calibration) {
    calibrationList.clear();
    QJsonObject json = calibration.object();
    QJsonArray array = json.take("images").toArray();
    foreach (const QJsonValue & value, array) {
        QJsonObject itemJson = value.toObject();
        SplicePicturesCalibrationItem item(itemJson);
        calibrationList.push_back(item);
    }
    update();
}
QByteArray SplicePicturesPanel::getCalibrationAsData() {
    return getCalibration().toJson();
}
void SplicePicturesPanel::loadCalibrationFromData(QByteArray calibration) {
    loadCalibration(QJsonDocument::fromJson(calibration));
}
QString SplicePicturesPanel::getCalibrationAsString() {
    return QString(getCalibrationAsData());
}
void SplicePicturesPanel::loadCalibrationFromString(QString calibration) {
    loadCalibrationFromData(calibration.toUtf8());
}

QVector<SplicePicturesBackgroundItem> & SplicePicturesPanel::getBackgroundList() {
    return backgroundList;
}
void SplicePicturesPanel::setBackgroundList(QVector<SplicePicturesBackgroundItem> &list) {
    backgroundList = list;
}

SplicePicturesImageItem *SplicePicturesPanel::selectImage(int row, int col) {
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            SplicePicturesImageItem item = *(SplicePicturesImageItem *)iter;
            imageList.erase(iter);
            imageList.push_back(item);
            update();
            return (SplicePicturesImageItem *)imageList.cend();
        }
    }
    return NULL;
}

SplicePicturesImageItem *SplicePicturesPanel::currentImage() {
    if (imageList.empty()) return NULL;
    else return &imageList.last();
}

void SplicePicturesPanel::move(int x, int y) {
    if (imageList.empty()) return;
    SplicePicturesImageItem *item = (SplicePicturesImageItem *)imageList.end() - 1;
    item->setX(item->getX() + x);
    item->setY(item->getY() + y);
    update();
}
void SplicePicturesPanel::rotate(Rational degree) {
    if (imageList.empty()) return;
    SplicePicturesImageItem *item = (SplicePicturesImageItem *)imageList.end() - 1;
    Rational rotation = item->getRotation();
    item->setRotation(rotation + degree);
    update();
}
void SplicePicturesPanel::zoom(Rational rate) {
    if (imageList.empty()) return;
    SplicePicturesImageItem *item = (SplicePicturesImageItem *)imageList.end() - 1;
    Rational zoom = item->getZoom();
    item->setZoom(zoom + rate);
    update();
}

void SplicePicturesPanel::doViewZoom(bool zoomIn) {
    float currentZoom = getZoom();
    if (currentZoom < MAX_ZOOM && zoomIn) {
        setViewZoom(currentZoom * ZOOM_RATE);
    } else if (currentZoom > MIN_ZOOM && !zoomIn) {
        setViewZoom(currentZoom / ZOOM_RATE);
    }
}

SplicePicturesCalibrationItem * SplicePicturesPanel::getCalibrationItem(int row, int col) {
    for (QVector<SplicePicturesCalibrationItem>::iterator iter = calibrationList.begin(); iter != calibrationList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            return (SplicePicturesCalibrationItem *)iter;
        }
    }
    return NULL;
}
QImage * SplicePicturesPanel::getBackground(int row, int col) {
    for (QVector<SplicePicturesBackgroundItem>::iterator iter = backgroundList.begin(); iter != backgroundList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            return iter->getImage();
        }
    }
    return NULL;
}
