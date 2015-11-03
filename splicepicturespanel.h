#ifndef SPLICEPICTURESPANEL_H
#define SPLICEPICTURESPANEL_H

#define ZOOM_RATE 1.5
#define MAX_ZOOM 160.0
#define MIN_ZOOM 1.0

#include <QWidget>

#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>

#include "rational.h"
#include "splicepicturesimageitem.h"
#include "splicepicturescalibrationitem.h"
#include "splicepicturesbackgrounditem.h"

class QSize;
class QPoint;
class QImage;
class QPixmap;

class SplicePicturesPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SplicePicturesPanel(int rows, int cols, int uwidth, int uheight, float marginRate = 1.0f, float viewZoom = 1.0f, QWidget *parent = 0);
    ~SplicePicturesPanel();
    void initialize();
    void drawLayout();
//    void setGrids(int rows, int cols);
//    void setUnitSize(int uwidth, int uheight);
    void setViewZoom(float viewZoom);
    void setZoomCenter(int x, int y);
    int getGridRows();
    int getGridCols();
    int getUnitWidth();
    int getUnitHeight();
    float getZoom();
    QPoint getZoomCenter();
    void loadImage(int row, int col, QString filePath, bool removeIfExists = true);
    bool removeImage(int row, int col);
    bool removeLastImage();
    bool removeAllImages();

    QJsonDocument getConfiguration();
    void loadConfiguration(QJsonDocument configuration);
    QByteArray getConfigurationAsData();
    void loadConfigurationFromData(QByteArray configuration);
    QString getConfigurationAsString();
    void loadConfigurationFromString(QString configuration);

    QJsonDocument getCalibration();
    void loadCalibration(QJsonDocument calibration);
    QByteArray getCalibrationAsData();
    void loadCalibrationFromData(QByteArray calibration);
    QString getCalibrationAsString();
    void loadCalibrationFromString(QString calibration);

    QVector<SplicePicturesBackgroundItem> & getBackgroundList();
    void setBackgroundList(QVector<SplicePicturesBackgroundItem> &list);

    SplicePicturesImageItem *selectImage(int row, int col);
    SplicePicturesImageItem *currentImage();
    QPixmap *getFullPixmap();
    QPixmap *getTransformedPixmap(int row, int col);
    void move(int x, int y);
    void rotate(Rational degree);
    void zoom(Rational rate);
    void doViewZoom(bool zoomIn = true);

    SplicePicturesCalibrationItem *getCalibrationItem(int row, int col);
    QImage * getBackground(int row, int col);
protected:
    void paintEvent(QPaintEvent *);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
signals:
    void onUnitClicked(int row, int col);
    void onDrag(int x1, int y1, int x2, int y2);
public slots:
    void onDragEvent(int x1, int y1, int x2, int y2);
private:
    int rows;
    int cols;
    int uwidth;
    int uheight;
    float marginRate;
    float viewZoom;
    float imageZoom;
    QVector<SplicePicturesImageItem> imageList;
    QVector<SplicePicturesCalibrationItem> calibrationList;
    QVector<SplicePicturesBackgroundItem> backgroundList;

    int _inDrag;
    int _dragPosX;
    int _dragPosY;
    int _didDrag;

    int zoomCenterX;
    int zoomCenterY;
};

#endif // SPLICEPICTURESPANEL_H
