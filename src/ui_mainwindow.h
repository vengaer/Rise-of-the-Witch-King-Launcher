/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *launch_tab;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_15;
    QPushButton *launch_quit;
    QLabel *label_8;
    QPushButton *botta_launch;
    QPushButton *edain_launch;
    QPushButton *rotwk_launch;
    QLabel *launch_img;
    QWidget *update_tab;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *cfg_quit;
    QPushButton *all_upd;
    QPushButton *botta_upd;
    QPushButton *edain_upd;
    QPushButton *rotwk_upd;
    QLabel *upd_img;
    QWidget *pref_tab;
    QGridLayout *gridLayout_5;
    QScrollArea *pref_scroll;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_4;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_10;
    QCheckBox *auto_mount;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_13;
    QPushButton *mount_exec;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_7;
    QPushButton *mount_image;
    QHBoxLayout *horizontalLayout_27;
    QLabel *label_22;
    QLineEdit *mount_opt;
    QHBoxLayout *horizontalLayout_26;
    QLabel *label_21;
    QLineEdit *umount_opt;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_6;
    QCheckBox *imspec_umount;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_8;
    QGridLayout *gridLayout_10;
    QLabel *label_5;
    QCheckBox *botta_installed;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_3;
    QPushButton *botta_path_chooser;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_4;
    QCheckBox *edain_installed;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label;
    QCheckBox *dat_swap;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_2;
    QPushButton *game_path_chooser;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_9;
    QComboBox *default_state;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_11;
    QCheckBox *kill_on_launch;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_12;
    QCheckBox *show_console;
    QGridLayout *gridLayout_8;
    QPushButton *pref_quit;
    QPushButton *pref_save;
    QLabel *label_23;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(443, 346);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setTabPosition(QTabWidget::North);
        launch_tab = new QWidget();
        launch_tab->setObjectName(QString::fromUtf8("launch_tab"));
        gridLayout = new QGridLayout(launch_tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 0, -1, -1);
        launch_quit = new QPushButton(launch_tab);
        launch_quit->setObjectName(QString::fromUtf8("launch_quit"));

        horizontalLayout_15->addWidget(launch_quit);

        label_8 = new QLabel(launch_tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_15->addWidget(label_8);

        botta_launch = new QPushButton(launch_tab);
        botta_launch->setObjectName(QString::fromUtf8("botta_launch"));

        horizontalLayout_15->addWidget(botta_launch);

        edain_launch = new QPushButton(launch_tab);
        edain_launch->setObjectName(QString::fromUtf8("edain_launch"));

        horizontalLayout_15->addWidget(edain_launch);

        rotwk_launch = new QPushButton(launch_tab);
        rotwk_launch->setObjectName(QString::fromUtf8("rotwk_launch"));

        horizontalLayout_15->addWidget(rotwk_launch);


        gridLayout->addLayout(horizontalLayout_15, 1, 0, 1, 1);

        launch_img = new QLabel(launch_tab);
        launch_img->setObjectName(QString::fromUtf8("launch_img"));
        launch_img->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(launch_img->sizePolicy().hasHeightForWidth());
        launch_img->setSizePolicy(sizePolicy1);
        launch_img->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(launch_img, 0, 0, 1, 1);

        tabWidget->addTab(launch_tab, QString());
        update_tab = new QWidget();
        update_tab->setObjectName(QString::fromUtf8("update_tab"));
        gridLayout_3 = new QGridLayout(update_tab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        horizontalLayout_14->setContentsMargins(-1, 0, -1, -1);
        cfg_quit = new QPushButton(update_tab);
        cfg_quit->setObjectName(QString::fromUtf8("cfg_quit"));

        horizontalLayout_14->addWidget(cfg_quit);

        all_upd = new QPushButton(update_tab);
        all_upd->setObjectName(QString::fromUtf8("all_upd"));

        horizontalLayout_14->addWidget(all_upd);

        botta_upd = new QPushButton(update_tab);
        botta_upd->setObjectName(QString::fromUtf8("botta_upd"));

        horizontalLayout_14->addWidget(botta_upd);

        edain_upd = new QPushButton(update_tab);
        edain_upd->setObjectName(QString::fromUtf8("edain_upd"));

        horizontalLayout_14->addWidget(edain_upd);

        rotwk_upd = new QPushButton(update_tab);
        rotwk_upd->setObjectName(QString::fromUtf8("rotwk_upd"));

        horizontalLayout_14->addWidget(rotwk_upd);


        gridLayout_3->addLayout(horizontalLayout_14, 1, 0, 1, 1);

        upd_img = new QLabel(update_tab);
        upd_img->setObjectName(QString::fromUtf8("upd_img"));
        sizePolicy1.setHeightForWidth(upd_img->sizePolicy().hasHeightForWidth());
        upd_img->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(upd_img, 0, 0, 1, 1);

        tabWidget->addTab(update_tab, QString());
        pref_tab = new QWidget();
        pref_tab->setObjectName(QString::fromUtf8("pref_tab"));
        gridLayout_5 = new QGridLayout(pref_tab);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        pref_scroll = new QScrollArea(pref_tab);
        pref_scroll->setObjectName(QString::fromUtf8("pref_scroll"));
        pref_scroll->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 382, 532));
        gridLayout_4 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        groupBox_4 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_7 = new QVBoxLayout(groupBox_4);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_17->addWidget(label_10);

        auto_mount = new QCheckBox(groupBox_4);
        auto_mount->setObjectName(QString::fromUtf8("auto_mount"));

        horizontalLayout_17->addWidget(auto_mount);


        verticalLayout_7->addLayout(horizontalLayout_17);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setSpacing(6);
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        label_13 = new QLabel(groupBox_4);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_18->addWidget(label_13);

        mount_exec = new QPushButton(groupBox_4);
        mount_exec->setObjectName(QString::fromUtf8("mount_exec"));

        horizontalLayout_18->addWidget(mount_exec);


        verticalLayout_7->addLayout(horizontalLayout_18);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_13->addWidget(label_7);

        mount_image = new QPushButton(groupBox_4);
        mount_image->setObjectName(QString::fromUtf8("mount_image"));

        horizontalLayout_13->addWidget(mount_image);


        verticalLayout_7->addLayout(horizontalLayout_13);

        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setSpacing(6);
        horizontalLayout_27->setObjectName(QString::fromUtf8("horizontalLayout_27"));
        horizontalLayout_27->setContentsMargins(0, -1, 0, -1);
        label_22 = new QLabel(groupBox_4);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_27->addWidget(label_22);

        mount_opt = new QLineEdit(groupBox_4);
        mount_opt->setObjectName(QString::fromUtf8("mount_opt"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(mount_opt->sizePolicy().hasHeightForWidth());
        mount_opt->setSizePolicy(sizePolicy2);
        mount_opt->setLayoutDirection(Qt::RightToLeft);
        mount_opt->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        mount_opt->setClearButtonEnabled(false);

        horizontalLayout_27->addWidget(mount_opt);


        verticalLayout_7->addLayout(horizontalLayout_27);

        horizontalLayout_26 = new QHBoxLayout();
        horizontalLayout_26->setSpacing(6);
        horizontalLayout_26->setObjectName(QString::fromUtf8("horizontalLayout_26"));
        horizontalLayout_26->setContentsMargins(0, -1, 0, -1);
        label_21 = new QLabel(groupBox_4);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout_26->addWidget(label_21);

        umount_opt = new QLineEdit(groupBox_4);
        umount_opt->setObjectName(QString::fromUtf8("umount_opt"));
        sizePolicy2.setHeightForWidth(umount_opt->sizePolicy().hasHeightForWidth());
        umount_opt->setSizePolicy(sizePolicy2);
        umount_opt->setLayoutDirection(Qt::RightToLeft);
        umount_opt->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        umount_opt->setClearButtonEnabled(false);

        horizontalLayout_26->addWidget(umount_opt);


        verticalLayout_7->addLayout(horizontalLayout_26);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_12->addWidget(label_6);

        imspec_umount = new QCheckBox(groupBox_4);
        imspec_umount->setObjectName(QString::fromUtf8("imspec_umount"));

        horizontalLayout_12->addWidget(imspec_umount);


        verticalLayout_7->addLayout(horizontalLayout_12);


        gridLayout_4->addWidget(groupBox_4, 7, 0, 1, 2);

        groupBox_3 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_8 = new QVBoxLayout(groupBox_3);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        gridLayout_10 = new QGridLayout();
        gridLayout_10->setSpacing(6);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_10->addWidget(label_5, 0, 0, 1, 1);

        botta_installed = new QCheckBox(groupBox_3);
        botta_installed->setObjectName(QString::fromUtf8("botta_installed"));

        gridLayout_10->addWidget(botta_installed, 0, 1, 1, 1);


        verticalLayout_8->addLayout(gridLayout_10);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_9->addWidget(label_3);

        botta_path_chooser = new QPushButton(groupBox_3);
        botta_path_chooser->setObjectName(QString::fromUtf8("botta_path_chooser"));

        horizontalLayout_9->addWidget(botta_path_chooser);


        verticalLayout_8->addLayout(horizontalLayout_9);


        gridLayout_4->addWidget(groupBox_3, 5, 0, 1, 2);

        groupBox_2 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_9 = new QVBoxLayout(groupBox_2);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_10->addWidget(label_4);

        edain_installed = new QCheckBox(groupBox_2);
        edain_installed->setObjectName(QString::fromUtf8("edain_installed"));

        horizontalLayout_10->addWidget(edain_installed);


        verticalLayout_9->addLayout(horizontalLayout_10);


        gridLayout_4->addWidget(groupBox_2, 3, 0, 1, 2);

        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_7 = new QGridLayout(groupBox);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_8->addWidget(label);

        dat_swap = new QCheckBox(groupBox);
        dat_swap->setObjectName(QString::fromUtf8("dat_swap"));

        horizontalLayout_8->addWidget(dat_swap);


        gridLayout_7->addLayout(horizontalLayout_8, 2, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_7->addWidget(label_2);

        game_path_chooser = new QPushButton(groupBox);
        game_path_chooser->setObjectName(QString::fromUtf8("game_path_chooser"));

        horizontalLayout_7->addWidget(game_path_chooser);


        gridLayout_7->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_11->addWidget(label_9);

        default_state = new QComboBox(groupBox);
        default_state->setObjectName(QString::fromUtf8("default_state"));

        horizontalLayout_11->addWidget(default_state);


        gridLayout_7->addLayout(horizontalLayout_11, 1, 0, 1, 1);


        gridLayout_4->addWidget(groupBox, 2, 0, 1, 2);

        groupBox_5 = new QGroupBox(scrollAreaWidgetContents);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_6 = new QGridLayout(groupBox_5);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_11 = new QLabel(groupBox_5);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_16->addWidget(label_11);

        kill_on_launch = new QCheckBox(groupBox_5);
        kill_on_launch->setObjectName(QString::fromUtf8("kill_on_launch"));

        horizontalLayout_16->addWidget(kill_on_launch);


        gridLayout_6->addLayout(horizontalLayout_16, 0, 0, 1, 1);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setSpacing(6);
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_19->addWidget(label_12);

        show_console = new QCheckBox(groupBox_5);
        show_console->setObjectName(QString::fromUtf8("show_console"));

        horizontalLayout_19->addWidget(show_console);


        gridLayout_6->addLayout(horizontalLayout_19, 1, 0, 1, 1);


        gridLayout_4->addWidget(groupBox_5, 8, 0, 1, 2);

        pref_scroll->setWidget(scrollAreaWidgetContents);

        gridLayout_5->addWidget(pref_scroll, 0, 0, 2, 2);

        gridLayout_8 = new QGridLayout();
        gridLayout_8->setSpacing(6);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setHorizontalSpacing(12);
        gridLayout_8->setContentsMargins(0, -1, 0, 1);
        pref_quit = new QPushButton(pref_tab);
        pref_quit->setObjectName(QString::fromUtf8("pref_quit"));
        pref_quit->setMinimumSize(QSize(75, 23));
        pref_quit->setMaximumSize(QSize(75, 23));

        gridLayout_8->addWidget(pref_quit, 0, 0, 1, 1);

        pref_save = new QPushButton(pref_tab);
        pref_save->setObjectName(QString::fromUtf8("pref_save"));
        pref_save->setMaximumSize(QSize(75, 23));

        gridLayout_8->addWidget(pref_save, 0, 2, 1, 1);

        label_23 = new QLabel(pref_tab);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setMinimumSize(QSize(225, 23));
        label_23->setMaximumSize(QSize(16777215, 23));

        gridLayout_8->addWidget(label_23, 0, 1, 1, 1);


        gridLayout_5->addLayout(gridLayout_8, 3, 0, 1, 2);

        tabWidget->addTab(pref_tab, QString());

        gridLayout_2->addWidget(tabWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        launch_quit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        label_8->setText(QString());
        botta_launch->setText(QApplication::translate("MainWindow", "BotTA", nullptr));
        edain_launch->setText(QApplication::translate("MainWindow", "Edain", nullptr));
        rotwk_launch->setText(QApplication::translate("MainWindow", "RotWK", nullptr));
        launch_img->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(launch_tab), QApplication::translate("MainWindow", "Launch", nullptr));
        cfg_quit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        all_upd->setText(QApplication::translate("MainWindow", "All", nullptr));
        botta_upd->setText(QApplication::translate("MainWindow", "BotTA", nullptr));
        edain_upd->setText(QApplication::translate("MainWindow", "Edain", nullptr));
        rotwk_upd->setText(QApplication::translate("MainWindow", "RotWK", nullptr));
        upd_img->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(update_tab), QApplication::translate("MainWindow", "Update Configs", nullptr));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Mounting", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Automatic", nullptr));
