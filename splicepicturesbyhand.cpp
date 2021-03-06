#include "splicepicturesbyhand.h"

#include "splicepicturespanel.h"
#include "splicepicturesimageitem.h"
#include "rational.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QPixmap>
#include <QStringList>
#include <QByteArray>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QMessageBox>
#include <QFrame>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QStyle>

SplicePicturesByHand::SplicePicturesByHand(QWidget *parent)
    : QWidget(parent), _row(0), _col(0)
{
    drawLayout();
    setWindowTitle("Muti-Images Mosaic GUI");
}

SplicePicturesByHand::~SplicePicturesByHand()
{
}

void SplicePicturesByHand::drawLayout() {
    panel = new SplicePicturesPanel(NUM_ROWS, NUM_COLS, PIC_WIDTH, PIC_HEIGHT, MARGIN_RATE, 1.0, this);
    panel->initialize();
    QPushButton *buttonPanelZoomIn = new QPushButton("Zoom In");
    QPushButton *buttonPanelZoomOut = new QPushButton("Zoom Out");
    QGridLayout *panelLayout = new QGridLayout();
    panelLayout->addWidget(panel, 0, 0, 1, 2);
    panelLayout->addWidget(buttonPanelZoomIn, 1, 0, 1, 1);
    panelLayout->addWidget(buttonPanelZoomOut, 1, 1, 1, 1);

    QLabel *label;
    QFont *font;

    QVBoxLayout *dodgingLayout = new QVBoxLayout();
    label = new QLabel("Step 1. De-vignetting:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    dodgingLayout->addWidget(label);
    QGridLayout *dodgingLayout1 = new QGridLayout();
    QPushButton *buttonLoadBackground = new QPushButton("Load Background Images");
//    QPushButton *buttonGenerateBackground = new QPushButton("Generate Background");
    QPushButton *buttonDodgingProcess = new QPushButton("Estimate Vignetted Functions");
    dodgingLayout1->addWidget(buttonLoadBackground, 0, 0, 1, 1);
//    dodgingLayout1->addWidget(buttonGenerateBackground);
    dodgingLayout1->addWidget(buttonDodgingProcess, 0, 1, 1, 1);
    dodgingLayout->addLayout(dodgingLayout1);

    QVBoxLayout *preCalibrationLayout = new QVBoxLayout();
    label = new QLabel("Step 2. Pre-calibration:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    preCalibrationLayout->addWidget(label);
    QGridLayout *preCalibrationLayout1 = new QGridLayout();
    QPushButton *buttonLoadCalibrationSamples = new QPushButton("Load Calibration Images");
    QPushButton *buttonCalibrateRotate = new QPushButton("Calculate Rotation Errors");
    QPushButton *buttonCalibrateScale = new QPushButton("Calculate Scale Errors");
    QPushButton *buttonImportCalibration = new QPushButton("Load Calibration Parameters");
    QPushButton *buttonExportCalibration = new QPushButton("Save Calibration Parameters");
    preCalibrationLayout1->addWidget(buttonLoadCalibrationSamples, 0, 0, 1, 2);
    preCalibrationLayout1->addWidget(buttonCalibrateRotate, 1, 0, 1, 1);
    preCalibrationLayout1->addWidget(buttonCalibrateScale, 1, 1, 1, 1);
    preCalibrationLayout1->addWidget(buttonImportCalibration, 2, 0, 1, 1);
    preCalibrationLayout1->addWidget(buttonExportCalibration, 2, 1, 1, 1);
    preCalibrationLayout->addLayout(preCalibrationLayout1);

    QVBoxLayout *dashboardLayout = new QVBoxLayout();

    QVBoxLayout *fileButtonLayout = new QVBoxLayout();
    label = new QLabel("Step 3. Load Images:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    fileButtonLayout->addWidget(label);
    QGridLayout *fileButtonLayout1 = new QGridLayout();
    QPushButton *buttonLoadImage = new QPushButton("Load Images");
    QPushButton *buttonCorrect = new QPushButton("Correct Errors");
    QPushButton *buttonRemoveImage = new QPushButton("Remove Selected Image");
    QPushButton *buttonClearImage = new QPushButton("Clear All Images");
    fileButtonLayout1->addWidget(buttonLoadImage, 0, 0, 1, 1);
    fileButtonLayout1->addWidget(buttonCorrect, 0, 1, 1, 1);
    fileButtonLayout1->addWidget(buttonRemoveImage, 1, 0, 1, 1);
    fileButtonLayout1->addWidget(buttonClearImage, 1, 1, 1, 1);
    fileButtonLayout->addLayout(fileButtonLayout1);

    QGridLayout *navLayout = new QGridLayout();
    QPushButton *buttonMoveUp = new QPushButton("↑");
    QPushButton *buttonMoveDown = new QPushButton("↓");
    QPushButton *buttonMoveLeft = new QPushButton("←");
    QPushButton *buttonMoveRight = new QPushButton("→");
    navLayout->addWidget(buttonMoveUp, 0, 1, 1, 1);
    navLayout->addWidget(buttonMoveLeft, 1, 0, 1, 1);
    navLayout->addWidget(buttonMoveDown, 2, 1, 1, 1);
    navLayout->addWidget(buttonMoveRight, 1, 2, 1, 1);

    QPushButton *buttonZoomIn = new QPushButton("+");
    QPushButton *buttonZoomOut = new QPushButton("-");
    navLayout->addWidget(buttonZoomIn, 0, 0, 1, 1);
    navLayout->addWidget(buttonZoomOut, 0, 2, 1, 1);

    QPushButton *buttonRotateLeft = new QPushButton("↺");
    QPushButton *buttonRotateRight = new QPushButton("↻");
    navLayout->addWidget(buttonRotateLeft, 2, 0, 1, 1);
    navLayout->addWidget(buttonRotateRight, 2, 2, 1, 1);

    QGridLayout *paraInputLayout = new QGridLayout();
    label = new QLabel("Offset Step (pixels)");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    moveStepInput = new QLineEdit("1");
    moveStepInput->setMinimumWidth(150);
    paraInputLayout->addWidget(label, 0, 0, 1, 1);
    paraInputLayout->addWidget(moveStepInput, 0, 1, 1, 3);
    label = new QLabel("Scale Step (Fraction)");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    zoomStepInput = new QLineEdit("1/4208");
    paraInputLayout->addWidget(label, 1, 0, 1, 1);
    paraInputLayout->addWidget(zoomStepInput, 1, 1, 1, 3);
    label = new QLabel("Rotate Step (Degree)");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    rotateStepInput = new QLineEdit("60/2104");
    paraInputLayout->addWidget(label, 2, 0, 1, 1);
    paraInputLayout->addWidget(rotateStepInput, 2, 1, 1, 3);

    QVBoxLayout *navAndParaLayout = new QVBoxLayout();
    label = new QLabel("Manual Adjusting:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    navAndParaLayout->addWidget(label);
    QHBoxLayout *navAndParaLayout1 = new QHBoxLayout();
    navAndParaLayout1->addLayout(navLayout, 0);
    navAndParaLayout1->addLayout(paraInputLayout, 1);
    navAndParaLayout->addLayout(navAndParaLayout1);

    QVBoxLayout *autoSpliceLayout = new QVBoxLayout();
    label = new QLabel("     ");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    autoSpliceLayout->addWidget(label);
    font = new QFont();
    font->setPixelSize(16);
    autoSpliceButton = new QPushButton("Auto-Mosaic", this);
//    autoSpliceButton->setFont(*font);
    autoSpliceButton->setStyleSheet(QString("QPushButton {font:16px;padding:16px; border: 1px solid gray; border-radius: 6px; background-color: #dddddd;} ") +
                                    QString("QPushButton:hover {background-color: white;}"));
    connect(autoSpliceButton, SIGNAL(pressed()), this, SLOT(onAutoStitchButtonPressed()));
    connect(autoSpliceButton, SIGNAL(released()), this, SLOT(onAutoStitchButtonReleased()));
    QGraphicsDropShadowEffect *autoSpliceButtonShadowEffect = new QGraphicsDropShadowEffect();
    autoSpliceButtonShadowEffect->setColor(Qt::black);
    autoSpliceButtonShadowEffect->setBlurRadius(5);
    autoSpliceButtonShadowEffect->setOffset(0);
    autoSpliceButtonShadowEffect->setEnabled(true);
    autoSpliceButton->setGraphicsEffect(autoSpliceButtonShadowEffect);
    autoSpliceButton->setCursor(Qt::OpenHandCursor);
    autoSpliceLayout->addWidget(autoSpliceButton);

    statusLabel = new QLabel("");
    statusLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    informationLabel = new QLabel("");
    informationLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QVBoxLayout *outputButtonLayout = new QVBoxLayout();
    label = new QLabel("Save Results:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    outputButtonLayout->addWidget(label);
    QHBoxLayout *outputButtonLayout1 = new QHBoxLayout();
    QPushButton *buttonConfigExport = new QPushButton("Export Configuration");
    QPushButton *buttonConfigImport = new QPushButton("Import Configuration");
    QPushButton *buttonGenerateImage = new QPushButton("Save Result Images");
    outputButtonLayout1->addWidget(buttonConfigExport);
    outputButtonLayout1->addWidget(buttonConfigImport);
    outputButtonLayout1->addWidget(buttonGenerateImage);
    outputButtonLayout->addLayout(outputButtonLayout1);

    dashboardLayout->addLayout(dodgingLayout);
    dashboardLayout->addLayout(preCalibrationLayout);
    dashboardLayout->addLayout(fileButtonLayout);
    dashboardLayout->addLayout(navAndParaLayout);
    dashboardLayout->addLayout(autoSpliceLayout);
    dashboardLayout->addWidget(statusLabel);
    dashboardLayout->addWidget(informationLabel);
    dashboardLayout->addLayout(outputButtonLayout);

    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->addLayout(panelLayout);
    bodyLayout->addLayout(dashboardLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    label = new QLabel("© 2014 Heng Mao Lab. Peking University");
    font = new QFont();
    font->setPointSize(9);
    label->setFont(*font);
    layout->addLayout(bodyLayout);
    layout->addWidget(label);
    setLayout(layout);

//    statusLabel->setMaximumWidth(dashboardLayout->sizeHint().width());
//    statusLabel->setWordWrap(true);
    statusLabel->setMinimumWidth(700);
    statusLabel->setMaximumWidth(700);

    connect(panel, SIGNAL(onUnitClicked(int,int)), this, SLOT(onPanelUnitClicked(int,int)));
    connect(panel, SIGNAL(onDrag(int,int,int,int)), this, SLOT(onPanelDragEvent(int,int,int,int)));
    connect(buttonLoadBackground, SIGNAL(clicked(bool)), this, SLOT(onButtonLoadBackgroundClicked()));
//    connect(buttonGenerateBackground, SIGNAL(clicked(bool)), this, SLOT(onButtonGenerateBackgroundClicked()));
    connect(buttonDodgingProcess, SIGNAL(clicked(bool)), this, SLOT(onButtonDodgingBackgroundClicked()));
    connect(buttonLoadCalibrationSamples, SIGNAL(clicked(bool)), this, SLOT(onButtonPreCalibrationLoadClicked()));
    connect(buttonCalibrateRotate, SIGNAL(clicked(bool)), this, SLOT(onButtonPreCalibrationRotateClicked()));
    connect(buttonCalibrateScale, SIGNAL(clicked(bool)), this, SLOT(onButtonPreCalibrationScaleClicked()));
    connect(buttonExportCalibration, SIGNAL(clicked(bool)), this, SLOT(onButtonPreCalibrationExportClicked()));
    connect(buttonImportCalibration, SIGNAL(clicked(bool)), this, SLOT(onButtonPreCalibrationImportClicked()));
    connect(buttonPanelZoomIn, SIGNAL(clicked(bool)), this, SLOT(onButtonPanelZoomInClicked()));
    connect(buttonPanelZoomOut, SIGNAL(clicked(bool)), this, SLOT(onButtonPanelZoomOutClicked()));
    connect(buttonLoadImage, SIGNAL(clicked(bool)), this, SLOT(onButtonLoadImageClicked()));
    connect(buttonRemoveImage, SIGNAL(clicked(bool)), this, SLOT(onButtonRemoveImageClicked()));
    connect(buttonClearImage, SIGNAL(clicked(bool)), this, SLOT(onButtonClearImageClicked()));
    connect(buttonMoveUp, SIGNAL(clicked(bool)), this, SLOT(onButtonMoveUpClicked()));
    connect(buttonMoveDown, SIGNAL(clicked(bool)), this, SLOT(onButtonMoveDownClicked()));
    connect(buttonMoveLeft, SIGNAL(clicked(bool)), this, SLOT(onButtonMoveLeftClicked()));
    connect(buttonMoveRight, SIGNAL(clicked(bool)), this, SLOT(onButtonMoveRightClicked()));
    connect(buttonZoomIn, SIGNAL(clicked(bool)), this, SLOT(onButtonZoomInClicked()));
    connect(buttonZoomOut, SIGNAL(clicked(bool)), this, SLOT(onButtonZoonOutClicked()));
    connect(buttonRotateLeft, SIGNAL(clicked(bool)), this, SLOT(onButtonRotateLeftClicked()));
    connect(buttonRotateRight, SIGNAL(clicked(bool)), this, SLOT(onButtonRotateRightClicked()));
    connect(autoSpliceButton, SIGNAL(clicked(bool)), this, SLOT(onButtonAutoSpliceClicked()));
    connect(buttonConfigExport, SIGNAL(clicked(bool)), this, SLOT(onButtonExportClicked()));
    connect(buttonConfigImport, SIGNAL(clicked(bool)), this, SLOT(onButtonImportClicked()));
    connect(buttonGenerateImage, SIGNAL(clicked(bool)), this, SLOT(onButtonGenerateClicked()));
}

void SplicePicturesByHand::onPanelUnitClicked(int row, int col) {
    if (!panel->selectImage(row, col)) {
        if (loadImages(_inPreCalibrationMode, row, col)) _inPreCalibrationMode = false;
    } else {
        loadStatus();
    }
}

void SplicePicturesByHand::onPanelDragEvent(int x1, int y1, int x2, int y2) {
    int x = panel->getZoomCenter().x() + x1 - x2;
    int y = panel->getZoomCenter().y() + y1 - y2;
    panel->setZoomCenter(x, y);
}

void SplicePicturesByHand::onButtonPanelZoomInClicked() {
    panel->doViewZoom(true);
}
void SplicePicturesByHand::onButtonPanelZoomOutClicked() {
    panel->doViewZoom(false);
}

void SplicePicturesByHand::onButtonLoadBackgroundClicked() {}
void SplicePicturesByHand::onButtonGenerateBackgroundClicked() {}
void SplicePicturesByHand::onButtonDodgingBackgroundClicked() {}

void SplicePicturesByHand::onButtonPreCalibrationLoadClicked() {
    if (loadImages(!_inPreCalibrationMode)) _inPreCalibrationMode = true;
}
void SplicePicturesByHand::onButtonPreCalibrationRotateClicked() {
    if (checkInPreCalibrationMode(true, false)) return;
}
void SplicePicturesByHand::onButtonPreCalibrationScaleClicked() {
    if (checkInPreCalibrationMode(true, false)) return;
}
void SplicePicturesByHand::onButtonPreCalibrationImportClicked() {
    static QString defaultPath = "/Users/cosmozhang/Desktop/calibration.json";
    QString filePath = QFileDialog::getOpenFileName(0, "Export the Configurations", defaultPath, "*.json");
    if (filePath != "") {
        defaultPath = QFileInfo(filePath).absoluteDir().absoluteFilePath("config.json");
        QString configurationStr;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Warning", "Failed to open the configuration file");
        }
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            configurationStr += stream.readLine();
        }
        file.close();
        panel->loadCalibrationFromString(configurationStr);
    }
}
void SplicePicturesByHand::onButtonPreCalibrationExportClicked() {
    static QString defaultPath = "/Users/cosmozhang/Desktop/calibration.json";
    QString filePath = QFileDialog::getSaveFileName(0, "Export the Configurations", defaultPath, "*.json");
    if (filePath != "") {
        defaultPath = QFileInfo(filePath).absoluteDir().absoluteFilePath("calibration.json");
        QString configurationStr = panel->getCalibrationAsString();
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Warning", "Failed to save the calibration configuration");
        }
        QTextStream stream(&file);
        stream << configurationStr;
        file.close();
    }
}

void SplicePicturesByHand::onButtonLoadImageClicked() {
    if (loadImages(_inPreCalibrationMode)) _inPreCalibrationMode = false;
}
void SplicePicturesByHand::onButtonRemoveImageClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
    panel->removeLastImage();
    loadStatus();
}

void SplicePicturesByHand::onButtonClearImageClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
    panel->removeAllImages();
    loadStatus();
}

void SplicePicturesByHand::onButtonMoveUpClicked() {
    int step = moveStepInput->text().toInt();
    if (step != 0) {
        panel->move(0, -step);
        loadStatus();
    }
}
void SplicePicturesByHand::onButtonMoveDownClicked() {
    int step = moveStepInput->text().toInt();
    if (step != 0) {
        panel->move(0, step);
        loadStatus();
    }
}
void SplicePicturesByHand::onButtonMoveLeftClicked() {
    int step = moveStepInput->text().toInt();
    if (step != 0) {
        panel->move(-step, 0);
        loadStatus();
    }
}
void SplicePicturesByHand::onButtonMoveRightClicked() {
    int step = moveStepInput->text().toInt();
    if (step != 0) {
        panel->move(step, 0);
        loadStatus();
    }
}
void SplicePicturesByHand::onButtonZoomInClicked() {
    QString stepStr = zoomStepInput->text();
    try {
        Rational step(stepStr);
        panel->zoom(step);
        loadStatus();
    } catch (QString err) {}
}
void SplicePicturesByHand::onButtonZoonOutClicked() {
    QString stepStr = zoomStepInput->text();
    try {
        Rational step(stepStr);
        panel->zoom(-step);
        loadStatus();
    } catch (QString err) {}
}
void SplicePicturesByHand::onButtonRotateLeftClicked() {
    QString stepStr = rotateStepInput->text();
    try {
        Rational step(stepStr);
        panel->rotate(-step);
        loadStatus();
    } catch (QString err) {}
}
void SplicePicturesByHand::onButtonRotateRightClicked() {
    QString stepStr = rotateStepInput->text();
    try {
        Rational step(stepStr);
        panel->rotate(step);
        loadStatus();
    } catch (QString err) {}
}

void SplicePicturesByHand::onButtonAutoSpliceClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
    panel->autoStitch(QSize(OVERLAP_X, OVERLAP_Y), QSize(SEARCH_REGION_X, SEARCH_REGION_Y), QSize(FEATURE_PADDING_X, FEATURE_PADDING_Y));
}

