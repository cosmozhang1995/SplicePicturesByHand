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
SplicePicturesBackgroundItem::~SplicePicturesBackgroundItem() {
    if (image) delete image;
}

void SplicePicturesBackgroundItem::setRow(int row) { this->row = row; }
void SplicePicturesBackgroundItem::setCol(int col) { this->col = col; }
void SplicePicturesBackgroundItem::setImage(QImage *image) { if (image == this->image) return; if (this->image) delete this->image; this->image = image; }
int SplicePicturesBackgroundItem::getRow() { return row; }
int SplicePicturesBackgroundItem::getCol() { return col; }
QImage * SplicePicturesBackgroundItem::getImage() { return image; }
