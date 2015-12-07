#include "autostitch.h"

#include <assert.h>
#include <math.h>
#include <limits.h>
#include <ctime>

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRegion>
#include <QPainter>
#include <QPoint>
#include <QSize>
#include <QRgb>

QImage AutoStitch::convertToGrayImage(QImage &image) {
    int width = image.width(), height = image.height();
#ifdef STITCH_CONVERT_TO_GRAY_METHOD_1
    /* Method 1 */
    QImage finalImage(width,height,QImage::Format_RGBA8888);
    QImage grayImage = image.convertToFormat(QImage::Format_Grayscale8);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int g = qRed(grayImage.pixel(x,y));
            int a = qAlpha(image.pixel(x,y));
            finalImage.setPixel(x,y,qRgba(g,g,g,a));
        }
    }
    return finalImage;
#endif
#ifdef STITCH_CONVERT_TO_GRAY_METHOD_2
    /* Method 2 */
    QPixmap finalPix(width,height);
    finalPix.fill(Qt::transparent);
    QPainter finalPainter(&finalPix);
    finalPainter.setClipRegion(QRegion(QPixmap::fromImage(image.createAlphaMask())));
    finalPainter.drawImage(0,0,image.convertToFormat(QImage::Format_Grayscale8));
    return finalPix.toImage();
#endif
}

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
            long templateVar = 0;
            double corrNumerator = 0.0;
            for (iterInt = 0; iterInt < featureImageDataLength; iterInt++) {
                tmpInt = templateImageData[iterInt] - templateMean;
                templateVar += tmpInt * tmpInt;
                corrNumerator += tmpInt * featureImageData[iterInt];
            }
            tmpDouble = corrNumerator / (sqrt(templateVar) * sqrt(featureVar));
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