void SplicePicturesByHand::onButtonGenerateClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
//    static QString defaultPath = "/Users/cosmozhang/Desktop/Untitled.bmp";
//    QString filePath = QFileDialog::getSaveFileName(0, "Save the Completed Image", defaultPath, "Images (*.bmp)");
//    if (filePath != "") {
//        defaultPath = QFileInfo(filePath).absoluteDir().absoluteFilePath("Untitled.bmp");
//        QPixmap *pix = panel->getFullPixmap();
//        pix->save(filePath);
//    }
    static QString defaultPath = "/Users/cosmozhang/Desktop/Untitled.bmp";
    QString filePath = QFileDialog::getSaveFileName(0, "Save the Completed Image", defaultPath, "Images (*.bmp)");
    if (filePath != "") {
        QDir dir = QFileInfo(filePath).absoluteDir();
        defaultPath = dir.absoluteFilePath("");
        panel->getFullPixmap()->save(dir.absoluteFilePath("full.tiff"));
        for (int r = 0; r < NUM_ROWS; r++) {
            for (int c = 0; c < NUM_COLS; c++) {
                bool success;
                QPixmap pix = panel->getTransformedPixmap(r, c, success);
                if (success) pix.save(dir.absoluteFilePath(QString("%1.tiff").arg(r * 4 + c + 1)));
            }
        }
    }
}
void SplicePicturesByHand::onButtonExportClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
    static QString defaultPath = "/Users/cosmozhang/Desktop/config.json";
    QString filePath = QFileDialog::getSaveFileName(0, "Export the Configurations", defaultPath, "*.json");
    if (filePath != "") {
        defaultPath = QFileInfo(filePath).absoluteDir().absoluteFilePath("config.json");
        QString configurationStr = panel->getConfigurationAsString();
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Warning", "Failed to save the configuration");
        }
        QTextStream stream(&file);
        stream << configurationStr;
        file.close();
    }
}
void SplicePicturesByHand::onButtonImportClicked() {
    if (checkInPreCalibrationMode(true, true)) return;
    static QString defaultPath = "/Users/cosmozhang/Desktop/config.json";
    QString filePath = QFileDialog::getOpenFileName(0, "Export the Configurations", defaultPath, "*.json");
    if (filePath != "") {
        defaultPath = QFileInfo(filePath).absoluteDir().absoluteFilePath("config.json");
        QString configurationStr;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Warning", "Failed to open the configuration file");
        }
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            configurationStr += stream.readLine();
        }
        file.close();
        panel->loadConfigurationFromString(configurationStr);
    }
}

