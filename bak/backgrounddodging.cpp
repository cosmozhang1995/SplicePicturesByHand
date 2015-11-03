#include "backgrounddodging.h"

#include <assert.h>
#include <math.h>

#include <QPixmap>
#include <QVector>
#include <QImage>
#include <qrgb.h>

const double BackgroundDodging::eps = 2.2204 * pow(10.0, -16.0);
const double BackgroundDodging::maxr = 1.0;
const double BackgroundDodging::minr = 0.5;

BackgroundDodging::BackgroundDodging()
{
}

QVector<QImage> BackgroundDodging::generatePackground(QVector<QImage> *imageList) {
    for (QVector<QImage>::iterator iter = imageList->begin(); iter != imageList->end(); iter++) {
        QImage theImg(iter->size().width(), iter->size().height(), QImage::Format_RGB666);
    }
    QVector<QImage> list;
    return list;
}

QVector<QImage> BackgroundDodging::dodgeBackground(QVector<QImage> *imageList, QVector<QImage> *backgroundList) {
    assert(imageList->size() == backgroundList->size());
    int len = imageList->size();
    QVector<QImage> list;
    for (int i = 0; i < len; i++) {
        QImage image = imageList->at(i);
        QImage background = backgroundList->at(i);
        list.push_back(dodgeBackground(image, background));
    }
    return list;
}

QImage & BackgroundDodging::dodgeBackground(QImage &image, QImage &background) {
    double means[] = {0.0, 0.0, 0.0};
    const int imageWidth = image.size().width();
    const int imageHeight = image.size().height();
    QImage processedImage(imageWidth, imageHeight, QImage::Format_RGB666);
    double r1[imageWidth][imageHeight][3];
    double r2[imageWidth][imageHeight][3];
    double processed_image[imageWidth][imageHeight][3];
    assert((imageWidth == background.size().width()) && (imageHeight == background.size().height()));
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < imageWidth; col++) {
            QRgb rgbIm = image.pixel(col, row);
            QRgb rgbBg = background.pixel(col, row);
            means[0] += (double)qRed(rgbBg);
            means[1] += (double)qGreen(rgbBg);
            means[2] += (double)qBlue(rgbBg);
            r1[row][col][0] = (double)qRed(rgbIm) - (double)qRed(rgbBg);
            r1[row][col][1] = (double)qGreen(rgbIm) - (double)qGreen(rgbBg);
            r1[row][col][2] = (double)qBlue(rgbIm) - (double)qBlue(rgbBg);
        }
    }
    means[0] = means[0] / imageWidth / imageHeight;
    means[1] = means[1] / imageWidth / imageHeight;
    means[2] = means[2] / imageWidth / imageHeight;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < imageWidth; col++) {
            QRgb rgbBg = background.pixel(col, row);
            r2[row][col][0] = fmin(fmax(means[0] / ((double)qRed(rgbBg) + eps), minr), maxr);
            r2[row][col][1] = fmin(fmax(means[1] / ((double)qGreen(rgbBg) + eps), minr), maxr);
            r2[row][col][2] = fmin(fmax(means[2] / ((double)qBlue(rgbBg) + eps), minr), maxr);
            processed_image[row][col][0] = r1[row][col][0] * r2[row][col][0] + means[0];
            processed_image[row][col][1] = r1[row][col][1] * r2[row][col][1] + means[1];
            processed_image[row][col][2] = r1[row][col][2] * r2[row][col][2] + means[2];
            processedImage.setPixel(col, row, qRgb((int)processed_image[row][col][0],
                                                    (int)processed_image[row][col][1],
                                                    (int)processed_image[row][col][2]));
        }
    }
    return processedImage;
}