#ifndef QT_NO_TOOLTIP
        auto_mount->setToolTip(QApplication::translate("MainWindow", "Enables automatic mounting of disc image", nullptr));
#endif // QT_NO_TOOLTIP
        auto_mount->setText(QString());
        label_13->setText(QApplication::translate("MainWindow", "Mounting Executable", nullptr));
#ifndef QT_NO_TOOLTIP
        mount_exec->setToolTip(QApplication::translate("MainWindow", "Path to mounting executable (e.g. DTCommandLine.exe, batchmnt.exe or similar)", nullptr));
#endif // QT_NO_TOOLTIP
        mount_exec->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Image", nullptr));
#ifndef QT_NO_TOOLTIP
        mount_image->setToolTip(QApplication::translate("MainWindow", "Path to disc image to mount", nullptr));
#endif // QT_NO_TOOLTIP
        mount_image->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label_22->setText(QApplication::translate("MainWindow", "Mounting Options", nullptr));
#ifndef QT_NO_TOOLTIP
        mount_opt->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Command line options for mounting with chosen executable. Leave empty if none are required.<br>(e.g. -m for DTCommandLine.exe, leave blank for batchmnt.exe)</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_21->setText(QApplication::translate("MainWindow", "Unmounting Options", nullptr));
#ifndef QT_NO_TOOLTIP
        umount_opt->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Command line options for unmounting with chosen executable. Leave empty if none are required. <br>(e.g. -u or -U for DTCommandLine.exe, /unmount or /unmountall for batchmnt.exe)</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("MainWindow", "Specify Image When Unmounting", nullptr));