bool SplicePicturesByHand::loadImages(bool emptyFirst, int row, int col) {
    static QString imagePath = "/Users/cosmozhang/work/lab/Pics/group1";
    QStringList filePathList = QFileDialog::getOpenFileNames(this, "Open the Image Files", imagePath, "Image Files(*.jpg *.png *.bmp *.tiff)");
    if (!filePathList.empty()) {
        imagePath = filePathList.at(0);
        if (emptyFirst) {
            panel->removeAllImages(false);
        }
    }
    if (row >= 0 && col >= 0) {
        for (QStringList::iterator iter = filePathList.begin(); iter < filePathList.end(); iter++) {
            QString filePath = *iter;
            if (filePath != "") {
                panel->removeImage(row, col, false);
                panel->loadImage(row, col, filePath, true, false);
                if (col >= NUM_COLS - 1) {
                    col = 0;
                    row ++;
                } else {
                    col++;
                }
                if (row > NUM_ROWS - 1) break;
            } else {
                break;
            }
        }
    } else {
        SplicePicturesImageItem *item = panel->currentImage();
        int _row, _col;
        if (item) {
            _row = item->getRow();
            _col = item->getCol();
            if (_col >= NUM_COLS - 1) {
                _row++;
                _col = 0;
            } else {
                _col ++;
            }
        } else {
            _row = 0;
            _col = 0;
        }
        if (_row > NUM_ROWS - 1) return false;
        for (QStringList::iterator iter = filePathList.begin(); iter < filePathList.end(); iter++) {
            QString filePath = *iter;
            if (filePath != "") {
                panel->loadImage(_row, _col, filePath, true, false);
                if (_col >= NUM_COLS - 1) {
                    _col = 0;
                    _row ++;
                } else {
                    _col++;
                }
                if (_row > NUM_ROWS - 1) break;
            } else {
                break;
            }
        }
    }
    panel->refresh();
    loadStatus();
    return !filePathList.empty();
}

