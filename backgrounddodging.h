#ifndef BACKGROUNDDODGING_H
#define BACKGROUNDDODGING_H

#include <QVector>
#include <QImage>

class BackgroundDodging
{
public:
    enum DodgeMode { DODGE_ADDITIVE, DODGE_MULTIPLICATIVE, DODGE_MIXED };
    static QVector<QImage> generatePackground(QVector<QImage> *imageList);
    static QVector<QImage> dodgeBackground(QVector<QImage> *imageList, QVector<QImage> *backgroundList, DodgeMode mode = DODGE_MIXED, double maxr = 1.0, double minr = 0.5);
    static QImage dodgeBackground(QImage &image, QImage &background, DodgeMode mode = DODGE_MIXED, double maxr = 1.0, double minr = 0.5);
    static const double eps;
    static const double maxr;
    static const double minr;
};

#endif // BACKGROUNDDODGING_H
