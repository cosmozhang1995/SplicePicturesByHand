#ifndef SPLICEPICTURESBACKGROUNDITEM_H
#define SPLICEPICTURESBACKGROUNDITEM_H

class QImage;

class SplicePicturesBackgroundItem
{
public:
    SplicePicturesBackgroundItem();
    SplicePicturesBackgroundItem(int row, int col, QImage *image);

    void setRow(int row);
    void setCol(int col);
    void setImage(QImage *image);
    int getRow();
    int getCol();
    QImage *getImage();
private:
    int row;
    int col;
    QImage *image;
};

#endif // SPLICEPICTURESBACKGROUNDITEM_H
