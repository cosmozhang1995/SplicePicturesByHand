#include "splicepicturespanel.h"

#include "rational.h"
#include "backgrounddodging.h"
#include "autostitch.h"

#include "limits.h"
#include "ctime"
#include "math.h"

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

SplicePicturesPanel::SplicePicturesPanel(int rows, int cols, int uwidth, int uheight, double marginRate, double zoom, QWidget *parent) : QWidget(parent),
    rows(rows), cols(cols), uwidth(uwidth), uheight(uheight), marginRate(marginRate), viewZoom(zoom), imageZoom(0.0f),
    _inDrag(true),
    fullPixmap(NULL), fullPixmapResized(NULL), fullPixmapResizedAndCut(NULL), fullPixmapCut(NULL), fullPixmapCutAndResized(NULL), finalPixmap(NULL),
    globalDrawStartX(INT_MAX), globalDrawStartY(INT_MAX), globalDrawWidth(0), globalDrawHeight(0),
    finalDrawStartX(0), finalDrawStartY(0), finalDrawWidth(0), finalDrawHeight(0)
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
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 0, new QImage("/Users/cosmozhang/work/test/Background//1.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 1, new QImage("/Users/cosmozhang/work/test/Background//2.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 2, new QImage("/Users/cosmozhang/work/test/Background//3.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(0, 3, new QImage("/Users/cosmozhang/work/test/Background//4.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 0, new QImage("/Users/cosmozhang/work/test/Background//5.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 1, new QImage("/Users/cosmozhang/work/test/Background//6.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 2, new QImage("/Users/cosmozhang/work/test/Background//7.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(1, 3, new QImage("/Users/cosmozhang/work/test/Background//8.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 0, new QImage("/Users/cosmozhang/work/test/Background//9.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 1, new QImage("/Users/cosmozhang/work/test/Background//10.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 2, new QImage("/Users/cosmozhang/work/test/Background//11.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(2, 3, new QImage("/Users/cosmozhang/work/test/Background//12.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 0, new QImage("/Users/cosmozhang/work/test/Background//13.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 1, new QImage("/Users/cosmozhang/work/test/Background//14.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 2, new QImage("/Users/cosmozhang/work/test/Background//15.tiff")));
    backgroundList.push_back(SplicePicturesBackgroundItem(3, 3, new QImage("/Users/cosmozhang/work/test/Background//16.tiff")));
}

QSize SplicePicturesPanel::sizeHint() const {
    return QSize(((double)cols + 2 * marginRate) * uwidth,
                 ((double)rows + 2 * marginRate) * uheight);
}
QSize SplicePicturesPanel::minimumSizeHint() const {
    return QSize(((double)cols + 2 * marginRate) * uwidth,
                 ((double)rows + 2 * marginRate) * uheight);
}