QPoint AutoStitch::corrMatchWithFlag(QImage *img1, QImage *img2, QSize overlap, QSize searchRegion, QSize featureHalfSize, StitchDirection direction) {
    assert(direction == DIRECTION_X || direction == DIRECTION_Y);

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
    QImage image1 = convertToGrayImage(*img1);
    QImage image2 = convertToGrayImage(*img2);
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
    int dataLength = featureFullWidth * featureFullHeight;
    double *featureImageData = new double[dataLength];
    double *templateImageData = new double[dataLength];
    bool *flagData = new bool[dataLength];

    /* initialize correlation paras */
#ifdef STITCH_SAVE_CORR
    double **corr = new double*[searchRegionFullWidth];
    for (iterInt = 0; iterInt < searchRegionFullWidth; iterInt++) corr[iterInt] = new double[searchRegionFullHeight];
#endif
    double corrMax = 0.0f;
    QPoint corrPoint;

    /* traverse to find max search */
//    featureImage.save(QString("/Users/cosmozhang/work/test/stitch_output/feature.tiff"));
    for (int sy = - searchRegionHalfHeight; sy <= searchRegionHalfHeight; sy++) {
        for (int sx = - searchRegionHalfWidth; sx <= searchRegionHalfWidth; sx++) {
            QImage templateImage = image1.copy(searchCenter.x() - featureHalfWidth + sx,
                                              searchCenter.y() - featureHalfHeight + sy,
                                              featureFullWidth, featureFullHeight);
//            templateImage.save(QString("/Users/cosmozhang/work/test/stitch_output/%1 %2.tiff").arg(sx).arg(sy));
            double templateMean = 0.0;
            double featureMean = 0.0;
            QRgb rgbTemplate, rgbFeature;
            iterInt = 0;
            int validDataLength = 0;
            for (y = 0; y < featureFullHeight; y++) {
                for (x = 0; x < featureFullWidth; x++) {
                    rgbTemplate = templateImage.pixel(x, y);
                    rgbFeature = featureImage.pixel(x, y);
                    if (qAlpha(rgbTemplate) == 255 && qAlpha(rgbFeature) == 255) {
                        flagData[iterInt] = true;
                        tmpDouble = qRed(rgbTemplate);
                        templateImageData[iterInt] = tmpDouble;
                        templateMean += tmpDouble;
                        tmpDouble = qRed(rgbFeature);
                        featureImageData[iterInt] = tmpDouble;
                        featureMean += tmpDouble;
                        validDataLength++;
                    } else {
                        flagData[iterInt] = false;
                    }
                    iterInt++;
                }
            }
            templateMean = templateMean / validDataLength;
            featureMean = featureMean / validDataLength;
            double templateVar = 0.0;
            double featureVar = 0.0;
            double corrNumerator = 0.0;
            for (iterInt = 0; iterInt < dataLength; iterInt++) {
                if (!flagData[iterInt]) continue;
                templateImageData[iterInt] -= templateMean;
                templateVar += templateImageData[iterInt] * templateImageData[iterInt];
                featureImageData[iterInt] -= featureMean;
                featureVar += featureImageData[iterInt] * featureImageData[iterInt];
                corrNumerator += templateImageData[iterInt] * featureImageData[iterInt];
            }
            tmpDouble = corrNumerator / (sqrt(templateVar) * sqrt(featureVar));
//            qDebug() << tmpDouble;
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
    delete []flagData;
#ifdef STITCH_SAVE_CORR
    for (iterInt = 0; iterInt < searchRegionFullWidth; iterInt++) delete [](corr[iterInt]);
    delete []corr;
#endif
    return corrPoint;
}

/**
 * @brief find the most match position of two images, feature region are given manually.
 * @param img1
 * @param img2
 * @param featureRegion: A vector that contains all the points of the feature region. The feature region is the region-to-match of img2.
 * @param searchCenterOffset: A point that specify the offset to calculate the standard template region. The template region is the region-to-match of img1. This region's shape is the same as the feature region. It will be cut differently as the feature region floats (to match the latter). The standard template region is the template region when the feature region floats at it's floating center. At this time, a point in the template region, p1, is calculate from the corresponding point in the feature region, p2, by the formula: p1 = p2 + searchCenterOffset
 * @param searchRegion: A size that specifies the half size of search range.
 * @return A point that represents the feature region's offset from it's floating center.
 */
QPoint AutoStitch::corrMatchWithFlag(QImage *img1, QImage *img2, QVector<QPoint> &featureRegion, QPoint searchCenterOffset, QSize searchRegion) {
    static double tmpDouble = 0.0;

    /* prepare immediat values */
    int searchRegionHalfWidth = searchRegion.width();
    int searchRegionHalfHeight = searchRegion.height();
    int searchRegionFullWidth = searchRegionHalfWidth * 2 + 1;
    int searchRegionFullHeight = searchRegionHalfHeight * 2 + 1;

    /* calculate sizes */
    QImage image1 = convertToGrayImage(*img1);
    QImage image2 = convertToGrayImage(*img2);

    /* initialize feature region data memory */
    int featureMinX = INT_MAX, featureMaxX = -INT_MAX,
        featureMinY = INT_MAX, featureMaxY = -INT_MAX;
    for (QVector<QPoint>::iterator iter = featureRegion.begin(); iter != featureRegion.end(); iter++) {
        if (featureMinX > iter->x()) featureMinX = iter->x();
        if (featureMinY > iter->y()) featureMinY = iter->y();
        if (featureMaxX < iter->x()) featureMaxX = iter->x();
        if (featureMaxY < iter->y()) featureMaxY = iter->y();
    }
    int featureFullWidth = featureMaxX - featureMinX + 1;
    int featureFullHeight = featureMaxX - featureMinX + 1;
    double **featureImageData = new double*[featureFullWidth];
    for (int i = 0; i < featureFullWidth; i++) { featureImageData[i] = new double[featureFullHeight]; featureImageData[i] -= featureMinY; }
    featureImageData -= featureMinX;
    double **templateImageData = new double*[featureFullWidth];
    for (int i = 0; i < featureFullWidth; i++) { templateImageData[i] = new double[featureFullHeight]; templateImageData[i] -= featureMinY; }
    templateImageData -= featureMinX;
    bool **flagData = new bool*[featureFullWidth];
    for (int i = 0; i < featureFullWidth; i++) { flagData[i] = new bool[featureFullHeight]; flagData[i] -= featureMinY; }
    flagData -= featureMinX;

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
            QPoint searchOffset(sx, sy);
            double templateMean = 0.0;
            double featureMean = 0.0;
            QRgb rgbTemplate, rgbFeature;
            int validDataLength = 0;
            QPoint point1;
            int x, y;
            for (QVector<QPoint>::iterator iter = featureRegion.begin(); iter != featureRegion.end(); iter++) {
                x = iter->x(); y = iter->y();
                point1 = (*iter) + searchCenterOffset + searchOffset;
                rgbTemplate = image1.pixel(point1.x(), point1.y());
                rgbFeature = image2.pixel(x, y);
                if (qAlpha(rgbTemplate) == 255 && qAlpha(rgbFeature) == 255) {
                    flagData[x][y] = true;
                    tmpDouble = qRed(rgbTemplate);
                    templateImageData[x][y] = tmpDouble;
                    templateMean += tmpDouble;
                    tmpDouble = qRed(rgbFeature);
                    featureImageData[x][y] = tmpDouble;
                    featureMean += tmpDouble;
                    validDataLength++;
                } else {
                    flagData[x][y] = false;
                }
            }
            templateMean = templateMean / validDataLength;
            featureMean = featureMean / validDataLength;
            double templateVar = 0.0;
            double featureVar = 0.0;
            double corrNumerator = 0.0;
            for (QVector<QPoint>::iterator iter = featureRegion.begin(); iter != featureRegion.end(); iter++) {
                x = iter->x(); y = iter->y();
                if (!flagData[x][y]) continue;
                templateImageData[x][y] -= templateMean;
                templateVar += templateImageData[x][y] * templateImageData[x][y];
                featureImageData[x][y] -= featureMean;
                featureVar += featureImageData[x][y] * featureImageData[x][y];
                corrNumerator += templateImageData[x][y] * featureImageData[x][y];
            }
            tmpDouble = corrNumerator / (sqrt(templateVar) * sqrt(featureVar));
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

    featureImageData += featureMinX;
    for (int i = 0; i < featureFullWidth; i++) { featureImageData[i] += featureMinY; delete [](featureImageData[i]); }
    delete []featureImageData;
    templateImageData += featureMinX;
    for (int i = 0; i < featureFullWidth; i++) { templateImageData[i] += featureMinY; delete [](templateImageData[i]); }
    delete []templateImageData;
    flagData += featureMinX;
    for (int i = 0; i < featureFullWidth; i++) { flagData[i] += featureMinY; delete [](flagData[i]); }
    delete []flagData;
#ifdef STITCH_SAVE_CORR
    for (iterInt = 0; iterInt < searchRegionFullWidth; iterInt++) delete [](corr[iterInt]);
    delete []corr;
#endif
    return corrPoint;
}

void AutoStitch::stitchImages(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding, StitchMode mode) {
    if (mode == ROW_BY_ROW_CUT) stitchImagesRowByRowWithCut(imageList, overlap, searchRegion, featurePadding);
    else if (mode == ROW_BY_ROW) stitchImagesRowByRow(imageList, overlap, searchRegion, featurePadding);
}

void AutoStitch::stitchImagesRowByRowWithCut(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding) {
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
            if (c == 0) image1 = items[r][c]->getTransformedImage(items[r][c]->getImage()->size());
            else image1 = image2;
            image2 = items[r][c + 1]->getTransformedImage(items[r][c + 1]->getImage()->size());
            int height1 = image1.size().height(), height2 = image2.size().height();
            int height = height1 > height2 ? height2 : height1;
            QSize featureHalfSizeRegion((overlap.width() - 1)/2 - searchRegion.width(), height/2 - searchRegion.height() - featurePadding.height());
            offsets[r][c] = new QPoint(corrMatchWithFlag(&image1, &image2, overlap, searchRegion, featureHalfSizeRegion, DIRECTION_X));
        }
    }
    qDebug() << "stitch single end";

    /* prepare row images */
    qDebug() << "prepare rows start";
    QPixmap **rowPixmaps = new QPixmap*[rows]; for (int r = 0; r < rows; r++) rowPixmaps[r] = NULL;
    int *maxUps = new int[rows];
    int *minDowns = new int[rows];
    for (int r = 0; r < rows; r++) {
        int totalOffsetX = 0, totalOffsetY = 0,
            maxUp = 0, minDown = INT_MAX,
            totalImageWidth = 0, totalImageHeight = 0;
        for (int c = 0; c < cols; c++) {
            QSize imageSize = items[r][c]->getImage()->size();
            totalImageWidth += imageSize.width();
            totalImageHeight += imageSize.height();
//            if (c > 0) {
//                QPoint *offset = offsets[r][c - 1];
//                int offsetX = offset->x(), offsetY = offset->y();
                int offsetX = c > 0 ? offsets[r][c - 1]->x() : 0,
                    offsetY = c > 0 ? offsets[r][c - 1]->y() : 0;
                int up = offsetY, down = offsetY + imageSize.height() - 1;
                if (maxUp < up) maxUp = up;
                if (minDown > down) minDown = down;
                totalOffsetX += offsetX;
                totalOffsetY += offsetY;
//            }
        }
        maxUps[r] = maxUp;
        minDowns[r] = minDown;
        int rowHeight = minDown - maxUp + 1;
        rowPixmaps[r] = new QPixmap(totalImageWidth - (cols - 1) * overlap.width() + totalOffsetX, rowHeight);
        QPainter painter(rowPixmaps[r]);
        totalImageWidth = 0;
        for (int c = 0; c < cols; c++) {
            QImage image = items[r][c]->getTransformedImage(items[r][c]->getImage()->size());
            QPoint offset = (c == 0) ? QPoint(0, 0) : *(offsets[r][c - 1]);
            totalImageWidth += offset.x();
            painter.drawImage(totalImageWidth, offset.y() - maxUp, image);
            totalImageWidth += items[r][c]->getImage()->size().width() - overlap.width();
        }
    }
    qDebug() << "prepare rows end";

    /* stitch row images */
    qDebug() << "stitch rows start";
    QPoint **rowOffsets = new QPoint*[rows - 1]; for (int r = 0; r < rows - 1; r++) rowOffsets[r] = NULL;
    int rowOffsetX = 0, rowOffsetY = 0;
    int minRowOffset = 0;
    QImage image1cut, image2cut;
    for (int r = 0; r < rows - 1; r++) {
        if (r == 0) image1 = rowPixmaps[r]->toImage();
        else image1 = image2;
        image2 = rowPixmaps[r + 1]->toImage();
        int maxUp1 = maxUps[r],     minDown1 = minDowns[r],
            maxUp2 = maxUps[r + 1], minDown2 = minDowns[r + 1];
        /* WARNING: potential problem here ! */
        rowOffsetY += maxUp1 + image1.size().height() - 1 - minDown1 - maxUp2;
        int width1 = image1.size().width(), width2 = image2.size().width();
        int width = width1 > width2 ? width2 : width1;
        image1cut = image1.copy(0, 0, width, image1.size().height());
        image2cut = image2.copy(0, 0, width, image2.size().height());
        image1cut.save("/Users/cosmozhang/Desktop/1.tiff");
        image2cut.save("/Users/cosmozhang/Desktop/2.tiff");
        QSize featureHalfSizeRegion(width/2 - searchRegion.width() - featurePadding.width(), (overlap.height() - 1)/2 - searchRegion.height());
        QPoint *offset = new QPoint(corrMatchWithFlag(&image1cut, &image2cut, overlap, searchRegion, featureHalfSizeRegion, DIRECTION_Y));
//        rowOffsetX = rowOffsetX + (width1/2 - width2/2);
//        if (minRowOffset > rowOffset) minRowOffset = rowOffsetX;
//        offset->setX(offset->x() + rowOffsetX);
        offset->setY(offset->y() + rowOffsetY);
        rowOffsets[r] = offset;
    }
    qDebug() << "stitch rows end";

    /* apply the results */
    qDebug() << "apply start";
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int row = iter->getRow(), col = iter->getCol();
        QPoint rowOffset = (row == 0 ? QPoint(0,0) : *rowOffsets[row - 1]);
        QPoint offset = (col == 0 ? QPoint(0,0) : *offsets[row][col - 1]);
//        iter->setX(offset.x() + rowOffset.x() - minRowOffset - overlap.width() * col);
        iter->setX(offset.x() + rowOffset.x() - overlap.width() * col);
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
    delete []maxUps;
    delete []minDowns;
    qDebug() << "memcycle end";
}