#ifndef QT_NO_TOOLTIP
        imspec_umount->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Whether the disc image should be specified when unmounting. Generally, this should be checked for any unmount command that does not unmount all mounted images.<br>(e.g. turn on if using -u with DTCommandLine.exe or /unmount with batchmnt.exe, leave off if using -U with DTCommandLine.exe or /unmountall with batchmnt.exe)</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        imspec_umount->setText(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "BotTA", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Installed", nullptr));
#ifndef QT_NO_TOOLTIP
        botta_installed->setToolTip(QApplication::translate("MainWindow", "Enables BotTA-related actions. Works only if the BotTA mod is installed and the correct path to its install directory is specified below", nullptr));
#endif // QT_NO_TOOLTIP
        botta_installed->setText(QString());
        label_3->setText(QApplication::translate("MainWindow", "BotTA Path", nullptr));
#ifndef QT_NO_TOOLTIP
        botta_path_chooser->setToolTip(QApplication::translate("MainWindow", "Path to BotTA directory (directory containing BotTa.lnk)", nullptr));
#endif // QT_NO_TOOLTIP
        botta_path_chooser->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Edain", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Installed", nullptr));
#ifndef QT_NO_TOOLTIP
        edain_installed->setToolTip(QApplication::translate("MainWindow", "Enables Edain-related actions. Works only if the Edain mod is installed", nullptr));
