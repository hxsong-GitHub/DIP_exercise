/********************************************************************************
** Form generated from reading UI file 'dip.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIP_H
#define UI_DIP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dipClass
{
public:
    QAction *fileOpen;
    QAction *saveAsPre;
    QAction *saveAs;
    QAction *mirror;
    QAction *offset;
    QAction *crop;
    QAction *scale;
    QAction *rot;
    QAction *fft;
    QAction *ifft;
    QAction *freFilter;
    QAction *filter;
    QAction *histogram;
    QAction *dct;
    QAction *action5;
    QAction *action6;
    QAction *aboutDIP;
    QAction *author;
    QWidget *centralwidget;
    QGridLayout *gridLayout_6;
    QWidget *widget;
    QGridLayout *gridLayout_5;
    QGroupBox *colorGrid;
    QLabel *labelColor_1;
    QLabel *labelColor_2;
    QGroupBox *actionGroup;
    QGridLayout *gridLayout_3;
    QGroupBox *radioGroup;
    QFormLayout *formLayout_3;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_1;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_4;
    QGroupBox *comboGroup;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_6;
    QRadioButton *radioButton_7;
    QRadioButton *radioButton_8;
    QGroupBox *inputGroup;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_1;
    QLineEdit *lineEdit_1;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *lineEdit_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_4;
    QLineEdit *lineEdit_4;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_5;
    QLineEdit *lineEdit_5;
    QGroupBox *selectGroup;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_3;
    QPushButton *resetButton;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer;
    QGroupBox *imgType;
    QGridLayout *gridLayout_9;
    QRadioButton *radioImgType_1;
    QRadioButton *radioImgType_2;
    QGridLayout *gridLayout_4;
    QGroupBox *sourceImg;
    QGridLayout *gridLayout_7;
    QScrollArea *sourceScrollArea;
    QWidget *scrollAreaWidgetContents_4;
    QGroupBox *outputImg;
    QGridLayout *gridLayout_8;
    QScrollArea *outputSrcollArea;
    QWidget *scrollAreaWidgetContents_3;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *pixelMenu;
    QMenu *freMenu;
    QMenu *aboutMenu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *dipClass)
    {
        if (dipClass->objectName().isEmpty())
            dipClass->setObjectName(QStringLiteral("dipClass"));
        dipClass->resize(759, 562);
        fileOpen = new QAction(dipClass);
        fileOpen->setObjectName(QStringLiteral("fileOpen"));
        fileOpen->setCheckable(false);
        saveAsPre = new QAction(dipClass);
        saveAsPre->setObjectName(QStringLiteral("saveAsPre"));
        saveAs = new QAction(dipClass);
        saveAs->setObjectName(QStringLiteral("saveAs"));
        mirror = new QAction(dipClass);
        mirror->setObjectName(QStringLiteral("mirror"));
        offset = new QAction(dipClass);
        offset->setObjectName(QStringLiteral("offset"));
        crop = new QAction(dipClass);
        crop->setObjectName(QStringLiteral("crop"));
        scale = new QAction(dipClass);
        scale->setObjectName(QStringLiteral("scale"));
        rot = new QAction(dipClass);
        rot->setObjectName(QStringLiteral("rot"));
        fft = new QAction(dipClass);
        fft->setObjectName(QStringLiteral("fft"));
        ifft = new QAction(dipClass);
        ifft->setObjectName(QStringLiteral("ifft"));
        freFilter = new QAction(dipClass);
        freFilter->setObjectName(QStringLiteral("freFilter"));
        filter = new QAction(dipClass);
        filter->setObjectName(QStringLiteral("filter"));
        histogram = new QAction(dipClass);
        histogram->setObjectName(QStringLiteral("histogram"));
        dct = new QAction(dipClass);
        dct->setObjectName(QStringLiteral("dct"));
        action5 = new QAction(dipClass);
        action5->setObjectName(QStringLiteral("action5"));
        action6 = new QAction(dipClass);
        action6->setObjectName(QStringLiteral("action6"));
        aboutDIP = new QAction(dipClass);
        aboutDIP->setObjectName(QStringLiteral("aboutDIP"));
        author = new QAction(dipClass);
        author->setObjectName(QStringLiteral("author"));
        centralwidget = new QWidget(dipClass);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_6 = new QGridLayout(centralwidget);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMaximumSize(QSize(16777215, 170));
        gridLayout_5 = new QGridLayout(widget);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout_5->setContentsMargins(10, 10, 10, 10);
        colorGrid = new QGroupBox(widget);
        colorGrid->setObjectName(QStringLiteral("colorGrid"));
        colorGrid->setMinimumSize(QSize(170, 150));
        colorGrid->setMaximumSize(QSize(170, 150));
        labelColor_1 = new QLabel(colorGrid);
        labelColor_1->setObjectName(QStringLiteral("labelColor_1"));
        labelColor_1->setGeometry(QRect(130, 20, 35, 110));
        labelColor_1->setMinimumSize(QSize(35, 110));
        labelColor_1->setMaximumSize(QSize(35, 110));
        labelColor_2 = new QLabel(colorGrid);
        labelColor_2->setObjectName(QStringLiteral("labelColor_2"));
        labelColor_2->setGeometry(QRect(10, 20, 110, 110));
        labelColor_2->setMinimumSize(QSize(110, 110));
        labelColor_2->setFrameShape(QFrame::Box);
        labelColor_2->setFrameShadow(QFrame::Plain);
        labelColor_2->setMargin(0);
        labelColor_2->setIndent(0);

        gridLayout_5->addWidget(colorGrid, 0, 1, 1, 1);

        actionGroup = new QGroupBox(widget);
        actionGroup->setObjectName(QStringLiteral("actionGroup"));
        actionGroup->setMaximumSize(QSize(16777215, 150));
        gridLayout_3 = new QGridLayout(actionGroup);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        radioGroup = new QGroupBox(actionGroup);
        radioGroup->setObjectName(QStringLiteral("radioGroup"));
        radioGroup->setMinimumSize(QSize(117, 121));
        radioGroup->setMaximumSize(QSize(117, 121));
        formLayout_3 = new QFormLayout(radioGroup);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        radioButton_1 = new QRadioButton(radioGroup);
        radioButton_1->setObjectName(QStringLiteral("radioButton_1"));

        verticalLayout->addWidget(radioButton_1);

        radioButton_2 = new QRadioButton(radioGroup);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        radioButton_3 = new QRadioButton(radioGroup);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        verticalLayout->addWidget(radioButton_3);

        radioButton_4 = new QRadioButton(radioGroup);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));

        verticalLayout->addWidget(radioButton_4);


        formLayout_3->setLayout(0, QFormLayout::LabelRole, verticalLayout);


        gridLayout_3->addWidget(radioGroup, 0, 1, 1, 1);

        comboGroup = new QGroupBox(actionGroup);
        comboGroup->setObjectName(QStringLiteral("comboGroup"));
        comboGroup->setMinimumSize(QSize(91, 121));
        comboGroup->setMaximumSize(QSize(91, 121));
        verticalLayout_4 = new QVBoxLayout(comboGroup);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        radioButton_5 = new QRadioButton(comboGroup);
        radioButton_5->setObjectName(QStringLiteral("radioButton_5"));

        verticalLayout_4->addWidget(radioButton_5);

        radioButton_6 = new QRadioButton(comboGroup);
        radioButton_6->setObjectName(QStringLiteral("radioButton_6"));

        verticalLayout_4->addWidget(radioButton_6);

        radioButton_7 = new QRadioButton(comboGroup);
        radioButton_7->setObjectName(QStringLiteral("radioButton_7"));

        verticalLayout_4->addWidget(radioButton_7);

        radioButton_8 = new QRadioButton(comboGroup);
        radioButton_8->setObjectName(QStringLiteral("radioButton_8"));

        verticalLayout_4->addWidget(radioButton_8);


        gridLayout_3->addWidget(comboGroup, 0, 2, 1, 1);

        inputGroup = new QGroupBox(actionGroup);
        inputGroup->setObjectName(QStringLiteral("inputGroup"));
        inputGroup->setMinimumSize(QSize(111, 121));
        inputGroup->setMaximumSize(QSize(180, 121));
        gridLayout = new QGridLayout(inputGroup);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_1 = new QLabel(inputGroup);
        label_1->setObjectName(QStringLiteral("label_1"));
        label_1->setMinimumSize(QSize(60, 0));
        label_1->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_2->addWidget(label_1);

        lineEdit_1 = new QLineEdit(inputGroup);
        lineEdit_1->setObjectName(QStringLiteral("lineEdit_1"));
        lineEdit_1->setMinimumSize(QSize(31, 0));
        lineEdit_1->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_2->addWidget(lineEdit_1);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(inputGroup);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(60, 0));
        label_2->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_3->addWidget(label_2);

        lineEdit_2 = new QLineEdit(inputGroup);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setMinimumSize(QSize(31, 0));
        lineEdit_2->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_3->addWidget(lineEdit_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(inputGroup);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(60, 0));
        label_3->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_4->addWidget(label_3);

        lineEdit_3 = new QLineEdit(inputGroup);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setMinimumSize(QSize(31, 0));
        lineEdit_3->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_4->addWidget(lineEdit_3);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_4 = new QLabel(inputGroup);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMinimumSize(QSize(60, 0));
        label_4->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_5->addWidget(label_4);

        lineEdit_4 = new QLineEdit(inputGroup);
        lineEdit_4->setObjectName(QStringLiteral("lineEdit_4"));
        lineEdit_4->setMinimumSize(QSize(31, 0));
        lineEdit_4->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_5->addWidget(lineEdit_4);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_5 = new QLabel(inputGroup);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(60, 0));
        label_5->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_6->addWidget(label_5);

        lineEdit_5 = new QLineEdit(inputGroup);
        lineEdit_5->setObjectName(QStringLiteral("lineEdit_5"));
        lineEdit_5->setMinimumSize(QSize(31, 0));
        lineEdit_5->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_6->addWidget(lineEdit_5);


        verticalLayout_2->addLayout(horizontalLayout_6);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        gridLayout_3->addWidget(inputGroup, 0, 3, 1, 1);

        selectGroup = new QGroupBox(actionGroup);
        selectGroup->setObjectName(QStringLiteral("selectGroup"));
        selectGroup->setMinimumSize(QSize(97, 121));
        selectGroup->setMaximumSize(QSize(97, 121));
        gridLayout_2 = new QGridLayout(selectGroup);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        resetButton = new QPushButton(selectGroup);
        resetButton->setObjectName(QStringLiteral("resetButton"));

        verticalLayout_3->addWidget(resetButton);

        okButton = new QPushButton(selectGroup);
        okButton->setObjectName(QStringLiteral("okButton"));

        verticalLayout_3->addWidget(okButton);


        gridLayout_2->addLayout(verticalLayout_3, 0, 0, 1, 1);


        gridLayout_3->addWidget(selectGroup, 0, 4, 1, 1);

        horizontalSpacer = new QSpacerItem(57, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 0, 5, 1, 1);


        gridLayout_5->addWidget(actionGroup, 0, 3, 1, 1);

        imgType = new QGroupBox(widget);
        imgType->setObjectName(QStringLiteral("imgType"));
        imgType->setMaximumSize(QSize(79, 16777215));
        gridLayout_9 = new QGridLayout(imgType);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        radioImgType_1 = new QRadioButton(imgType);
        radioImgType_1->setObjectName(QStringLiteral("radioImgType_1"));

        gridLayout_9->addWidget(radioImgType_1, 0, 0, 1, 1);

        radioImgType_2 = new QRadioButton(imgType);
        radioImgType_2->setObjectName(QStringLiteral("radioImgType_2"));
        radioImgType_2->setChecked(true);

        gridLayout_9->addWidget(radioImgType_2, 1, 0, 1, 1);


        gridLayout_5->addWidget(imgType, 0, 2, 1, 1);


        gridLayout_6->addWidget(widget, 0, 0, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        sourceImg = new QGroupBox(centralwidget);
        sourceImg->setObjectName(QStringLiteral("sourceImg"));
        sourceImg->setMinimumSize(QSize(321, 321));
        gridLayout_7 = new QGridLayout(sourceImg);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        sourceScrollArea = new QScrollArea(sourceImg);
        sourceScrollArea->setObjectName(QStringLiteral("sourceScrollArea"));
        sourceScrollArea->viewport()->setProperty("cursor", QVariant(QCursor(Qt::ArrowCursor)));
        sourceScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_4 = new QWidget();
        scrollAreaWidgetContents_4->setObjectName(QStringLiteral("scrollAreaWidgetContents_4"));
        scrollAreaWidgetContents_4->setGeometry(QRect(0, 0, 322, 287));
        sourceScrollArea->setWidget(scrollAreaWidgetContents_4);

        gridLayout_7->addWidget(sourceScrollArea, 0, 0, 1, 1);


        gridLayout_4->addWidget(sourceImg, 0, 0, 1, 1);

        outputImg = new QGroupBox(centralwidget);
        outputImg->setObjectName(QStringLiteral("outputImg"));
        outputImg->setMinimumSize(QSize(321, 321));
        gridLayout_8 = new QGridLayout(outputImg);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        outputSrcollArea = new QScrollArea(outputImg);
        outputSrcollArea->setObjectName(QStringLiteral("outputSrcollArea"));
        outputSrcollArea->setFrameShadow(QFrame::Plain);
        outputSrcollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QStringLiteral("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 321, 287));
        outputSrcollArea->setWidget(scrollAreaWidgetContents_3);

        gridLayout_8->addWidget(outputSrcollArea, 0, 0, 1, 1);


        gridLayout_4->addWidget(outputImg, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_2, 0, 1, 1, 1);


        gridLayout_6->addLayout(gridLayout_4, 1, 0, 1, 1);

        dipClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(dipClass);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 759, 23));
        fileMenu = new QMenu(menubar);
        fileMenu->setObjectName(QStringLiteral("fileMenu"));
        pixelMenu = new QMenu(menubar);
        pixelMenu->setObjectName(QStringLiteral("pixelMenu"));
        freMenu = new QMenu(menubar);
        freMenu->setObjectName(QStringLiteral("freMenu"));
        aboutMenu = new QMenu(menubar);
        aboutMenu->setObjectName(QStringLiteral("aboutMenu"));
        dipClass->setMenuBar(menubar);
        statusbar = new QStatusBar(dipClass);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        dipClass->setStatusBar(statusbar);

        menubar->addAction(fileMenu->menuAction());
        menubar->addAction(pixelMenu->menuAction());
        menubar->addAction(freMenu->menuAction());
        menubar->addAction(aboutMenu->menuAction());
        fileMenu->addAction(fileOpen);
        fileMenu->addAction(saveAs);
        pixelMenu->addAction(mirror);
        pixelMenu->addAction(offset);
        pixelMenu->addAction(crop);
        pixelMenu->addAction(scale);
        pixelMenu->addAction(rot);
        pixelMenu->addAction(filter);
        pixelMenu->addAction(histogram);
        freMenu->addAction(fft);
        freMenu->addAction(ifft);
        freMenu->addAction(freFilter);
        freMenu->addAction(dct);
        aboutMenu->addAction(author);

        retranslateUi(dipClass);
        QObject::connect(okButton, SIGNAL(clicked()), dipClass, SLOT(DIPRun()));

        QMetaObject::connectSlotsByName(dipClass);
    } // setupUi

    void retranslateUi(QMainWindow *dipClass)
    {
        dipClass->setWindowTitle(QApplication::translate("dipClass", "\346\225\260\345\255\227\345\233\276\345\203\217\345\244\204\347\220\206", 0));
        fileOpen->setText(QApplication::translate("dipClass", "\346\211\223\345\274\200", 0));
        saveAsPre->setText(QApplication::translate("dipClass", "\344\277\235\345\255\230\345\210\260\346\272\220\346\226\207\344\273\266\345\220\214\350\267\257\345\276\204", 0));
        saveAs->setText(QApplication::translate("dipClass", "\345\217\246\345\255\230\344\270\272", 0));
        mirror->setText(QApplication::translate("dipClass", "\351\225\234\345\203\217", 0));
        offset->setText(QApplication::translate("dipClass", "\345\271\263\347\247\273", 0));
        crop->setText(QApplication::translate("dipClass", "\345\211\252\345\210\207", 0));
        scale->setText(QApplication::translate("dipClass", "\347\274\251\346\224\276", 0));
        rot->setText(QApplication::translate("dipClass", "\346\227\213\350\275\254", 0));
        fft->setText(QApplication::translate("dipClass", "\345\202\205\351\207\214\345\217\266\345\217\230\346\215\242", 0));
        ifft->setText(QApplication::translate("dipClass", "\345\202\205\351\207\214\345\217\266\345\217\215\345\217\230\346\215\242", 0));
        freFilter->setText(QApplication::translate("dipClass", "\351\242\221\345\237\237\346\273\244\346\263\242", 0));
        filter->setText(QApplication::translate("dipClass", "\347\251\272\345\237\237\346\273\244\346\263\242", 0));
        histogram->setText(QApplication::translate("dipClass", "\347\233\264\346\226\271\345\233\276\345\235\207\350\241\241", 0));
        dct->setText(QApplication::translate("dipClass", "\344\275\231\345\274\246\345\217\230\346\215\242", 0));
        action5->setText(QApplication::translate("dipClass", "5", 0));
        action6->setText(QApplication::translate("dipClass", "6", 0));
        aboutDIP->setText(QApplication::translate("dipClass", "\350\257\276\347\250\213\344\273\213\347\273\215", 0));
        author->setText(QApplication::translate("dipClass", "\347\273\204\345\221\230", 0));
        colorGrid->setTitle(QApplication::translate("dipClass", "\351\274\240\346\240\207\345\217\226\350\211\262", 0));
        labelColor_1->setText(QApplication::translate("dipClass", "TextLabel", 0));
        labelColor_2->setText(QApplication::translate("dipClass", "TextLabel", 0));
        actionGroup->setTitle(QApplication::translate("dipClass", "\346\223\215\344\275\234", 0));
        radioGroup->setTitle(QApplication::translate("dipClass", "\345\215\225\351\200\211", 0));
        radioButton_1->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_2->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_3->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_4->setText(QApplication::translate("dipClass", "RadioButton", 0));
        comboGroup->setTitle(QApplication::translate("dipClass", "\344\270\213\346\213\211", 0));
        radioButton_5->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_6->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_7->setText(QApplication::translate("dipClass", "RadioButton", 0));
        radioButton_8->setText(QApplication::translate("dipClass", "RadioButton", 0));
        inputGroup->setTitle(QApplication::translate("dipClass", "\350\276\223\345\205\245", 0));
        label_1->setText(QApplication::translate("dipClass", "11111", 0));
        label_2->setText(QApplication::translate("dipClass", "11111", 0));
        lineEdit_2->setText(QString());
        label_3->setText(QApplication::translate("dipClass", "11111", 0));
        label_4->setText(QApplication::translate("dipClass", "11111", 0));
        label_5->setText(QApplication::translate("dipClass", "11111", 0));
        selectGroup->setTitle(QApplication::translate("dipClass", "\351\200\211\346\213\251", 0));
        resetButton->setText(QApplication::translate("dipClass", "\346\211\223\345\274\200\345\233\276\345\203\217", 0));
        okButton->setText(QApplication::translate("dipClass", "\350\277\220\350\241\214", 0));
        imgType->setTitle(QApplication::translate("dipClass", "\347\251\272\345\237\237\345\244\204\347\220\206", 0));
        radioImgType_1->setText(QApplication::translate("dipClass", "\345\275\251\350\211\262\345\233\276", 0));
        radioImgType_2->setText(QApplication::translate("dipClass", "\347\201\260\345\272\246\345\233\276", 0));
        sourceImg->setTitle(QApplication::translate("dipClass", "\346\272\220\345\233\276\345\203\217", 0));
        outputImg->setTitle(QApplication::translate("dipClass", "\350\276\223\345\207\272", 0));
        fileMenu->setTitle(QApplication::translate("dipClass", "\346\226\207\344\273\266", 0));
        pixelMenu->setTitle(QApplication::translate("dipClass", "\347\251\272\345\237\237", 0));
        freMenu->setTitle(QApplication::translate("dipClass", "\351\242\221\345\237\237", 0));
        aboutMenu->setTitle(QApplication::translate("dipClass", "\350\207\264\350\260\242", 0));
    } // retranslateUi

};

namespace Ui {
    class dipClass: public Ui_dipClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIP_H
