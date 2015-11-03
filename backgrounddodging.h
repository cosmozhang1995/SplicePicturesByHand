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
    static QImage * dodgeBackground(QImage &image, QImage &background);
    static const double eps;
    static const double maxr;
    static const double minr;
};

#endif // BACKGROUNDDODGING_H
