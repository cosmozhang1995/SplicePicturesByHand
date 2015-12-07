#include "splicepicturesbyhand.h"
#include "rational.h"
#include "autostitch.h"

#include <math.h>
#include <ctime>

#include <QApplication>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>

#include <QPoint>
#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QPainter>
#include <QRgb>
#include <QRegion>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SplicePicturesByHand w;
    w.show();

    return a.exec();
}