void SplicePicturesPanel::paintEvent(QPaintEvent *) {
    int drawUWidth = uwidth * viewZoom;
    int drawUHeight = uheight * viewZoom;
    int drawMarginWidth = marginRate * viewZoom * uwidth;
    int drawMarginHeight = marginRate * viewZoom * uheight;
    int canvasWidth = minimumSizeHint().width();
    int canvasHeight = minimumSizeHint().height();
    int zoomOffsetX = canvasWidth / 2 - zoomCenterX;
    int zoomOffsetY = canvasHeight / 2 - zoomCenterY;
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor::fromRgb(200,200,200));
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    QVector<QLine> lines;
    for (int i = 0; i <= cols; i++) {
        int line_x = drawUWidth * i + drawMarginWidth + zoomOffsetX;
        if (line_x >= 0 && line_x <= canvasWidth)
            lines.push_back(QLine(line_x, 0, line_x, canvasHeight));
    }
    for (int i = 0; i <= rows; i++) {
        int line_y = drawUHeight * i + drawMarginHeight + zoomOffsetY;
        if (line_y >= 0 && line_y <= canvasHeight)
            lines.push_back(QLine(0, line_y, canvasWidth, line_y));
    }
    int fillStartX = drawMarginWidth + zoomOffsetX; if (fillStartX < 0) fillStartX = 0;
    int fillStartY = drawMarginHeight + zoomOffsetY; if (fillStartY < 0) fillStartY = 0;
    int fillEndX = fillStartX + cols * drawUWidth; if (fillEndX > canvasWidth) fillEndX = canvasWidth;
    int fillEndY = fillStartY + rows * drawUHeight; if (fillEndY > canvasHeight) fillEndY = canvasHeight;
    painter.fillRect(fillStartX, fillStartY, fillEndX - fillStartX, fillEndY - fillStartY, QColor::fromRgb(255,255,255));
    painter.drawLines(lines);
    if (finalPixmap) {
        painter.drawPixmap(finalDrawStartX, finalDrawStartY, finalDrawWidth, finalDrawHeight,
                           *finalPixmap);
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
void SplicePicturesPanel::setViewZoom(double zoom, bool withUpdate) {
    double oldViewZoom = this->viewZoom;
    this->viewZoom = zoom;
    setZoomCenter(zoomCenterX * zoom / oldViewZoom, zoomCenterY * zoom / oldViewZoom, false);
    if (withUpdate) {
        if (imageZoom * viewZoom < 1) {
            generateFullPixmapResized();
        } else {
            generateFullPixmapCutAndResized();
        }
        update();
    }
}
void SplicePicturesPanel::setZoomCenter(int x, int y, bool withUpdate) {
    zoomCenterX = std::min(std::max(x, minimumSizeHint().width() / 2), (int)(minimumSizeHint().width() * (viewZoom - 0.5f)));
    zoomCenterY = std::min(std::max(y, minimumSizeHint().height() / 2), (int)(minimumSizeHint().height() * (viewZoom - 0.5f)));
    if (withUpdate) {
        if (imageZoom * viewZoom < 1) {
            generateFullPixmapResizedAndCut();
        } else {
            generateFullPixmapCutAndResized();
        }
        update();
    }
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
double SplicePicturesPanel::getZoom() {
    return viewZoom;
}
QPoint SplicePicturesPanel::getZoomCenter() {
    return QPoint(zoomCenterX, zoomCenterY);
}

QPixmap *SplicePicturesPanel::getFullPixmap(bool repaint) {
    if (!repaint) return fullPixmap;
    int globalDrawStartX = INT_MAX, globalDrawStartY = INT_MAX;
    int globalDrawEndX = -INT_MAX, globalDrawEndY = -INT_MAX;
    QVector<QPixmap> pixmaps;
    QVector<double*> pixargs;
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        double drawWidth = (double)iter->getZoom() * (double)iter->getWidth();
        double drawHeight = (double)iter->getZoom() * (double)iter->getHeight();
        double drawOffsetX = iter->getX();
        double drawOffsetY = iter->getY();
        double drawStartX = iter->getCol() * uwidth / imageZoom + (uwidth / imageZoom - drawWidth) / 2 + drawOffsetX;
        double drawStartY = iter->getRow() * uheight / imageZoom + (uheight / imageZoom - drawHeight) / 2 + drawOffsetY;
        double drawCenterX = drawStartX + drawWidth / 2;
        double drawCenterY = drawStartY + drawHeight / 2;
        QPixmap transformedPix = iter->getTransformedPixmap();
        pixmaps.push_back(transformedPix);
        double resizedDrawWidth = transformedPix.size().width();
        double resizedDrawHeight = transformedPix.size().height();
        drawWidth = resizedDrawWidth;
        drawHeight = resizedDrawHeight;
        drawStartX = drawCenterX - drawWidth / 2;
        drawStartY = drawCenterY - drawHeight / 2;
        double drawEndX = drawStartX + drawWidth;
        double drawEndY = drawStartY + drawHeight;
        double *ap = new double[4];
        ap[0] = drawStartX;
        ap[1] = drawStartY;
        ap[2] = drawWidth;
        ap[3] = drawHeight;
        pixargs.push_back(ap);
        if (globalDrawStartX > drawStartX) globalDrawStartX  = drawStartX;
        if (globalDrawStartY > drawStartY) globalDrawStartY  = drawStartY;
        if (globalDrawEndX < drawEndX) globalDrawEndX  = drawEndX;
        if (globalDrawEndY < drawEndY) globalDrawEndY  = drawEndY;
    }
    QPixmap *pix = new QPixmap(globalDrawEndX - globalDrawStartX, globalDrawEndY - globalDrawStartY);
    pix->fill(Qt::transparent);
    QPainter painter(pix);
    QVector<QPixmap>::iterator iterp = pixmaps.begin();
    QVector<double*>::iterator itera = pixargs.begin();
    for (; iterp != pixmaps.end() && itera != pixargs.end(); iterp++, itera++) {
        QPixmap *pixmap = (QPixmap *)iterp;
        double *ap = *(double **)itera;
        double drawStartX = ap[0] - globalDrawStartX;
        double drawStartY = ap[1] - globalDrawStartY;
        double drawWidth = ap[2];
        double drawHeight = ap[3];
        painter.drawPixmap(drawStartX, drawStartY,
                           drawWidth, drawHeight,
                           *pixmap);
        delete []ap;
    }
    this->globalDrawStartX = globalDrawStartX;
    this->globalDrawStartY = globalDrawStartY;
    this->globalDrawWidth = pix->size().width();
    this->globalDrawHeight = pix->size().height();
    if (fullPixmap) delete fullPixmap;
    fullPixmap = pix;
    return fullPixmap;
}
void SplicePicturesPanel::redrawFullPixmap() {
    clock_t _ts = clock();
    getFullPixmap(true);
    clock_t _te = clock();
    if (imageZoom * viewZoom < 1) {
        generateFullPixmapResized();
    } else {
        generateFullPixmapCutAndResized();
    }
}
void SplicePicturesPanel::generateFullPixmapResized() {
    clock_t _ts = clock();
    if (!fullPixmap) return;
    double scaleRate = viewZoom * imageZoom;
    QMatrix matrix;
    matrix.scale(scaleRate, scaleRate);
    if (fullPixmapResized) delete fullPixmapResized;
    fullPixmapResized = new QPixmap(fullPixmap->transformed(matrix));
    clock_t _te = clock();
    generateFullPixmapResizedAndCut();
}
void SplicePicturesPanel::generateFullPixmapResizedAndCut() {
    clock_t _ts = clock();
    if (!fullPixmapResized) return;
    QSize imageSize = fullPixmapResized->size();
    double imageWidth = imageSize.width();
    double imageHeight = imageSize.height();
    double canvasWidth = minimumSizeHint().width();
    double canvasHeight = minimumSizeHint().height();
    double zoomOffsetX = canvasWidth / 2.0 - zoomCenterX;
    double zoomOffsetY = canvasHeight / 2.0 - zoomCenterY;
    double drawMarginWidth = marginRate * viewZoom * uwidth;
    double drawMarginHeight = marginRate * viewZoom * uheight;
    finalDrawStartX = 0;
    finalDrawStartY = 0;
    double cutStartX = - zoomOffsetX - drawMarginWidth - globalDrawStartX * viewZoom * imageZoom;
    double cutStartY = - zoomOffsetY - drawMarginHeight - globalDrawStartY * viewZoom * imageZoom;
    if (cutStartX < 0) { finalDrawStartX -= cutStartX; cutStartX = 0; }
    if (cutStartY < 0) { finalDrawStartY -= cutStartY; cutStartY = 0; }
    double finalDrawEndX = finalDrawStartX + imageWidth - cutStartX;
    double finalDrawEndY = finalDrawStartY + imageHeight - cutStartY;
    double cutEndX = imageWidth;
    double cutEndY = imageHeight;
    if (finalDrawEndX > canvasWidth) { cutEndX -= finalDrawEndX - canvasWidth; finalDrawEndX = canvasWidth; }
    if (finalDrawEndY > canvasHeight) { cutEndY -= finalDrawEndY - canvasHeight; finalDrawEndY = canvasHeight; }
    finalDrawWidth = cutEndX - cutStartX;
    finalDrawHeight = cutEndY - cutStartY;
    if (fullPixmapResizedAndCut) delete fullPixmapResizedAndCut;
    fullPixmapResizedAndCut = new QPixmap(fullPixmapResized->copy(cutStartX, cutStartY,
                                                                  finalDrawWidth, finalDrawHeight));
    finalPixmap = fullPixmapResizedAndCut;
    clock_t _te = clock();
}
void SplicePicturesPanel::generateFullPixmapCutAndResized() {
    clock_t _ts = clock();
    double scaleRate = imageZoom * viewZoom;
    if (!fullPixmap) return;
    QSize imageSize = fullPixmap->size();
    double imageWidth = imageSize.width();
    double imageHeight = imageSize.height();
    double canvasWidth = minimumSizeHint().width() / scaleRate;
    double canvasHeight = minimumSizeHint().height() / scaleRate;
    double zoomOffsetX = canvasWidth / 2.0 - zoomCenterX / scaleRate;
    double zoomOffsetY = canvasHeight / 2.0 - zoomCenterY / scaleRate;
    double drawMarginWidth = marginRate * uwidth / imageZoom;
    double drawMarginHeight = marginRate * uheight / imageZoom;
    finalDrawStartX = 0;
    finalDrawStartY = 0;
    double cutStartX = - zoomOffsetX - drawMarginWidth - globalDrawStartX;
    double cutStartY = - zoomOffsetY - drawMarginHeight - globalDrawStartY;
    if (cutStartX < 0) { finalDrawStartX -= cutStartX; cutStartX = 0; }
    if (cutStartY < 0) { finalDrawStartY -= cutStartY; cutStartY = 0; }
    double finalDrawEndX = finalDrawStartX + imageWidth - cutStartX;
    double finalDrawEndY = finalDrawStartY + imageHeight - cutStartY;
    double cutEndX = imageWidth;
    double cutEndY = imageHeight;
    if (finalDrawEndX > canvasWidth) { cutEndX -= finalDrawEndX - canvasWidth; finalDrawEndX = canvasWidth; }
    if (finalDrawEndY > canvasHeight) { cutEndY -= finalDrawEndY - canvasHeight; finalDrawEndY = canvasHeight; }
    finalDrawWidth = cutEndX - cutStartX;
    finalDrawHeight = cutEndY - cutStartY;
    finalDrawWidth = ceil(finalDrawWidth) + 1;
    finalDrawHeight = ceil(finalDrawHeight) + 1;
    if (finalDrawWidth < 0) finalDrawWidth = 0;
    if (finalDrawHeight < 0) finalDrawHeight = 0;
    if (fullPixmapCutAndResized) delete fullPixmapCutAndResized;
    QMatrix matrix;
    matrix.scale(scaleRate, scaleRate);
    QPixmap transPix = fullPixmap
            ->copy(cutStartX, cutStartY, finalDrawWidth, finalDrawHeight)
            .transformed(matrix);
    finalDrawStartX *= scaleRate;
    finalDrawStartY *= scaleRate;
    finalDrawWidth = finalDrawWidth * scaleRate;
    finalDrawHeight = finalDrawHeight * scaleRate;
    canvasWidth = canvasWidth * scaleRate;
    canvasHeight = canvasHeight * scaleRate;
    cutStartX = floor((cutStartX - floor(cutStartX)) * scaleRate);
    cutStartY = floor((cutStartY - floor(cutStartY)) * scaleRate);
    double cutWidth = transPix.size().width();
    double cutHeight = transPix.size().height();
    finalDrawEndX = finalDrawStartX + transPix.size().width();
    finalDrawEndY = finalDrawStartY + transPix.size().height();
    if (finalDrawEndX > canvasWidth) { cutWidth -= finalDrawEndX - canvasWidth; finalDrawEndX = canvasWidth; }
    if (finalDrawEndY > canvasHeight) { cutHeight -= finalDrawEndY - canvasHeight; finalDrawEndY = canvasHeight; }
    if (cutWidth < 0) cutWidth = 0;
    if (cutHeight < 0) cutHeight = 0;
    fullPixmapCutAndResized = new QPixmap(transPix.copy(cutStartX, cutStartY, cutWidth, cutHeight));
    finalDrawWidth = cutWidth;
    finalDrawHeight = cutHeight;
    finalPixmap = fullPixmapCutAndResized;
    clock_t _te = clock();
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

void SplicePicturesPanel::loadImage(int row, int col, QString filePath, bool removeIfExists, bool withUpdate) {
    if (removeIfExists) removeImage(row, col);
    SplicePicturesImageItem item(row, col, filePath);
    QImage *backgroundImage = getBackground(row, col);
    if (backgroundImage != NULL) {
        QImage dodgedImage = BackgroundDodging::dodgeBackground(*item.getImage(), *backgroundImage);
        item.setImage(new QImage(dodgedImage));
        item.setPixmap(new QPixmap(QPixmap::fromImage(dodgedImage)));
        dodgedImage.save(QString("/Users/cosmozhang/work/test/Background/export_dodged/%1.tiff").arg(row * 4 + col + 1));
    }
    SplicePicturesCalibrationItem *calibrationItem = getCalibrationItem(row, col);
    if (calibrationItem != NULL) {
        item.setRotation(calibrationItem->getRotation());
        item.setZoom(calibrationItem->getZoom());
        item.getPixmap()->save(QString("/Users/cosmozhang/work/test/Background/export_calibrated/%1.tiff").arg(row * 4 + col + 1));
    }
    imageList.push_back(item);
    if (imageZoom == 0.0f) {
        if ((double)item.getWidth() / (double)item.getHeight() < (double)uwidth / (double)uheight) {
            imageZoom = (double)uwidth / (double)item.getWidth();
        } else {
            imageZoom = (double)uheight / (double)item.getHeight();
        }
    }
    if (withUpdate) refresh();
}
bool SplicePicturesPanel::removeImage(int row, int col, bool withUpdate) {
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        if (iter->getRow() == row && iter->getCol() == col) {
            imageList.erase(iter);
            if (withUpdate) refresh();
            return true;
        }
    }
    return false;
}
bool SplicePicturesPanel::removeLastImage(bool withUpdate) {
    if (imageList.empty()) return false;
    imageList.pop_back();
    if (withUpdate) refresh();
    return true;
}
bool SplicePicturesPanel::removeAllImages(bool withUpdate) {
    if (imageList.empty()) return false;
    imageList.clear();
    if (withUpdate) refresh();
    return true;
}

void SplicePicturesPanel::refresh() {
    redrawFullPixmap();
    update();
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
    redrawFullPixmap();
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
            redrawFullPixmap();
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
    redrawFullPixmap();
    update();
}
void SplicePicturesPanel::rotate(Rational degree) {
    if (imageList.empty()) return;
    SplicePicturesImageItem *item = (SplicePicturesImageItem *)imageList.end() - 1;
    Rational rotation = item->getRotation();
    item->setRotation(rotation + degree);
    redrawFullPixmap();
    update();
}
void SplicePicturesPanel::zoom(Rational rate) {
    if (imageList.empty()) return;
    SplicePicturesImageItem *item = (SplicePicturesImageItem *)imageList.end() - 1;
    Rational zoom = item->getZoom();
    item->setZoom(zoom + rate);
    redrawFullPixmap();
    update();
}

void SplicePicturesPanel::doViewZoom(bool zoomIn) {
    double currentZoom = getZoom();
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

void SplicePicturesPanel::autoStitch(QSize overlap, QSize searchRegion, QSize featurePadding) {
    AutoStitch::stitchImages(imageList, overlap, searchRegion, featurePadding);
    int actualWidth = uwidth / imageZoom;
    int actualHeight = uheight / imageZoom;
    SplicePicturesImageItem ***items = new SplicePicturesImageItem **;
    for (int r = 0; r < rows; r++) {
        items[r] = new SplicePicturesImageItem *;
        for (int c = 0; c < cols; c++) items[r][c] = NULL;
    }
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        items[iter->getRow()][iter->getCol()] = (SplicePicturesImageItem *)iter;
    }
    for (int r = 0; r < rows; r++) {
        int accOffsetX = 0;
        int deltaX1 = actualWidth - items[r][0]->getImage()->size().width(), deltaX2;
        for (int c = 1; c < cols; c++) {
            if (c > 1) deltaX1 = deltaX2;
            SplicePicturesImageItem *item = items[r][c];
            deltaX2 = actualWidth - item->getImage()->size().width();
            accOffsetX += deltaX2 / 2 + deltaX1 / 2 + (int)((deltaX1 % 2) && (deltaX2 % 2));
            item->setX(item->getX() + accOffsetX);
        }
    }
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        qDebug() << QString("%1,%2  ---  x = %3, y = %4").arg(iter->getRow()).arg(iter->getCol()).arg(iter->getX()).arg(iter->getY());
    }
    update();
}