#endif // QT_NO_TOOLTIP
        edain_installed->setText(QString());
        groupBox->setTitle(QApplication::translate("MainWindow", "Base Game", nullptr));
        label->setText(QApplication::translate("MainWindow", "Swap .dat File", nullptr));
#ifndef QT_NO_TOOLTIP
        dat_swap->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Specifies whether the launcher should swap game.dat files. Turning this off means the 2.02 .dat file will always be kept active, making it possible to play 2.01 and any mods requiring it without a disc.<br>(Recommended: On)</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        dat_swap->setText(QString());
        label_2->setText(QApplication::translate("MainWindow", "Game Path", nullptr));
#ifndef QT_NO_TOOLTIP
        game_path_chooser->setToolTip(QApplication::translate("MainWindow", "Path to game directory (directory containing lotrbfme2ep1.exe)", nullptr));
#endif // QT_NO_TOOLTIP
        game_path_chooser->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Default State", nullptr));
#ifndef QT_NO_TOOLTIP
        default_state->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>State in which the game is left after launch. Options are:<br>RotWK - patch 2.02 (Recommended for maximum compatibility)<br>Edain - patch 2.01 with Edain enabled<br>BotTA - patch 2.01<br> Last Launched - Whatever version was launched last</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Launcher", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "Terminate On Launch", nullptr));
#ifndef QT_NO_TOOLTIP
        kill_on_launch->setToolTip(QApplication::translate("MainWindow", "Terminate launcher after starting the game", nullptr));
#endif // QT_NO_TOOLTIP
        kill_on_launch->setText(QString());
        label_12->setText(QApplication::translate("MainWindow", "Show Console", nullptr));
#ifndef QT_NO_TOOLTIP
        show_console->setToolTip(QApplication::translate("MainWindow", "Show console", nullptr));
#endif // QT_NO_TOOLTIP
        show_console->setText(QString());
        pref_quit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        pref_save->setText(QApplication::translate("MainWindow", "Save", nullptr));
        label_23->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(pref_tab), QApplication::translate("MainWindow", "Preferences", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
