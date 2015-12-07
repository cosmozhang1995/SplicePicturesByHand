#ifndef SPLICEPICTURESBYHAND_H
#define SPLICEPICTURESBYHAND_H

#include <QWidget>

class SplicePicturesPanel;
class QSlider;
class QLineEdit;
class QLabel;
class QPushButton;

#define NUM_ROWS 4
#define NUM_COLS 4
#define PIC_WIDTH 160
#define PIC_HEIGHT 120
#define MARGIN_RATE 0.1

#define OVERLAP_X 101
#define OVERLAP_Y 101
#define SEARCH_REGION_X 20
#define SEARCH_REGION_Y 20
#define FEATURE_PADDING_X 81
#define FEATURE_PADDING_Y 1
//#define OVERLAP_X 201
//#define OVERLAP_Y 201
//#define SEARCH_REGION_X 20
//#define SEARCH_REGION_Y 20
//#define FEATURE_PADDING_X 81
//#define FEATURE_PADDING_Y 1

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
    void onButtonPreCalibrationScaleClicked();
    void onButtonPreCalibrationRotateClicked();
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

    void onAutoStitchButtonPressed();
    void onAutoStitchButtonReleased();
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
    QPushButton *autoSpliceButton;

    int _row;
    int _col;

    bool _inPreCalibrationMode;
};

#endif // SPLICEPICTURESBYHAND_H
