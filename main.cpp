#include "splicepicturesbyhand.h"
#include "rational.h"
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

//    QImage image("");
//    QImage background("");
//    QImage processedImage = BackgroundDodging::dodgeBackground(image, background);
//    processedImage.save("");

    return a.exec();
}
