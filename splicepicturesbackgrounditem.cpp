#include "splicepicturesbackgrounditem.h"

SplicePicturesBackgroundItem::SplicePicturesBackgroundItem()
{
    row = 0;
    col = 0;
}

SplicePicturesBackgroundItem::SplicePicturesBackgroundItem(int row, int col, QImage *image)
    : row(row), col(col), image(image)
{
}

void SplicePicturesBackgroundItem::setRow(int row) { this->row = row; }
void SplicePicturesBackgroundItem::setCol(int col) { this->col = col; }
void SplicePicturesBackgroundItem::setImage(QImage *image) { this->image = image; }
int SplicePicturesBackgroundItem::getRow() { return row; }
int SplicePicturesBackgroundItem::getCol() { return col; }
QImage * SplicePicturesBackgroundItem::getImage() { return image; }