void SplicePicturesByHand::loadStatus() {
    SplicePicturesImageItem *item = panel->currentImage();
    if (item) {
        QString statusStr = QString("Image (%1 : %2) --- Offset: %3 , %4     Scale: %5     Rotating degrees: %6")
                .arg(item->getCol() + 1)
                .arg(item->getRow() + 1)
                .arg(item->getX())
                .arg(item->getY())
                .arg(item->getZoom().toString())
                .arg(item->getRotation().toString());
        statusLabel->setText(statusStr);
    } else {
        statusLabel->setText("");
    }
}

bool SplicePicturesByHand::checkInPreCalibrationMode(bool withAlert, bool alertIn) {
    if (withAlert) {
        if (alertIn && _inPreCalibrationMode) {
            QMessageBox::critical(this, "Operation Illegal", "In pre-calibration mode, cannot do operation.", QMessageBox::Accepted, QMessageBox::Cancel);
        } else if (!alertIn && !_inPreCalibrationMode) {
            QMessageBox::critical(this, "Operation Illegal", "Not in pre-calibration mode, cannot do operation.", QMessageBox::Accepted, QMessageBox::Cancel);
        }
    }
    return _inPreCalibrationMode;
}

void SplicePicturesByHand::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_Up) {
        onButtonMoveUpClicked();
    } else if (key == Qt::Key_Down) {
        onButtonMoveDownClicked();
    } else if (key == Qt::Key_Left) {
        onButtonMoveLeftClicked();
    } else if (key == Qt::Key_Right) {
        onButtonMoveRightClicked();
    } else if (key == Qt::Key_W) {
        onButtonPanelZoomInClicked();
    } else if (key == Qt::Key_S) {
        onButtonZoonOutClicked();
    } else if (key == Qt::Key_A) {
        onButtonRotateLeftClicked();
    } else if (key == Qt::Key_D) {
        onButtonMoveRightClicked();
    }
    return QWidget::keyPressEvent(event);
}


void SplicePicturesByHand::onAutoStitchButtonPressed() {
    autoSpliceButton->graphicsEffect()->setEnabled(false);
}
void SplicePicturesByHand::onAutoStitchButtonReleased() {
    autoSpliceButton->graphicsEffect()->setEnabled(true);
}
