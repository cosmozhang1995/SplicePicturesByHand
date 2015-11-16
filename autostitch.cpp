#include "autostitch.h"

#include <assert.h>
#include <math.h>
#include <limits.h>
#include <ctime>

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QPoint>
#include <QSize>
#include <QRgb>

QPoint AutoStitch::corrMatch(QImage *img1, QImage *img2, QSize overlap, QSize searchRegion, QSize featureHalfSize, StitchDirection direction) {
    assert(direction == DIRECTION_X || direction == DIRECTION_Y);

    static int tmpInt = 0;
    static double tmpDouble = 0.0;
    static int iterInt = 0;
    static int x = 0;
    static int y = 0;

    /* prepare immediat values */
    int searchRegionHalfWidth = searchRegion.width();
    int searchRegionHalfHeight = searchRegion.height();
    int searchRegionFullWidth = searchRegionHalfWidth * 2 + 1;
    int searchRegionFullHeight = searchRegionHalfHeight * 2 + 1;
    int featureHalfWidth = featureHalfSize.width();
    int featureHalfHeight = featureHalfSize.height();
    int featureFullWidth = featureHalfWidth * 2 + 1;
    int featureFullHeight = featureHalfHeight * 2 + 1;

    /* ensure overlap size is odd */
    if (overlap.width() % 2 == 0) overlap.setWidth(overlap.width() - 1);
    if (overlap.height() % 2 == 0) overlap.setHeight(overlap.height() - 1);

    /* calculate sizes */
    QImage image1 = img1->convertToFormat(QImage::Format_Grayscale8);
    QImage image2 = img2->convertToFormat(QImage::Format_Grayscale8);
    QSize imageSize1 = image1.size();
    QSize imageSize2 = image2.size();
    QPoint imageCenter, searchCenter;
    if (direction == DIRECTION_X) {
        imageCenter.setX(overlap.width() / 2);
        imageCenter.setY(imageSize2.height() / 2);
        searchCenter.setX(imageSize1.width() - (overlap.width() + 1) / 2);
        searchCenter.setY(imageSize1.height() / 2);
    } else {
        imageCenter.setX(imageSize2.width() / 2);
        imageCenter.setY(overlap.height() / 2);
        searchCenter.setX(imageSize1.width() / 2);
        searchCenter.setY(imageSize1.height() - (overlap.height() + 1) / 2);
    }

    /* fetch feature image */
    QImage featureImage = image2.copy(imageCenter.x() - featureHalfWidth,
                                      imageCenter.y() - featureHalfHeight,
                                      featureFullWidth, featureFullHeight);
    int featureImageDataLength = featureFullWidth * featureFullHeight;
    int *featureImageData = new int[featureImageDataLength];
    int *templateImageData = new int[featureImageDataLength];
    int featureMean = 0;
    iterInt = 0;
    for (y = 0; y < featureFullHeight; y++) {
        for (x = 0; x < featureFullWidth; x++) {
            tmpInt = qRed(featureImage.pixel(x, y));
            featureImageData[iterInt] = tmpInt;
            featureMean += tmpInt;
            iterInt++;
        }
    }
    featureMean = featureMean / featureImageDataLength;
//    qDebug() << "featureMean" << featureMean;
    long featureVar = 0;
    for (iterInt = 0; iterInt < featureImageDataLength; iterInt++) {
        tmpInt = featureImageData[iterInt] - featureMean;
        featureImageData[iterInt] = tmpInt;
        featureVar += tmpInt * tmpInt;
    }
//    qDebug() << "featureVar" << featureVar;

    /* initialize correlation paras */
#ifdef STITCH_SAVE_CORR
    double **corr = new double*[searchRegionFullWidth];
    for (iterInt = 0; iterInt < searchRegionFullWidth; iterInt++) corr[iterInt] = new double[searchRegionFullHeight];
#endif
    double corrMax = 0.0f;
    QPoint corrPoint;

    /* traverse to find max search */
    for (int sy = - searchRegionHalfHeight; sy <= searchRegionHalfHeight; sy++) {
        for (int sx = - searchRegionHalfWidth; sx <= searchRegionHalfWidth; sx++) {
            QImage templateImage = image1.copy(searchCenter.x() - featureHalfWidth + sx,
                                              searchCenter.y() - featureHalfHeight + sy,
                                              featureFullWidth, featureFullHeight);
//            templateImage.save(QString("/Users/cosmozhang/work/test/stitch_samples/output/%1 %2.bmp").arg(sx).arg(sy));
//            continue;
            int templateMean = 0;
            iterInt = 0;
            for (y = 0; y < featureFullHeight; y++) {
                for (x = 0; x < featureFullWidth; x++) {
                    tmpInt = qRed(templateImage.pixel(x, y));
                    templateImageData[iterInt] = tmpInt;
                    templateMean += tmpInt;
                    iterInt++;
                }
            }
            templateMean = templateMean / featureImageDataLength;
//            qDebug() << "templateMean" << templateMean;
            long templateVar = 0;
            double corrNumerator = 0.0;
            for (iterInt = 0; iterInt < featureImageDataLength; iterInt++) {
                tmpInt = templateImageData[iterInt] - templateMean;
//                templateImageData[iterInt] = tmpInt;
                templateVar += tmpInt * tmpInt;
                corrNumerator += tmpInt * featureImageData[iterInt];
            }
//            qDebug() << "templateVar" << templateVar;
            tmpDouble = corrNumerator / (sqrt(templateVar) * sqrt(featureVar));
//            qDebug() << "corr" << tmpDouble;
#ifdef STITCH_SAVE_CORR
            corr[sx + searchRegionHalfWidth][sy + searchRegionHalfHeight] = tmpDouble;
#endif
            if (corrMax < tmpDouble) {
                corrMax = tmpDouble;
                corrPoint.setX(sx);
                corrPoint.setY(sy);
            }
        }
    }

    delete []featureImageData;
    delete []templateImageData;
#ifdef STITCH_SAVE_CORR
    for (iterInt = 0; iterInt < searchRegionFullWidth; iterInt++) delete [](corr[iterInt]);
    delete []corr;
#endif
    return corrPoint;
}

