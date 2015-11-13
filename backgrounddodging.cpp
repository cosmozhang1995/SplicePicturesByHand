#include "backgrounddodging.h"

#include <assert.h>
#include <math.h>
#include <limits.h>

#include <QPixmap>
#include <QVector>
#include <QImage>
#include <qrgb.h>

#include <QDebug>

const double BackgroundDodging::eps = 2.2204 * pow(10.0, -16.0);

QVector<QImage> BackgroundDodging::generatePackground(QVector<QImage> *imageList) {
    for (QVector<QImage>::iterator iter = imageList->begin(); iter != imageList->end(); iter++) {
        QImage theImg(iter->size().width(), iter->size().height(), QImage::Format_RGB666);
    }
    QVector<QImage> list;
    return list;
}

QVector<QImage> BackgroundDodging::dodgeBackground(QVector<QImage> *imageList, QVector<QImage> *backgroundList, DodgeMode mode, double maxr, double minr) {
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

QImage BackgroundDodging::dodgeBackground(QImage &image, QImage &background, DodgeMode mode, double maxr, double minr) {
    double means[] = {0.0, 0.0, 0.0};
    const int imageWidth = image.size().width();
    const int imageHeight = image.size().height();
    QImage processedImage(imageWidth, imageHeight, QImage::Format_RGB888);
    assert((imageWidth == background.size().width()) && (imageHeight == background.size().height()));
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < imageWidth; col++) {
            QRgb rgbBg = background.pixel(col, row);
            means[0] += (double)qRed(rgbBg);
            means[1] += (double)qGreen(rgbBg);
            means[2] += (double)qBlue(rgbBg);
        }
    }
    means[0] = means[0] / imageWidth / imageHeight;
    means[1] = means[1] / imageWidth / imageHeight;
    means[2] = means[2] / imageWidth / imageHeight;
    for (int row = 0; row < imageHeight; row++) {
        for (int col = 0; col < imageWidth; col++) {
            QRgb rgbIm = image.pixel(col, row);
            QRgb rgbBg = background.pixel(col, row);
            int r1 = qRed(rgbIm) - qRed(rgbBg);
            int g1 = qGreen(rgbIm) - qGreen(rgbBg);
            int b1 = qBlue(rgbIm) - qBlue(rgbBg);
            int r = (double)r1 * fmin(fmax(means[0] / ((double)qRed(rgbBg) + eps), minr), maxr) + means[0];
            int g = (double)g1 * fmin(fmax(means[1] / ((double)qGreen(rgbBg) + eps), minr), maxr) + means[1];
            int b = (double)b1 * fmin(fmax(means[2] / ((double)qBlue(rgbBg) + eps), minr), maxr) + means[2];
            processedImage.setPixel(col, row, qRgb(r,g,b));
        }
    }
    return processedImage;
}