void AutoStitch::stitchImagesRowByRow(QVector<SplicePicturesImageItem> &imageList, QSize overlap, QSize searchRegion, QSize featurePadding) {
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
    QImage ***images = new QImage**[rows];
    for (int r = 0; r < rows; r++) { images[r] = new QImage*[cols]; for (int c = 0; c < cols; c++) images[r][c] = NULL; }
    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
        int row = iter->getRow(),
            col = iter->getCol();
        items[row][col] = (SplicePicturesImageItem *)iter;
        images[row][col] = new QImage(iter->getTransformedImage());
    }
    qDebug() << "item array prepare end";

    /* stitch images for each row */
    qDebug() << "stitch single start";
    QPoint **offsets = new QPoint*[rows]; for (int r = 0; r < rows; r++) offsets[r] = new QPoint[cols - 1];
    QSize **realOverlaps = new QSize*[rows]; for (int r = 0; r < rows; r++) realOverlaps[r] = new QSize[cols - 1];
    for (int r = 0; r < rows; r++) {
        int absoluteYOffset = 0;
        for (int c = 0; c < cols - 1; c++) {
            QImage *image1 = images[r][c];
            QImage *image2 = images[r][c + 1];
            int height1 = image1->size().height(), height2 = image2->size().height();
            int height = height1 > height2 ? height2 : height1;
            absoluteYOffset += height1 / 2 - height2 / 2;
            QSize featureHalfSizeRegion((overlap.width() - 1)/2 - searchRegion.width(), height/2 - searchRegion.height() - featurePadding.height());
            int width = image1->size().width() - items[r][c]->getWidth() + image2->size().width() - items[r][c + 1]->getWidth();
            int realOverlapWidth = overlap.width() + (width / 4 + 1) * 2;
            if (realOverlapWidth / 2 <= featureHalfSizeRegion.width()) {
                for (int r = 0; r < rows; r++) delete [](items[r]);
                delete []items;
                for (int r = 0; r < rows; r++) {
                    for (int c = 0; c < cols - 1; c++) delete images[r][c];
                    delete [](images[r]);
                }
                delete []images;
                for (int r = 0; r < rows; r++) delete [](offsets[r]);
                delete []offsets;
                for (int r = 0; r < rows; r++) delete [](realOverlaps[r]);
                delete []realOverlaps;
                throw AutoStitchNoOverlapError(QPoint(r,c), QPoint(r,c+1), DIRECTION_X);
            }
            QSize realOverlap = QSize(realOverlapWidth, overlap.height());
            realOverlaps[r][c] = realOverlap;
            QSize searchRegion((realOverlap.width() - 1)/2 - featureHalfSizeRegion.width(), height/2 - featureHalfSizeRegion.height() - featurePadding.height());
            offsets[r][c] = QPoint(corrMatchWithFlag(image1, image2, realOverlap, searchRegion, featureHalfSizeRegion, DIRECTION_X));
            offsets[r][c].setY(offsets[r][c].y() + absoluteYOffset);
        }
    }
    qDebug() << "stitch single end";

    /* prepare row images */
    qDebug() << "prepare rows start";
    QPixmap **rowPixmaps = new QPixmap*[rows]; for (int r = 0; r < rows; r++) rowPixmaps[r] = NULL;
    int *minUps = new int[rows];
    int *maxUps = new int[rows];
    int *minDowns = new int[rows];
    int *maxDowns = new int[rows];
    for (int r = 0; r < rows; r++) {
        int totalOffsetX = 0, totalOffsetY = 0,
            minUp = INT_MAX, maxUp = 0,
            minDown = INT_MAX, maxDown = 0,
            totalImageWidth = 0, totalImageHeight = 0,
            totalOverlapWidth = 0;
        for (int c = 0; c < cols; c++) {
            QSize imageSize = images[r][c]->size();
            totalImageWidth += imageSize.width();
            totalImageHeight += imageSize.height();
            if (c < cols - 1) totalOverlapWidth += realOverlaps[r][c].width();
            int offsetX = c > 0 ? offsets[r][c - 1].x() : 0,
                offsetY = c > 0 ? offsets[r][c - 1].y() : 0;
            int up = offsetY, down = offsetY + imageSize.height() - 1;
            if (maxUp < up) maxUp = up;
            if (minUp > up) minUp = up;
            if (minDown > down) minDown = down;
            if (maxDown < down) maxDown = down;
            totalOffsetX += offsetX;
            totalOffsetY += offsetY;
        }
        minUps[r] = minUp;
        maxDowns[r] = maxDown;
        int rowHeight = maxDown - minUp + 1;
        rowPixmaps[r] = new QPixmap(totalImageWidth - totalOverlapWidth + totalOffsetX, rowHeight);
        rowPixmaps[r]->fill(Qt::transparent);
        QPainter painter(rowPixmaps[r]);
        totalImageWidth = 0;
        for (int c = 0; c < cols; c++) {
            QImage *image = images[r][c];
            QPoint offset = (c == 0) ? QPoint(0, 0) : offsets[r][c - 1];
            QSize realOverlap = (c >= cols - 1) ? QSize(0,0) : realOverlaps[r][c];
            totalImageWidth += offset.x();
            painter.drawImage(totalImageWidth, offset.y() - minUp, *image);
            totalImageWidth += image->width() - realOverlap.width();
        }
//        rowPixmaps[r]->save(QString("/Users/cosmozhang/Desktop/%1.tiff").arg(r));
    }
    qDebug() << "prepare rows end";

    /* calculate inter-row feature regions */
    qDebug() << "calculate inter-row feature regions start";
    QPoint *searchCenterOffsets = new QPoint[rows - 1];
    QVector<QPoint> *featureRegions = new QVector<QPoint>[rows - 1];
    QSize *realRowOverlaps = new QSize[rows - 1];
    for (int r = 0; r < rows - 1; r++) {
        int     top1 = 0,    top2 = 0,
                left1 = 0,   left2 = 0,
                bottom1 = 0, bottom2 = 0,
                right1 = 0,  right2 = 0; // image boundary
        int topStd2 = 0;
        int top, left, bottom, right; // overlap boundary
        int lastTop = -1, lastLeft = -1, lastBottom = -1, lastRight = -1;
        for (int c = 0; c < cols; c++) {
            top2 = (c > 0 ? offsets[r + 1][c - 1].y() : 0) - minUps[r + 1];
            if (c == 0) topStd2 = top2 + (images[r + 1][0]->size().height() - items[r + 1][0]->getHeight()) / 2;
            left2 = left2 + (c > 0 ? offsets[r + 1][c - 1].x() : 0) + (c > 0 ? images[r + 1][c - 1]->size().width() - realOverlaps[r + 1][c - 1].width() : 0);
            left1 = (c > 0 ?
                         left1 + images[r][c - 1]->size().width() - realOverlaps[r][c - 1].width() :
                         (images[r + 1][0]->size().width() - items[r + 1][0]->getWidth()) / 2
                                - (images[r][0]->size().width() - items[r][0]->getWidth()) / 2
                    ) + (c > 0 ? offsets[r][c - 1].x() : 0);
            bottom1 = topStd2
                        + overlap.height()
                        + (images[r][0]->size().height() - items[r][0]->getHeight() + 1) / 2
                        + (c > 0 ? offsets[r][c - 1].y() : 0) - 1;
            right1 = left1 + images[r][c]->size().width() - 1;
            right2 = left2 + images[r + 1][c]->size().width() - 1;
            top1 = bottom1 - images[r][c]->size().height() + 1;
            bottom2 = top2 + images[r + 1][c]->size().height() - 1;
//            qDebug() << top2 << bottom1 << topStd2 << overlap.height() << (images[r][0]->size().height() - items[r][0]->getHeight() + 1) / 2 + (c > 0 ? offsets[r][c - 1].y() : 0) - 1;
            if (c == 0) {
                searchCenterOffsets[r].setX(-left1);
                searchCenterOffsets[r].setY(images[r][c]->size().height() - bottom1 - minUps[r] - 1);
            }
//            qDebug() << (c > 0 ? offsets[r + 1][c - 1].y() : 0) << - minUps[r + 1];
//            qDebug() << left1 << right1 << top1 << bottom1;
//            qDebug() << left2 << right2 << top2 << bottom2;
            top = fmax(top1, top2);
            bottom = fmin(bottom1, bottom2);
            left = fmax(left1, left2);
            right = fmin(right1, right2);
            if (c == 0) realRowOverlaps[r] = QSize(right - left + 1, bottom - top + 1);
            top += searchRegion.height();
            bottom -= searchRegion.height();
            left += searchRegion.width();
            right -= searchRegion.height();
//            qDebug() << left << right << top << bottom;
//            qDebug() << left + searchCenterOffsets->x() << right + searchCenterOffsets->x()
//                     << top + searchCenterOffsets->y() << bottom + searchCenterOffsets->y();
//            qDebug() << "";
            if (top >= bottom || left >= right) {
                for (int r = 0; r < rows; r++) delete [](items[r]);
                delete []items;
                for (int r = 0; r < rows; r++) {
                    for (int c = 0; c < cols - 1; c++) delete images[r][c];
                    delete [](images[r]);
                }
                delete []images;
                for (int r = 0; r < rows; r++) delete [](offsets[r]);
                delete []offsets;
                for (int r = 0; r < rows; r++) delete [](realOverlaps[r]);
                delete []realOverlaps;
                delete []featureRegions;
                delete []realRowOverlaps;
                for (int r = 0; r < rows; r++) delete rowPixmaps[r];
                delete []rowPixmaps;
                delete []minUps;
                delete []maxUps;
                delete []minDowns;
                delete []maxDowns;
                throw AutoStitchNoOverlapError(QPoint(r,0), QPoint(r+1,0), DIRECTION_Y);
            }
            for (int x = left; x <= right; x++) {
                for (int y = top; y <= bottom; y++) {
                    if (x < lastLeft || x > lastRight || y < lastTop || y > lastBottom) {
                        featureRegions[r].push_back(QPoint(x,y));
                    }
                }
            }
        }
    }
    qDebug() << "calculate inter-row feature regions end";
