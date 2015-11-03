#ifndef SPLICEPICTURESBYHAND_H
#define SPLICEPICTURESBYHAND_H

#include <QWidget>

class SplicePicturesPanel;
class QSlider;
class QLineEdit;
class QLabel;

#define NUM_ROWS 4
#define NUM_COLS 4
#define PIC_WIDTH 100
#define PIC_HEIGHT 75
#define MARGIN_RATE 1.0

class SplicePicturesByHand : public QWidget
{
    Q_OBJECT

public:
    SplicePicturesByHand(QWidget *parent = 0);
    ~SplicePicturesByHand();
    void drawLayout();
protected:
    void keyPressEvent(QKeyEvent *);
public slots:
    void onPanelUnitClicked(int row, int col);
    void onPanelDragEvent(int x1, int y1, int x2, int y2);
    void onButtonPanelZoomInClicked();
    void onButtonPanelZoomOutClicked();
    void onButtonLoadBackgroundClicked();
    void onButtonGenerateBackgroundClicked();
    void onButtonDodgingBackgroundClicked();
    void onButtonPreCalibrationLoadClicked();
    void onButtonPreCalibrationDoClicked();
    void onButtonPreCalibrationImportClicked();
    void onButtonPreCalibrationExportClicked();
    void onButtonLoadImageClicked();
    void onButtonRemoveImageClicked();
    void onButtonClearImageClicked();
    void onButtonMoveUpClicked();
    void onButtonMoveDownClicked();
    void onButtonMoveLeftClicked();
    void onButtonMoveRightClicked();
    void onButtonZoomInClicked();
    void onButtonZoonOutClicked();
    void onButtonRotateLeftClicked();
    void onButtonRotateRightClicked();
    void onButtonAutoSpliceClicked();
    void onButtonGenerateClicked();
    void onButtonExportClicked();
    void onButtonImportClicked();
private:
    void loadStatus();
    bool loadImages(bool emptyFirst, int row = -1, int col = -1);
    bool checkInPreCalibrationMode(bool withAlert = false, bool alertIn = false);

    SplicePicturesPanel *panel;

    QLineEdit *moveStepInput;
    QLineEdit *rotateStepInput;
    QLineEdit *zoomStepInput;

    QLabel *informationLabel;
    QLabel *statusLabel;

    int _row;
    int _col;

    bool _inPreCalibrationMode;
};

#endif // SPLICEPICTURESBYHAND_H
