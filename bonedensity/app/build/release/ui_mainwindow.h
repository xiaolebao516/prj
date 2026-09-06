/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <agesoschartwidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *pageMain;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout;
    QPushButton *btnArchive;
    QPushButton *connectButton;
    QPushButton *pushButton_2;
    QPushButton *btnReport;
    QPushButton *wifi_button;
    QPushButton *triggerButton;
    QPushButton *pushButton;
    QPushButton *btnSaveResult;
    QComboBox *comboPort;
    QWidget *mainBodyWidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayoutMainBody;
    QWidget *leftColumnWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *grpWaveArea;
    QWidget *layoutWidget_3;
    QVBoxLayout *verticalLayoutChart;
    QGroupBox *grpSpeedArea;
    QChartView *chartViewSpeed;
    QWidget *centerColumnWidget;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *grpReferenceCurveArea;
    AgeSosChartWidget *chartViewReference;
    QGroupBox *grpProcessArea;
    QLabel *lblProcessTitle;
    QWidget *widgetBalanceArea;
    QLabel *lblBPairTitle;
    QProgressBar *barPairB;
    QLabel *lblPairBValue;
    QProgressBar *barPairA;
    QLabel *lblPairAValue;
    QLabel *lblAPairTitle;
    QProgressBar *barMeasureProgress;
    QLabel *lblGateStats;
    QLabel *lblProcessStatus;
    QWidget *rightColumnWidget;
    QGroupBox *grpPatientInfoRight;
    QLabel *labelID;
    QLabel *labelWeight;
    QLabel *labelName;
    QLabel *labelGender;
    QLabel *label_28;
    QLabel *labelHeight;
    QLabel *label_27;
    QLabel *labelBirth;
    QPushButton *btnPatientInfo;
    QPushButton *btnStartMeasurement;
    QGroupBox *grpLatestResultRight;
    QLabel *label_33;
    QLabel *label_34;
    QLabel *label_35;
    QLabel *label_36;
    QLabel *label_37;
    QLabel *label_38;
    QLabel *label_39;
    QLabel *lblLatestPart;
    QLabel *lblLatestSOS;
    QLabel *lblLatestT;
    QLabel *lblLatestZ;
    QLabel *lblLatestStrength;
    QLabel *lblLatestRisk;
    QLabel *lblLatestBoneAge;
    QGroupBox *grpPartImageRight;
    QLabel *label_32;
    QWidget *pageReport;
    QWidget *pagePatientSelect;
    QLineEdit *eName;
    QLineEdit *eID;
    QComboBox *eGender;
    QDateEdit *eBirth;
    QLineEdit *eHeight;
    QLineEdit *eWeight;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QLabel *label_25;
    QLabel *label_26;
    QPushButton *btnPatientNewSave;
    QPushButton *btnImportFromDB;
    QPushButton *btnBackToMain;
    QWidget *pagePatientForm;
    QLineEdit *editName;
    QLineEdit *editID;
    QLineEdit *editDiag;
    QLineEdit *editWeight;
    QLineEdit *editHeight;
    QComboBox *comboGender;
    QDateEdit *dateBirth;
    QPushButton *btnFormSave;
    QPushButton *btnFormBack;
    QLabel *label;
    QLabel *label_18;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QDateEdit *dateCheck;
    QWidget *pagePatientDetail;
    QLabel *label_10;
    QPushButton *btnDetailSave;
    QLabel *label_11;
    QLineEdit *dName;
    QLabel *label_12;
    QLineEdit *dWeight;
    QLineEdit *dID;
    QLineEdit *dHeight;
    QComboBox *dGender;
    QPushButton *btnDetailDelete;
    QDateEdit *dBirth;
    QLabel *label_13;
    QLabel *label_14;
    QLineEdit *dDiag;
    QLabel *label_16;
    QLabel *label_17;
    QPushButton *btnDetailBack;
    QLabel *label_9;
    QDateEdit *dCheck;
    QWidget *pageLogin;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout_4;
    QLabel *label_19;
    QLineEdit *editUsername;
    QLabel *label_20;
    QLineEdit *editPassword;
    QLabel *lblLoginMsg;
    QPushButton *btnLogin;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QWidget *pageArchive;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_5;
    QLabel *label_29;
    QPushButton *btnBackFromArchive;
    QLabel *label_15;
    QPushButton *btnDeleteSelected;
    QLabel *label_4;
    QLabel *label_30;
    QPushButton *btnAdd;
    QPushButton *btnSelectPatient;
    QPushButton *btnViewHistory;
    QComboBox *comboMonth;
    QLabel *label_31;
    QPushButton *btnSearchName;
    QPushButton *btnShowAll;
    QTableWidget *table;
    QComboBox *comboDay;
    QLineEdit *editSearchKeyword;
    QPushButton *btnSearchID;
    QComboBox *comboYear;
    QPushButton *btnSearchDate;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1920, 1080);
        QFont font;
        font.setPointSize(12);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setMinimumSize(QSize(0, 0));
        stackedWidget->setFont(font);
        pageMain = new QWidget();
        pageMain->setObjectName("pageMain");
        QFont font1;
        font1.setPointSize(20);
        pageMain->setFont(font1);
        layoutWidget_2 = new QWidget(pageMain);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(0, 21, 1851, 62));
        gridLayout = new QGridLayout(layoutWidget_2);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        btnArchive = new QPushButton(layoutWidget_2);
        btnArchive->setObjectName("btnArchive");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnArchive->sizePolicy().hasHeightForWidth());
        btnArchive->setSizePolicy(sizePolicy);
        btnArchive->setMinimumSize(QSize(0, 60));
        btnArchive->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(btnArchive, 0, 4, 1, 1);

        connectButton = new QPushButton(layoutWidget_2);
        connectButton->setObjectName("connectButton");
        sizePolicy.setHeightForWidth(connectButton->sizePolicy().hasHeightForWidth());
        connectButton->setSizePolicy(sizePolicy);
        connectButton->setMinimumSize(QSize(0, 60));
        connectButton->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(connectButton, 0, 2, 1, 1);

        pushButton_2 = new QPushButton(layoutWidget_2);
        pushButton_2->setObjectName("pushButton_2");
        sizePolicy.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy);
        pushButton_2->setMinimumSize(QSize(0, 60));
        pushButton_2->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(pushButton_2, 0, 9, 1, 1);

        btnReport = new QPushButton(layoutWidget_2);
        btnReport->setObjectName("btnReport");
        sizePolicy.setHeightForWidth(btnReport->sizePolicy().hasHeightForWidth());
        btnReport->setSizePolicy(sizePolicy);
        btnReport->setMinimumSize(QSize(0, 60));
        btnReport->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(btnReport, 0, 10, 1, 1);

        wifi_button = new QPushButton(layoutWidget_2);
        wifi_button->setObjectName("wifi_button");
        wifi_button->setEnabled(false);
        sizePolicy.setHeightForWidth(wifi_button->sizePolicy().hasHeightForWidth());
        wifi_button->setSizePolicy(sizePolicy);
        wifi_button->setMinimumSize(QSize(0, 60));
        wifi_button->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(wifi_button, 0, 3, 1, 1);

        triggerButton = new QPushButton(layoutWidget_2);
        triggerButton->setObjectName("triggerButton");
        sizePolicy.setHeightForWidth(triggerButton->sizePolicy().hasHeightForWidth());
        triggerButton->setSizePolicy(sizePolicy);
        triggerButton->setMinimumSize(QSize(0, 60));
        triggerButton->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(triggerButton, 0, 8, 1, 1);

        pushButton = new QPushButton(layoutWidget_2);
        pushButton->setObjectName("pushButton");
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);
        pushButton->setMinimumSize(QSize(0, 60));
        pushButton->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(pushButton, 0, 7, 1, 1);

        btnSaveResult = new QPushButton(layoutWidget_2);
        btnSaveResult->setObjectName("btnSaveResult");
        sizePolicy.setHeightForWidth(btnSaveResult->sizePolicy().hasHeightForWidth());
        btnSaveResult->setSizePolicy(sizePolicy);
        btnSaveResult->setMinimumSize(QSize(0, 60));
        btnSaveResult->setMaximumSize(QSize(140, 16777215));

        gridLayout->addWidget(btnSaveResult, 0, 5, 1, 1);

        comboPort = new QComboBox(layoutWidget_2);
        comboPort->setObjectName("comboPort");
        sizePolicy.setHeightForWidth(comboPort->sizePolicy().hasHeightForWidth());
        comboPort->setSizePolicy(sizePolicy);
        comboPort->setMinimumSize(QSize(250, 40));
        comboPort->setMaximumSize(QSize(250, 40));

        gridLayout->addWidget(comboPort, 0, 1, 1, 1);

        mainBodyWidget = new QWidget(pageMain);
        mainBodyWidget->setObjectName("mainBodyWidget");
        mainBodyWidget->setGeometry(QRect(10, 99, 1871, 901));
        layoutWidget = new QWidget(mainBodyWidget);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(-1, -1, 1871, 901));
        horizontalLayoutMainBody = new QHBoxLayout(layoutWidget);
        horizontalLayoutMainBody->setObjectName("horizontalLayoutMainBody");
        horizontalLayoutMainBody->setContentsMargins(0, 0, 0, 0);
        leftColumnWidget = new QWidget(layoutWidget);
        leftColumnWidget->setObjectName("leftColumnWidget");
        verticalLayout = new QVBoxLayout(leftColumnWidget);
        verticalLayout->setObjectName("verticalLayout");
        grpWaveArea = new QGroupBox(leftColumnWidget);
        grpWaveArea->setObjectName("grpWaveArea");
        layoutWidget_3 = new QWidget(grpWaveArea);
        layoutWidget_3->setObjectName("layoutWidget_3");
        layoutWidget_3->setGeometry(QRect(0, 0, 721, 521));
        verticalLayoutChart = new QVBoxLayout(layoutWidget_3);
        verticalLayoutChart->setObjectName("verticalLayoutChart");
        verticalLayoutChart->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(grpWaveArea);

        grpSpeedArea = new QGroupBox(leftColumnWidget);
        grpSpeedArea->setObjectName("grpSpeedArea");
        chartViewSpeed = new QChartView(grpSpeedArea);
        chartViewSpeed->setObjectName("chartViewSpeed");
        chartViewSpeed->setGeometry(QRect(0, 0, 721, 351));

        verticalLayout->addWidget(grpSpeedArea);

        verticalLayout->setStretch(0, 6);
        verticalLayout->setStretch(1, 4);

        horizontalLayoutMainBody->addWidget(leftColumnWidget);

        centerColumnWidget = new QWidget(layoutWidget);
        centerColumnWidget->setObjectName("centerColumnWidget");
        verticalLayout_2 = new QVBoxLayout(centerColumnWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        grpReferenceCurveArea = new QGroupBox(centerColumnWidget);
        grpReferenceCurveArea->setObjectName("grpReferenceCurveArea");
        chartViewReference = new AgeSosChartWidget(grpReferenceCurveArea);
        chartViewReference->setObjectName("chartViewReference");
        chartViewReference->setGeometry(QRect(0, 1, 531, 431));

        verticalLayout_2->addWidget(grpReferenceCurveArea);

        grpProcessArea = new QGroupBox(centerColumnWidget);
        grpProcessArea->setObjectName("grpProcessArea");
        lblProcessTitle = new QLabel(grpProcessArea);
        lblProcessTitle->setObjectName("lblProcessTitle");
        lblProcessTitle->setGeometry(QRect(170, 20, 191, 31));
        widgetBalanceArea = new QWidget(grpProcessArea);
        widgetBalanceArea->setObjectName("widgetBalanceArea");
        widgetBalanceArea->setGeometry(QRect(30, 70, 481, 361));
        lblBPairTitle = new QLabel(widgetBalanceArea);
        lblBPairTitle->setObjectName("lblBPairTitle");
        lblBPairTitle->setGeometry(QRect(-40, 0, 221, 31));
        lblBPairTitle->setAlignment(Qt::AlignCenter);
        barPairB = new QProgressBar(widgetBalanceArea);
        barPairB->setObjectName("barPairB");
        barPairB->setGeometry(QRect(30, 30, 81, 201));
        barPairB->setMaximum(1000);
        barPairB->setValue(500);
        barPairB->setTextVisible(false);
        barPairB->setOrientation(Qt::Vertical);
        lblPairBValue = new QLabel(widgetBalanceArea);
        lblPairBValue->setObjectName("lblPairBValue");
        lblPairBValue->setGeometry(QRect(-10, 230, 231, 31));
        lblPairBValue->setAlignment(Qt::AlignCenter);
        barPairA = new QProgressBar(widgetBalanceArea);
        barPairA->setObjectName("barPairA");
        barPairA->setGeometry(QRect(370, 30, 81, 201));
        barPairA->setMaximum(1000);
        barPairA->setValue(500);
        barPairA->setTextVisible(false);
        barPairA->setOrientation(Qt::Vertical);
        lblPairAValue = new QLabel(widgetBalanceArea);
        lblPairAValue->setObjectName("lblPairAValue");
        lblPairAValue->setGeometry(QRect(240, 230, 261, 31));
        lblPairAValue->setAlignment(Qt::AlignCenter);
        lblAPairTitle = new QLabel(widgetBalanceArea);
        lblAPairTitle->setObjectName("lblAPairTitle");
        lblAPairTitle->setGeometry(QRect(310, 0, 191, 31));
        lblAPairTitle->setAlignment(Qt::AlignCenter);
        barMeasureProgress = new QProgressBar(widgetBalanceArea);
        barMeasureProgress->setObjectName("barMeasureProgress");
        barMeasureProgress->setGeometry(QRect(100, 270, 281, 61));
        barMeasureProgress->setMaximum(20);
        barMeasureProgress->setValue(0);
        barMeasureProgress->setTextVisible(false);
        lblGateStats = new QLabel(widgetBalanceArea);
        lblGateStats->setObjectName("lblGateStats");
        lblGateStats->setGeometry(QRect(0, 315, 481, 25));
        QFont font2;
        font2.setPointSize(9);
        lblGateStats->setFont(font2);
        lblGateStats->setAlignment(Qt::AlignCenter);
        lblProcessStatus = new QLabel(widgetBalanceArea);
        lblProcessStatus->setObjectName("lblProcessStatus");
        lblProcessStatus->setGeometry(QRect(0, 340, 481, 81));
        QFont font3;
        font3.setPointSize(10);
        lblProcessStatus->setFont(font3);
        lblProcessStatus->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(grpProcessArea);


        horizontalLayoutMainBody->addWidget(centerColumnWidget);

        rightColumnWidget = new QWidget(layoutWidget);
        rightColumnWidget->setObjectName("rightColumnWidget");
        grpPatientInfoRight = new QGroupBox(rightColumnWidget);
        grpPatientInfoRight->setObjectName("grpPatientInfoRight");
        grpPatientInfoRight->setGeometry(QRect(0, 0, 561, 341));
        QFont font4;
        font4.setPointSize(18);
        grpPatientInfoRight->setFont(font4);
        labelID = new QLabel(grpPatientInfoRight);
        labelID->setObjectName("labelID");
        labelID->setGeometry(QRect(20, 80, 171, 31));
        sizePolicy.setHeightForWidth(labelID->sizePolicy().hasHeightForWidth());
        labelID->setSizePolicy(sizePolicy);
        QFont font5;
        font5.setPointSize(16);
        labelID->setFont(font5);
        labelWeight = new QLabel(grpPatientInfoRight);
        labelWeight->setObjectName("labelWeight");
        labelWeight->setGeometry(QRect(20, 240, 131, 31));
        sizePolicy.setHeightForWidth(labelWeight->sizePolicy().hasHeightForWidth());
        labelWeight->setSizePolicy(sizePolicy);
        labelWeight->setFont(font5);
        labelName = new QLabel(grpPatientInfoRight);
        labelName->setObjectName("labelName");
        labelName->setGeometry(QRect(20, 40, 181, 31));
        sizePolicy.setHeightForWidth(labelName->sizePolicy().hasHeightForWidth());
        labelName->setSizePolicy(sizePolicy);
        labelName->setFont(font5);
        labelGender = new QLabel(grpPatientInfoRight);
        labelGender->setObjectName("labelGender");
        labelGender->setGeometry(QRect(20, 120, 171, 31));
        sizePolicy.setHeightForWidth(labelGender->sizePolicy().hasHeightForWidth());
        labelGender->setSizePolicy(sizePolicy);
        labelGender->setFont(font5);
        label_28 = new QLabel(grpPatientInfoRight);
        label_28->setObjectName("label_28");
        label_28->setGeometry(QRect(170, 240, 51, 31));
        sizePolicy.setHeightForWidth(label_28->sizePolicy().hasHeightForWidth());
        label_28->setSizePolicy(sizePolicy);
        label_28->setFont(font5);
        labelHeight = new QLabel(grpPatientInfoRight);
        labelHeight->setObjectName("labelHeight");
        labelHeight->setGeometry(QRect(20, 200, 131, 31));
        sizePolicy.setHeightForWidth(labelHeight->sizePolicy().hasHeightForWidth());
        labelHeight->setSizePolicy(sizePolicy);
        labelHeight->setFont(font5);
        label_27 = new QLabel(grpPatientInfoRight);
        label_27->setObjectName("label_27");
        label_27->setGeometry(QRect(170, 200, 41, 31));
        sizePolicy.setHeightForWidth(label_27->sizePolicy().hasHeightForWidth());
        label_27->setSizePolicy(sizePolicy);
        label_27->setFont(font5);
        labelBirth = new QLabel(grpPatientInfoRight);
        labelBirth->setObjectName("labelBirth");
        labelBirth->setGeometry(QRect(20, 160, 211, 31));
        sizePolicy.setHeightForWidth(labelBirth->sizePolicy().hasHeightForWidth());
        labelBirth->setSizePolicy(sizePolicy);
        labelBirth->setFont(font5);
        btnPatientInfo = new QPushButton(grpPatientInfoRight);
        btnPatientInfo->setObjectName("btnPatientInfo");
        btnPatientInfo->setGeometry(QRect(291, 270, 170, 60));
        sizePolicy.setHeightForWidth(btnPatientInfo->sizePolicy().hasHeightForWidth());
        btnPatientInfo->setSizePolicy(sizePolicy);
        btnPatientInfo->setMinimumSize(QSize(0, 60));
        btnPatientInfo->setMaximumSize(QSize(170, 16777215));
        btnStartMeasurement = new QPushButton(grpPatientInfoRight);
        btnStartMeasurement->setObjectName("btnStartMeasurement");
        btnStartMeasurement->setGeometry(QRect(100, 270, 170, 60));
        grpLatestResultRight = new QGroupBox(rightColumnWidget);
        grpLatestResultRight->setObjectName("grpLatestResultRight");
        grpLatestResultRight->setGeometry(QRect(0, 340, 561, 331));
        grpLatestResultRight->setFont(font4);
        label_33 = new QLabel(grpLatestResultRight);
        label_33->setObjectName("label_33");
        label_33->setGeometry(QRect(20, 40, 131, 21));
        QFont font6;
        font6.setPointSize(14);
        label_33->setFont(font6);
        label_34 = new QLabel(grpLatestResultRight);
        label_34->setObjectName("label_34");
        label_34->setGeometry(QRect(20, 70, 121, 21));
        label_34->setFont(font6);
        label_35 = new QLabel(grpLatestResultRight);
        label_35->setObjectName("label_35");
        label_35->setGeometry(QRect(20, 100, 121, 21));
        label_35->setFont(font6);
        label_36 = new QLabel(grpLatestResultRight);
        label_36->setObjectName("label_36");
        label_36->setGeometry(QRect(20, 130, 121, 21));
        label_36->setFont(font6);
        label_37 = new QLabel(grpLatestResultRight);
        label_37->setObjectName("label_37");
        label_37->setGeometry(QRect(20, 160, 111, 21));
        label_37->setFont(font6);
        label_38 = new QLabel(grpLatestResultRight);
        label_38->setObjectName("label_38");
        label_38->setGeometry(QRect(20, 190, 131, 21));
        label_38->setFont(font6);
        label_39 = new QLabel(grpLatestResultRight);
        label_39->setObjectName("label_39");
        label_39->setGeometry(QRect(20, 220, 121, 21));
        label_39->setFont(font6);
        lblLatestPart = new QLabel(grpLatestResultRight);
        lblLatestPart->setObjectName("lblLatestPart");
        lblLatestPart->setGeometry(QRect(120, 40, 111, 21));
        lblLatestPart->setFont(font6);
        lblLatestSOS = new QLabel(grpLatestResultRight);
        lblLatestSOS->setObjectName("lblLatestSOS");
        lblLatestSOS->setGeometry(QRect(80, 70, 111, 20));
        lblLatestSOS->setFont(font6);
        lblLatestT = new QLabel(grpLatestResultRight);
        lblLatestT->setObjectName("lblLatestT");
        lblLatestT->setGeometry(QRect(80, 100, 91, 20));
        lblLatestT->setFont(font6);
        lblLatestZ = new QLabel(grpLatestResultRight);
        lblLatestZ->setObjectName("lblLatestZ");
        lblLatestZ->setGeometry(QRect(80, 130, 121, 21));
        lblLatestZ->setFont(font6);
        lblLatestStrength = new QLabel(grpLatestResultRight);
        lblLatestStrength->setObjectName("lblLatestStrength");
        lblLatestStrength->setGeometry(QRect(100, 160, 121, 21));
        lblLatestStrength->setFont(font6);
        lblLatestRisk = new QLabel(grpLatestResultRight);
        lblLatestRisk->setObjectName("lblLatestRisk");
        lblLatestRisk->setGeometry(QRect(150, 190, 141, 21));
        lblLatestRisk->setFont(font6);
        lblLatestBoneAge = new QLabel(grpLatestResultRight);
        lblLatestBoneAge->setObjectName("lblLatestBoneAge");
        lblLatestBoneAge->setGeometry(QRect(120, 220, 101, 21));
        lblLatestBoneAge->setFont(font6);
        grpPartImageRight = new QGroupBox(rightColumnWidget);
        grpPartImageRight->setObjectName("grpPartImageRight");
        grpPartImageRight->setGeometry(QRect(0, 670, 561, 231));
        grpPartImageRight->setFont(font4);
        label_32 = new QLabel(grpPartImageRight);
        label_32->setObjectName("label_32");
        label_32->setGeometry(QRect(0, 0, 561, 221));
        label_32->setPixmap(QPixmap(QString::fromUtf8(":/images/Radius.bmp")));
        label_32->setScaledContents(true);

        horizontalLayoutMainBody->addWidget(rightColumnWidget);

        horizontalLayoutMainBody->setStretch(0, 4);
        horizontalLayoutMainBody->setStretch(1, 3);
        horizontalLayoutMainBody->setStretch(2, 3);
        stackedWidget->addWidget(pageMain);
        pageReport = new QWidget();
        pageReport->setObjectName("pageReport");
        stackedWidget->addWidget(pageReport);
        pagePatientSelect = new QWidget();
        pagePatientSelect->setObjectName("pagePatientSelect");
        pagePatientSelect->setFont(font1);
        eName = new QLineEdit(pagePatientSelect);
        eName->setObjectName("eName");
        eName->setGeometry(QRect(680, 90, 721, 51));
        eID = new QLineEdit(pagePatientSelect);
        eID->setObjectName("eID");
        eID->setGeometry(QRect(680, 220, 721, 51));
        eGender = new QComboBox(pagePatientSelect);
        eGender->addItem(QString());
        eGender->addItem(QString());
        eGender->setObjectName("eGender");
        eGender->setGeometry(QRect(680, 350, 721, 51));
        eBirth = new QDateEdit(pagePatientSelect);
        eBirth->setObjectName("eBirth");
        eBirth->setGeometry(QRect(680, 480, 721, 51));
        eHeight = new QLineEdit(pagePatientSelect);
        eHeight->setObjectName("eHeight");
        eHeight->setGeometry(QRect(680, 612, 721, 51));
        eWeight = new QLineEdit(pagePatientSelect);
        eWeight->setObjectName("eWeight");
        eWeight->setGeometry(QRect(680, 740, 721, 51));
        label_21 = new QLabel(pagePatientSelect);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(560, 90, 101, 51));
        label_22 = new QLabel(pagePatientSelect);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(560, 220, 101, 41));
        label_23 = new QLabel(pagePatientSelect);
        label_23->setObjectName("label_23");
        label_23->setGeometry(QRect(560, 350, 81, 51));
        label_24 = new QLabel(pagePatientSelect);
        label_24->setObjectName("label_24");
        label_24->setGeometry(QRect(550, 480, 131, 51));
        label_25 = new QLabel(pagePatientSelect);
        label_25->setObjectName("label_25");
        label_25->setGeometry(QRect(560, 610, 111, 41));
        label_26 = new QLabel(pagePatientSelect);
        label_26->setObjectName("label_26");
        label_26->setGeometry(QRect(560, 740, 111, 41));
        btnPatientNewSave = new QPushButton(pagePatientSelect);
        btnPatientNewSave->setObjectName("btnPatientNewSave");
        btnPatientNewSave->setGeometry(QRect(750, 820, 170, 60));
        btnImportFromDB = new QPushButton(pagePatientSelect);
        btnImportFromDB->setObjectName("btnImportFromDB");
        btnImportFromDB->setGeometry(QRect(960, 820, 170, 60));
        btnBackToMain = new QPushButton(pagePatientSelect);
        btnBackToMain->setObjectName("btnBackToMain");
        btnBackToMain->setGeometry(QRect(1170, 820, 170, 60));
        stackedWidget->addWidget(pagePatientSelect);
        pagePatientForm = new QWidget();
        pagePatientForm->setObjectName("pagePatientForm");
        pagePatientForm->setFont(font1);
        editName = new QLineEdit(pagePatientForm);
        editName->setObjectName("editName");
        editName->setGeometry(QRect(700, 40, 611, 41));
        editID = new QLineEdit(pagePatientForm);
        editID->setObjectName("editID");
        editID->setGeometry(QRect(700, 120, 611, 41));
        editDiag = new QLineEdit(pagePatientForm);
        editDiag->setObjectName("editDiag");
        editDiag->setGeometry(QRect(700, 680, 611, 161));
        editWeight = new QLineEdit(pagePatientForm);
        editWeight->setObjectName("editWeight");
        editWeight->setGeometry(QRect(700, 440, 611, 41));
        editHeight = new QLineEdit(pagePatientForm);
        editHeight->setObjectName("editHeight");
        editHeight->setGeometry(QRect(700, 360, 611, 41));
        comboGender = new QComboBox(pagePatientForm);
        comboGender->addItem(QString());
        comboGender->addItem(QString());
        comboGender->setObjectName("comboGender");
        comboGender->setGeometry(QRect(700, 200, 611, 41));
        dateBirth = new QDateEdit(pagePatientForm);
        dateBirth->setObjectName("dateBirth");
        dateBirth->setGeometry(QRect(700, 280, 611, 41));
        btnFormSave = new QPushButton(pagePatientForm);
        btnFormSave->setObjectName("btnFormSave");
        btnFormSave->setGeometry(QRect(790, 560, 171, 71));
        btnFormBack = new QPushButton(pagePatientForm);
        btnFormBack->setObjectName("btnFormBack");
        btnFormBack->setGeometry(QRect(1050, 560, 171, 71));
        label = new QLabel(pagePatientForm);
        label->setObjectName("label");
        label->setGeometry(QRect(550, 40, 101, 41));
        label_18 = new QLabel(pagePatientForm);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(550, 120, 101, 41));
        label_2 = new QLabel(pagePatientForm);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(550, 200, 101, 41));
        label_3 = new QLabel(pagePatientForm);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(550, 280, 131, 41));
        label_5 = new QLabel(pagePatientForm);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(550, 360, 121, 41));
        label_6 = new QLabel(pagePatientForm);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(550, 440, 121, 41));
        label_7 = new QLabel(pagePatientForm);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(550, 680, 121, 51));
        label_8 = new QLabel(pagePatientForm);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(550, 360, 121, 41));
        dateCheck = new QDateEdit(pagePatientForm);
        dateCheck->setObjectName("dateCheck");
        dateCheck->setGeometry(QRect(700, 360, 611, 41));
        stackedWidget->addWidget(pagePatientForm);
        pagePatientDetail = new QWidget();
        pagePatientDetail->setObjectName("pagePatientDetail");
        pagePatientDetail->setFont(font1);
        label_10 = new QLabel(pagePatientDetail);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(630, 430, 121, 41));
        btnDetailSave = new QPushButton(pagePatientDetail);
        btnDetailSave->setObjectName("btnDetailSave");
        btnDetailSave->setGeometry(QRect(790, 550, 161, 71));
        btnDetailSave->setFont(font1);
        label_11 = new QLabel(pagePatientDetail);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(630, 660, 121, 61));
        dName = new QLineEdit(pagePatientDetail);
        dName->setObjectName("dName");
        dName->setGeometry(QRect(770, 30, 611, 41));
        label_12 = new QLabel(pagePatientDetail);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(630, 350, 121, 41));
        dWeight = new QLineEdit(pagePatientDetail);
        dWeight->setObjectName("dWeight");
        dWeight->setGeometry(QRect(770, 430, 611, 41));
        dID = new QLineEdit(pagePatientDetail);
        dID->setObjectName("dID");
        dID->setGeometry(QRect(770, 110, 611, 41));
        dHeight = new QLineEdit(pagePatientDetail);
        dHeight->setObjectName("dHeight");
        dHeight->setGeometry(QRect(770, 350, 611, 41));
        dGender = new QComboBox(pagePatientDetail);
        dGender->addItem(QString());
        dGender->addItem(QString());
        dGender->setObjectName("dGender");
        dGender->setGeometry(QRect(770, 190, 611, 41));
        btnDetailDelete = new QPushButton(pagePatientDetail);
        btnDetailDelete->setObjectName("btnDetailDelete");
        btnDetailDelete->setGeometry(QRect(1000, 550, 151, 71));
        dBirth = new QDateEdit(pagePatientDetail);
        dBirth->setObjectName("dBirth");
        dBirth->setGeometry(QRect(770, 270, 611, 41));
        label_13 = new QLabel(pagePatientDetail);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(630, 110, 81, 41));
        label_14 = new QLabel(pagePatientDetail);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(630, 190, 101, 41));
        dDiag = new QLineEdit(pagePatientDetail);
        dDiag->setObjectName("dDiag");
        dDiag->setGeometry(QRect(770, 670, 611, 101));
        label_16 = new QLabel(pagePatientDetail);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(630, 30, 101, 41));
        label_17 = new QLabel(pagePatientDetail);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(630, 270, 111, 41));
        btnDetailBack = new QPushButton(pagePatientDetail);
        btnDetailBack->setObjectName("btnDetailBack");
        btnDetailBack->setGeometry(QRect(1210, 550, 151, 71));
        label_9 = new QLabel(pagePatientDetail);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(630, 350, 121, 41));
        dCheck = new QDateEdit(pagePatientDetail);
        dCheck->setObjectName("dCheck");
        dCheck->setGeometry(QRect(770, 350, 611, 41));
        stackedWidget->addWidget(pagePatientDetail);
        pageLogin = new QWidget();
        pageLogin->setObjectName("pageLogin");
        pageLogin->setFont(font1);
        gridLayout_3 = new QGridLayout(pageLogin);
        gridLayout_3->setObjectName("gridLayout_3");
        verticalSpacer = new QSpacerItem(20, 400, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(615, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 1, 0, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName("gridLayout_4");
        label_19 = new QLabel(pageLogin);
        label_19->setObjectName("label_19");

        gridLayout_4->addWidget(label_19, 0, 0, 1, 1);

        editUsername = new QLineEdit(pageLogin);
        editUsername->setObjectName("editUsername");

        gridLayout_4->addWidget(editUsername, 0, 1, 1, 1);

        label_20 = new QLabel(pageLogin);
        label_20->setObjectName("label_20");

        gridLayout_4->addWidget(label_20, 1, 0, 1, 1);

        editPassword = new QLineEdit(pageLogin);
        editPassword->setObjectName("editPassword");

        gridLayout_4->addWidget(editPassword, 1, 1, 1, 1);

        lblLoginMsg = new QLabel(pageLogin);
        lblLoginMsg->setObjectName("lblLoginMsg");
        lblLoginMsg->setStyleSheet(QString::fromUtf8("color: red;"));
        lblLoginMsg->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(lblLoginMsg, 2, 1, 1, 1);

        btnLogin = new QPushButton(pageLogin);
        btnLogin->setObjectName("btnLogin");

        gridLayout_4->addWidget(btnLogin, 3, 1, 1, 1);


        gridLayout_3->addLayout(gridLayout_4, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(615, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 400, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 2, 1, 1, 1);

        stackedWidget->addWidget(pageLogin);
        pageArchive = new QWidget();
        pageArchive->setObjectName("pageArchive");
        pageArchive->setFont(font1);
        gridLayout_6 = new QGridLayout(pageArchive);
        gridLayout_6->setObjectName("gridLayout_6");
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName("gridLayout_5");
        label_29 = new QLabel(pageArchive);
        label_29->setObjectName("label_29");

        gridLayout_5->addWidget(label_29, 1, 3, 1, 1);

        btnBackFromArchive = new QPushButton(pageArchive);
        btnBackFromArchive->setObjectName("btnBackFromArchive");

        gridLayout_5->addWidget(btnBackFromArchive, 0, 0, 1, 1);

        label_15 = new QLabel(pageArchive);
        label_15->setObjectName("label_15");

        gridLayout_5->addWidget(label_15, 1, 1, 1, 1);

        btnDeleteSelected = new QPushButton(pageArchive);
        btnDeleteSelected->setObjectName("btnDeleteSelected");

        gridLayout_5->addWidget(btnDeleteSelected, 0, 13, 1, 1);

        label_4 = new QLabel(pageArchive);
        label_4->setObjectName("label_4");

        gridLayout_5->addWidget(label_4, 0, 1, 1, 1);

        label_30 = new QLabel(pageArchive);
        label_30->setObjectName("label_30");

        gridLayout_5->addWidget(label_30, 1, 5, 1, 1);

        btnAdd = new QPushButton(pageArchive);
        btnAdd->setObjectName("btnAdd");

        gridLayout_5->addWidget(btnAdd, 0, 10, 1, 1);

        btnSelectPatient = new QPushButton(pageArchive);
        btnSelectPatient->setObjectName("btnSelectPatient");

        gridLayout_5->addWidget(btnSelectPatient, 0, 11, 1, 1);

        btnViewHistory = new QPushButton(pageArchive);
        btnViewHistory->setObjectName("btnViewHistory");

        gridLayout_5->addWidget(btnViewHistory, 0, 12, 1, 1);

        comboMonth = new QComboBox(pageArchive);
        comboMonth->setObjectName("comboMonth");

        gridLayout_5->addWidget(comboMonth, 1, 4, 1, 1);

        label_31 = new QLabel(pageArchive);
        label_31->setObjectName("label_31");

        gridLayout_5->addWidget(label_31, 1, 7, 1, 1);

        btnSearchName = new QPushButton(pageArchive);
        btnSearchName->setObjectName("btnSearchName");

        gridLayout_5->addWidget(btnSearchName, 0, 8, 1, 1);

        btnShowAll = new QPushButton(pageArchive);
        btnShowAll->setObjectName("btnShowAll");

        gridLayout_5->addWidget(btnShowAll, 1, 0, 1, 1);

        table = new QTableWidget(pageArchive);
        table->setObjectName("table");

        gridLayout_5->addWidget(table, 2, 0, 1, 14);

        comboDay = new QComboBox(pageArchive);
        comboDay->setObjectName("comboDay");

        gridLayout_5->addWidget(comboDay, 1, 6, 1, 1);

        editSearchKeyword = new QLineEdit(pageArchive);
        editSearchKeyword->setObjectName("editSearchKeyword");

        gridLayout_5->addWidget(editSearchKeyword, 0, 2, 1, 6);

        btnSearchID = new QPushButton(pageArchive);
        btnSearchID->setObjectName("btnSearchID");
        btnSearchID->setVisible(false);

        gridLayout_5->addWidget(btnSearchID, 0, 9, 1, 1);

        comboYear = new QComboBox(pageArchive);
        comboYear->setObjectName("comboYear");

        gridLayout_5->addWidget(comboYear, 1, 2, 1, 1);

        btnSearchDate = new QPushButton(pageArchive);
        btnSearchDate->setObjectName("btnSearchDate");

        gridLayout_5->addWidget(btnSearchDate, 1, 8, 1, 1);


        gridLayout_6->addLayout(gridLayout_5, 0, 0, 1, 2);

        stackedWidget->addWidget(pageArchive);

        gridLayout_2->addWidget(stackedWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1920, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnArchive->setText(QCoreApplication::translate("MainWindow", "\346\241\243\346\241\210", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\346\240\241\345\207\206", nullptr));
        btnReport->setText(QCoreApplication::translate("MainWindow", "\346\212\245\350\241\250", nullptr));
        wifi_button->setText(QCoreApplication::translate("MainWindow", "WIFI", nullptr));
#if QT_CONFIG(tooltip)
        wifi_button->setToolTip(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275\345\260\232\346\234\252\345\274\200\346\224\276", nullptr));
#endif // QT_CONFIG(tooltip)
        triggerButton->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\351\207\207\351\233\206", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\350\216\267\345\217\226\346\263\242\345\275\242", nullptr));
        btnSaveResult->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        grpWaveArea->setTitle(QCoreApplication::translate("MainWindow", "\345\233\233\351\200\232\351\201\223\346\263\242\345\275\242", nullptr));
        grpSpeedArea->setTitle(QCoreApplication::translate("MainWindow", "\345\243\260\351\200\237\350\266\213\345\212\277", nullptr));
        grpReferenceCurveArea->setTitle(QCoreApplication::translate("MainWindow", "\345\271\264\351\276\204-SOS\345\217\202\350\200\203", nullptr));
        grpProcessArea->setTitle(QCoreApplication::translate("MainWindow", "\346\243\200\346\265\213\350\277\207\347\250\213", nullptr));
        lblProcessTitle->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\350\277\207\347\250\213\347\244\272\346\204\217\345\214\272", nullptr));
        lblBPairTitle->setText(QCoreApplication::translate("MainWindow", "G \344\275\215\347\275\256\345\271\263\350\241\241", nullptr));
        lblPairBValue->setText(QCoreApplication::translate("MainWindow", "G=-- \347\233\256\346\240\207=0.0", nullptr));
        lblPairAValue->setText(QCoreApplication::translate("MainWindow", "D=-- \347\233\256\346\240\207=9.0", nullptr));
        lblAPairTitle->setText(QCoreApplication::translate("MainWindow", "D \345\273\266\350\277\237\344\270\200\350\207\264\346\200\247", nullptr));
        lblGateStats->setText(QString());
        lblProcessStatus->setText(QCoreApplication::translate("MainWindow", "\347\255\211\345\276\205\345\274\200\345\247\213\346\265\213\351\207\217", nullptr));
        grpPatientInfoRight->setTitle(QCoreApplication::translate("MainWindow", "\350\242\253\346\265\213\350\200\205\344\277\241\346\201\257", nullptr));
        labelID->setText(QCoreApplication::translate("MainWindow", "ID:", nullptr));
        labelWeight->setText(QCoreApplication::translate("MainWindow", "\344\275\223\351\207\215\357\274\232", nullptr));
        labelName->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215\357\274\232", nullptr));
        labelGender->setText(QCoreApplication::translate("MainWindow", "\346\200\247\345\210\253\357\274\232", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "kg", nullptr));
        labelHeight->setText(QCoreApplication::translate("MainWindow", "\350\272\253\351\253\230\357\274\232", nullptr));
        label_27->setText(QCoreApplication::translate("MainWindow", "cm", nullptr));
        labelBirth->setText(QCoreApplication::translate("MainWindow", "\345\207\272\347\224\237\346\227\245\346\234\237\357\274\232", nullptr));
        btnPatientInfo->setText(QCoreApplication::translate("MainWindow", "\345\273\272\347\253\213\346\241\243\346\241\210", nullptr));
        btnStartMeasurement->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\346\243\200\346\265\213", nullptr));
        grpLatestResultRight->setTitle(QCoreApplication::translate("MainWindow", "\346\265\213\351\207\217\347\273\223\346\236\234", nullptr));
        label_33->setText(QCoreApplication::translate("MainWindow", "\346\265\213\351\207\217\351\203\250\344\275\215\357\274\232", nullptr));
        label_34->setText(QCoreApplication::translate("MainWindow", "SOS\357\274\232", nullptr));
        label_35->setText(QCoreApplication::translate("MainWindow", "T\345\200\274\357\274\232", nullptr));
        label_36->setText(QCoreApplication::translate("MainWindow", "Z\345\200\274\357\274\232", nullptr));
        label_37->setText(QCoreApplication::translate("MainWindow", "\351\252\250\345\274\272\345\272\246\357\274\232", nullptr));
        label_38->setText(QCoreApplication::translate("MainWindow", "\347\233\270\345\257\271\351\252\250\346\212\230\351\243\216\351\231\251\357\274\232", nullptr));
        label_39->setText(QCoreApplication::translate("MainWindow", "\347\233\270\345\257\271\351\252\250\351\276\204\357\274\232", nullptr));
        lblLatestPart->setText(QCoreApplication::translate("MainWindow", "\346\241\241\351\252\250", nullptr));
        lblLatestSOS->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        lblLatestT->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        lblLatestZ->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        lblLatestStrength->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        lblLatestRisk->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        lblLatestBoneAge->setText(QCoreApplication::translate("MainWindow", "--", nullptr));
        grpPartImageRight->setTitle(QCoreApplication::translate("MainWindow", "\346\265\213\351\207\217\351\203\250\344\275\215", nullptr));
        label_32->setText(QString());
        eGender->setItemText(0, QCoreApplication::translate("MainWindow", "\347\224\267", nullptr));
        eGender->setItemText(1, QCoreApplication::translate("MainWindow", "\345\245\263", nullptr));

        label_21->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "\346\200\247\345\210\253", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "\345\207\272\347\224\237\346\227\245\346\234\237", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "\350\272\253\351\253\230(cm)", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "\344\275\223\351\207\215(kg)", nullptr));
        btnPatientNewSave->setText(QCoreApplication::translate("MainWindow", "\347\241\256\345\256\232", nullptr));
        btnImportFromDB->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\205\245", nullptr));
        btnBackToMain->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        comboGender->setItemText(0, QCoreApplication::translate("MainWindow", "\347\224\267", nullptr));
        comboGender->setItemText(1, QCoreApplication::translate("MainWindow", "\345\245\263", nullptr));

        btnFormSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        btnFormBack->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\200\247\345\210\253", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\345\207\272\347\224\237\346\227\245\346\234\237", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\350\272\253\351\253\230(cm)", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\344\275\223\351\207\215(kg)", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\350\257\212\346\226\255\346\217\220\347\244\272", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\346\227\245\346\234\237", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\344\275\223\351\207\215(kg)", nullptr));
        btnDetailSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\350\257\212\346\226\255\344\277\241\346\201\257", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\350\272\253\351\253\230(cm)", nullptr));
        dGender->setItemText(0, QCoreApplication::translate("MainWindow", "\347\224\267", nullptr));
        dGender->setItemText(1, QCoreApplication::translate("MainWindow", "\345\245\263", nullptr));

        btnDetailDelete->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "\346\200\247\345\210\253", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\345\207\272\347\224\237\346\227\245\346\234\237", nullptr));
        btnDetailBack->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\346\227\245\346\234\237", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\350\264\246\345\217\267", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "\345\257\206\347\240\201", nullptr));
        lblLoginMsg->setText(QString());
        btnLogin->setText(QCoreApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "\345\271\264", nullptr));
        btnBackFromArchive->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\346\243\200\346\237\245\346\227\245\346\234\237\346\237\245\346\211\276\357\274\232", nullptr));
        btnDeleteSelected->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\345\213\276\351\200\211\351\241\271", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215/\347\274\226\345\217\267\346\237\245\346\211\276\357\274\232", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "\346\234\210", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240", nullptr));
        btnSelectPatient->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\202\243\350\200\205", nullptr));
        btnViewHistory->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\345\216\206\345\217\262", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "\346\227\245", nullptr));
        btnSearchName->setText(QCoreApplication::translate("MainWindow", "\346\237\245\346\211\276", nullptr));
        btnShowAll->setText(QCoreApplication::translate("MainWindow", "\346\230\276\347\244\272\345\205\250\351\203\250", nullptr));
        btnSearchID->setText(QCoreApplication::translate("MainWindow", "\346\214\211\347\274\226\345\217\267\346\237\245\346\211\276", nullptr));
        btnSearchDate->setText(QCoreApplication::translate("MainWindow", "\346\214\211\346\243\200\346\237\245\346\227\245\346\234\237\346\237\245\346\211\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
