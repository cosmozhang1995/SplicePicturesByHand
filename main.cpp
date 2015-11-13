#include "splicepicturesbyhand.h"
#include "rational.h"
#include "autostitch.h"

#include <math.h>

#include <QApplication>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QByteArray>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SplicePicturesByHand w;
    w.show();

//    QImage *image1 = new QImage("/Users/cosmozhang/work/test/stitch_samples/im1.bmp");
//    QImage *image2 = new QImage("/Users/cosmozhang/work/test/stitch_samples/im2.bmp");
//    QSize imageSize(fmin(image1->size().width(), image2->size().width()), fmin(image1->size().height(), image2->size().height()));
//    QSize overlap(101, 601);
//    QSize searchRegion(20, 20);
//    QSize featureHalfSize((overlap.width() - searchRegion.width() * 2 - 1)/2, (imageSize.height() - searchRegion.height() * 2 - 1)/2);
//    QPoint offset = AutoStitch::corrMatch(image1, image2, overlap, searchRegion, featureHalfSize, AutoStitch::DIRECTION_X);
//    qDebug() << offset.x() << offset.y();
//    delete image1;
//    delete image2;

//    SplicePicturesImageItem *item = new SplicePicturesImageItem();
//    item->setX(10);
//    SplicePicturesImageItem ***items = new (SplicePicturesImageItem**);
//    items[0] = new (SplicePicturesImageItem*);
//    items[0][0] = item;
//    items[0][1] = item;
//    items[0][2] = item;
//    qDebug() << items[0][0]->getX() << items[0][1]->getX();
//    delete items[0][0];
//    delete [](items[0]);
//    delete []items;
//    qDebug() << items[0][0]->getX();
//    qDebug() << items[0][1]->getX();

    return a.exec();
}