void AutoStitch::stitchImages(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding) {
    /* calculate total rows and columns */
    qDebug() << "statistics start";
    int rows = 0, cols = 0;
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int row = iter->getRow(),
            col = iter->getCol();
        if (rows < row) rows = row;
        if (cols < col) cols = col;
    }
    rows++; cols++;
    qDebug() << "statistics end";

    /* prepare item array */
    qDebug() << "item array prepare start";
    SplicePicturesImageItem ***items = new SplicePicturesImageItem**[rows];
    for (int r = 0; r < rows; r++) { items[r] = new SplicePicturesImageItem*[cols]; for (int c = 0; c < cols; c++) items[r][c] = NULL; }
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int row = iter->getRow(),
            col = iter->getCol();
        items[row][col] = (SplicePicturesImageItem *)iter;
    }
    qDebug() << "item array prepare end";

    /* stitch images for each row */
    qDebug() << "stitch single start";
    QPoint ***offsets = new QPoint**[rows];
    for (int r = 0; r < rows; r++) { offsets[r] = new QPoint*[cols - 1]; for (int c = 0; c < cols - 1; c++) offsets[r][c] = NULL; }
    QImage image1, image2;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 1; c++) {
            qDebug() << QString("stitch: %1, %2").arg(r).arg(c);
            if (c == 0) image1 = items[r][c]->getTransformedImage(items[r][c]->getImage()->size());
            else image1 = image2;
            image2 = items[r][c + 1]->getTransformedImage(items[r][c + 1]->getImage()->size());
            int height1 = image1.size().height(), height2 = image2.size().height();
            int height = height1 > height2 ? height2 : height1;
            QSize featureHalfSizeRegion((overlap.width() - 1)/2 - searchRegion.width(), height/2 - searchRegion.height() - featurePadding.height());
            offsets[r][c] = new QPoint(corrMatch(&image1, &image2, overlap, searchRegion, featureHalfSizeRegion, DIRECTION_X));
        }
    }
    qDebug() << "stitch single end";

    /* prepare row images */
    qDebug() << "prepare rows start";
    QPixmap **rowPixmaps = new QPixmap*[rows]; for (int r = 0; r < rows; r++) rowPixmaps[r] = NULL;
    for (int r = 0; r < rows; r++) {
        int
//            maxOffsetX = 0, maxOffsetY = 0,
//            minOffsetX = 0, minOffsetY = 0,
            totalOffsetX = 0, totalOffsetY = 0,
            maxUp = 0, minDown = INT_MAX,
            totalImageWidth = 0, totalImageHeight = 0;
        for (int c = 0; c < cols; c++) {
            QSize imageSize = items[r][c]->getImage()->size();
            totalImageWidth += imageSize.width();
            totalImageHeight += imageSize.height();
            if (c > 0) {
                QPoint *offset = offsets[r][c - 1];
                int offsetX = offset->x(), offsetY = offset->y();
//                if (offsetX < minOffsetX) minOffsetX = offsetX;
//                else if (offsetX > maxOffsetX) maxOffsetX = offsetX;
//                if (offsetY < minOffsetY) minOffsetY = offsetY;
//                else if (offsetY > maxOffsetY) maxOffsetY = offsetY;
                int up = offsetY, down = offsetY + imageSize.height();
                if (maxUp < up) maxUp = up;
                if (minDown > down) minDown = down;
                totalOffsetX += offsetX;
                totalOffsetY += offsetY;
            }
        }
        int rowHeight = minDown - maxUp;
//        if (minOffsetY > 0) minOffsetY = 0;
//        if (maxOffsetY < 0) maxOffsetY = 0;
        rowPixmaps[r] = new QPixmap(totalImageWidth - (cols - 1) * overlap.width() + totalOffsetX, rowHeight);
        QPainter painter(rowPixmaps[r]);
        totalImageWidth = 0;
        for (int c = 0; c < cols; c++) {
            QImage image = items[r][c]->getTransformedImage(items[r][c]->getImage()->size());
            QPoint offset = (c == 0) ? QPoint(0, 0) : *(offsets[r][c - 1]);
            painter.drawImage(totalImageWidth, offset.y() - maxUp, image);
            totalImageWidth += offset.x() - overlap.width();
        }
    }
    qDebug() << "prepare rows end";

    /* stitch row images */
    qDebug() << "stitch rows start";
    QPoint **rowOffsets = new QPoint*[rows - 1]; for (int r = 0; r < rows - 1; r++) rowOffsets[r] = NULL;
    int rowOffset = 0;
    int minRowOffset = 0;
    for (int r = 0; r < rows - 1; r++) {
        if (r == 0) image1 = rowPixmaps[r]->toImage();
        else image1 = image2;
        image2 = rowPixmaps[r + 1]->toImage();
        int width1 = image1.size().width(), width2 = image2.size().width();
        int width = width1 > width2 ? width2 : width1;
        QSize featureHalfSizeRegion(width/2 - searchRegion.width() - featurePadding.width(), (overlap.height() - 1)/2 - searchRegion.height());
        QPoint *offset = new QPoint(corrMatch(&image1, &image2, overlap, searchRegion, featureHalfSizeRegion, DIRECTION_Y));
        rowOffset = rowOffset + (width1/2 - width2/2);
        if (minRowOffset > rowOffset) minRowOffset = rowOffset;
        offset->setX(offset->x() + rowOffset);
        rowOffsets[r] = offset;
    }
    qDebug() << "stitch rows end";

    /* apply the results */
    qDebug() << "apply start";
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int row = iter->getRow(), col = iter->getCol();
        QPoint rowOffset = (row == 0 ? QPoint() : *rowOffsets[row - 1]);
        QPoint offset = (col == 0 ? QPoint() : *offsets[row][col - 1]);
        iter->setX(offset.x() + rowOffset.x() - minRowOffset - overlap.width() * col);
        iter->setY(offset.y() + rowOffset.y() - overlap.height() * row);
    }
    qDebug() << "apply end";

    /* memory recycle */
    qDebug() << "memcycle start";
    for (int r = 0; r < rows; r++) delete [](items[r]);
    delete []items;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 1; c++) delete offsets[r][c];
        delete [](offsets[r]);
    }
    delete []offsets;
    for (int r = 0; r < rows; r++) delete rowPixmaps[r];
    delete [](rowPixmaps);
    for (int r = 0; r < rows - 1; r++) delete rowOffsets[r];
    delete [](rowOffsets);
    qDebug() << "memcycle end";
}