//    throw QString("self end");

    /* stitch row images */
    qDebug() << "stitch rows start";
    QPoint *rowOffsets = new QPoint[rows - 1];
    QImage image1, image2;
    QPoint accRowIntrinsicOffset(0,0); // the intrinsic offset is the offset between row(i)'s top-left and row(i-1)'s bottom-left when there is no error-offset
    for (int r = 0; r < rows - 1; r++) {
        image1 = r == 0 ? rowPixmaps[r]->toImage() : image2;
        image2 = rowPixmaps[r + 1]->toImage();
        QPoint offset = corrMatchWithFlag(&image1, &image2, featureRegions[r], searchCenterOffsets[r], searchRegion);
        qDebug() << QString("%1 ---  x = %2, y = %3").arg(r).arg(offset.x()).arg(offset.y());
//        accRowIntrinsicOffset += QPoint(0,image1.size().height()) - searchCenterOffsets[r];
//        accRowIntrinsicOffset += QPoint(0,image1.size().height()) + QPoint(searchCenterOffsets[r].x(), - overlap.height());
        accRowIntrinsicOffset += QPoint(searchCenterOffsets[r].x(), - realRowOverlaps[r].height());
        qDebug() << accRowIntrinsicOffset.x() << accRowIntrinsicOffset.y();
        rowOffsets[r] = offset + accRowIntrinsicOffset;
    }
    qDebug() << "stitch rows end";

    /* apply the results */
    qDebug() << "apply start";
