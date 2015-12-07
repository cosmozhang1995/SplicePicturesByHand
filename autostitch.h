#ifndef AUTOSTITCH_H
#define AUTOSTITCH_H

//#define STITCH_SAVE_CORR

#include <QPoint>
#include <QSize>
#include <QVector>

#include "splicepicturesimageitem.h"

#define STITCH_CONVERT_TO_GRAY_METHOD_1
//#define STITCH_CONVERT_TO_GRAY_METHOD_2

class QImage;

class AutoStitch
{
public:
    enum StitchDirection {DIRECTION_X, DIRECTION_Y};
    enum StitchMode {ROW_BY_ROW, ROW_BY_ROW_CUT};
    static void stitchImages(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding, StitchMode mode = ROW_BY_ROW);
private:
    static QPoint corrMatch(QImage *img1, QImage *img2, QSize overlap, QSize searchRegion, QSize featureHalfSize, StitchDirection direction);
    static QPoint corrMatchWithFlag(QImage *img1, QImage *img2, QSize overlap, QSize searchRegion, QSize featureHalfSize, StitchDirection direction);
    static QPoint corrMatchWithFlag(QImage *img1, QImage *img2, QVector<QPoint> &featureRegion, QPoint searchCenterOffset, QSize searchRegion);
    static QImage convertToGrayImage(QImage &image);
    static void stitchImagesRowByRowWithCut(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding);
    static void stitchImagesRowByRow(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding);
};

class AutoStitchNoOverlapError
{
public:
    AutoStitchNoOverlapError(QPoint imagePos1, QPoint imagePos2, AutoStitch::StitchDirection direction);
    const QPoint imagePos1;
    const QPoint imagePos2;
    const AutoStitch::StitchDirection direction;
};

#endif // AUTOSTITCH_H
