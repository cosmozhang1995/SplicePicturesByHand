#ifndef AUTOSTITCH_H
#define AUTOSTITCH_H

#define STITCH_SAVE_CORR

#include <QPoint>
#include <QSize>
#include <QVector>

#include "splicepicturesimageitem.h"

class QImage;

class AutoStitch
{
public:
    enum StitchDirection {DIRECTION_X, DIRECTION_Y};
    static QPoint corrMatch(QImage *img1, QImage *img2, QSize overlap, QSize searchRegion, QSize featureHalfSize, StitchDirection direction);
    static void stitchImages(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding);
};

#endif // AUTOSTITCH_H