//    for (QVector<SplicePicturesImageItem>::iterator iter = imageList.begin(); iter != imageList.end(); iter++) {
//        int row = iter->getRow(), col = iter->getCol();
    int accWidthOffset = 0, accHeightOffset = 0,
            widthOffset = 0, heightOffset = 0;
    for (int row = 0; row < rows; row++) {
        accWidthOffset = 0;
        QPoint rowOffset = (row == 0 ? QPoint(0,0) : rowOffsets[row - 1]);
//        QPoint rowOffset(0,0);
        for (int col = 0; col < cols; col++) {
            SplicePicturesImageItem *iter = items[row][col];
            QPoint offset = (col == 0 ? QPoint(0,0) : offsets[row][col - 1]);
            offset += QPoint(0, (images[row][col]->size().height() - items[row][col]->getHeight()) / 2);
            QSize realOverlap = col > 0 ? realOverlaps[row][col - 1] : QSize(0,0);
            QImage *image = images[row][col];
            widthOffset = image->size().width() - iter->getWidth();
            accWidthOffset += widthOffset / 2;
            if (col == 0) {
                heightOffset = image->size().height() - iter->getHeight();
                accHeightOffset += heightOffset / 2;
            }
            iter->setX(offset.x() + rowOffset.x() - realOverlap.width() * col);
            iter->setY(offset.y() + rowOffset.y());
//            qDebug() << QString("%1,%2 ---  x = %3, y = %4").arg(row).arg(col).arg(iter->getX()).arg(iter->getY());
//            qDebug() << QString("%1,%2 ---  %3").arg(row).arg(col).arg(accWidthOffset);
            accWidthOffset += (widthOffset + 1) / 2;
            if (col == 0) accHeightOffset += (heightOffset + 1) / 2;
        }
    }
//    }
    qDebug() << "apply end";

    /* memory recycle */
    qDebug() << "memcycle start";
    for (int r = 0; r < rows; r++) delete [](items[r]);
    delete []items;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 1; c++) delete images[r][c];
        delete [](images[r]);
    }
    delete []images;
    for (int r = 0; r < rows; r++) delete [](offsets[r]);
    delete []offsets;
    for (int r = 0; r < rows; r++) delete [](realOverlaps[r]);
    delete []realOverlaps;
    delete []featureRegions;
    delete []realRowOverlaps;
    for (int r = 0; r < rows; r++) delete rowPixmaps[r];
    delete []rowPixmaps;
    delete []minUps;
    delete []maxUps;
    delete []minDowns;
    delete []maxDowns;
    delete []rowOffsets;
    qDebug() << "memcycle end";
}

AutoStitchNoOverlapError::AutoStitchNoOverlapError(QPoint imagePos1, QPoint imagePos2, AutoStitch::StitchDirection direction)
    : imagePos1(imagePos1), imagePos2(imagePos2), direction(direction)
{
}



