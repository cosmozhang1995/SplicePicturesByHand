#ifndef BACKGROUNDDODGING_H
#define BACKGROUNDDODGING_H

#include <QVector>
#include <QImage>

class BackgroundDodging
{
public:
    BackgroundDodging();
    static QVector<QImage> generatePackground(QVector<QImage> *imageList);
    static QVector<QImage> dodgeBackground(QVector<QImage> *imageList, QVector<QImage> *backgroundList);
    static QImage & dodgeBackground(QImage &image, QImage &background);
    const static double eps;
    const static double maxr;
    const static double minr;
};

#endif // BACKGROUNDDODGING_H
