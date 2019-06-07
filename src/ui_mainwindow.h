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
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_15;
    QPushButton *launch_quit;
    QLabel *label_8;
    QPushButton *botta_launch;
    QPushButton *edain_launch;
    QPushButton *rotwk_launch;
    QWidget *update_tab;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_14;
    QPushButton *cfg_quit;
    QPushButton *all_upd;
    QPushButton *botta_upd;
    QPushButton *edain_upd;
    QPushButton *rotwk_upd;
    QWidget *pref_tab;
    QGridLayout *gridLayout_5;
    QScrollArea *pref_scroll;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_4;
    QCheckBox *edain_installed;
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
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_2;
    QPushButton *game_path_chooser;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label;
    QCheckBox *dat_swap;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_8;
    QGridLayout *gridLayout_10;
    QLabel *label_5;
    QCheckBox *botta_installed;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_3;
    QPushButton *botta_path_chooser;
    QGridLayout *gridLayout_8;
    QPushButton *pref_save;
    QPushButton *pref_quit;
    QLabel *label_23;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(443, 339);
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
        gridLayout_4 = new QGridLayout(launch_tab);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 220, -1, -1);
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


        gridLayout_4->addLayout(horizontalLayout_15, 0, 0, 1, 1);

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
        horizontalLayout_14->setContentsMargins(-1, 220, -1, -1);
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


        gridLayout_3->addLayout(horizontalLayout_14, 1, 1, 1, 1);

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
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 382, 429));
        gridLayout_6 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
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


        gridLayout_6->addWidget(groupBox_2, 1, 0, 1, 1);

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
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mount_opt->sizePolicy().hasHeightForWidth());
        mount_opt->setSizePolicy(sizePolicy1);
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
        sizePolicy1.setHeightForWidth(umount_opt->sizePolicy().hasHeightForWidth());
        umount_opt->setSizePolicy(sizePolicy1);
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


        gridLayout_6->addWidget(groupBox_4, 3, 0, 1, 1);

        groupBox = new QGroupBox(scrollAreaWidgetContents);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_10 = new QVBoxLayout(groupBox);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_7->addWidget(label_2);

        game_path_chooser = new QPushButton(groupBox);
        game_path_chooser->setObjectName(QString::fromUtf8("game_path_chooser"));

        horizontalLayout_7->addWidget(game_path_chooser);


        verticalLayout_10->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_8->addWidget(label);

        dat_swap = new QCheckBox(groupBox);
        dat_swap->setObjectName(QString::fromUtf8("dat_swap"));

        horizontalLayout_8->addWidget(dat_swap);


        verticalLayout_10->addLayout(horizontalLayout_8);


        gridLayout_6->addWidget(groupBox, 0, 0, 1, 1);

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


        gridLayout_6->addWidget(groupBox_3, 2, 0, 1, 1);

        pref_scroll->setWidget(scrollAreaWidgetContents);

        gridLayout_5->addWidget(pref_scroll, 0, 0, 2, 2);

        gridLayout_8 = new QGridLayout();
        gridLayout_8->setSpacing(6);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setHorizontalSpacing(12);
        gridLayout_8->setContentsMargins(0, -1, 0, 1);
        pref_save = new QPushButton(pref_tab);
        pref_save->setObjectName(QString::fromUtf8("pref_save"));

        gridLayout_8->addWidget(pref_save, 0, 2, 1, 1);

        pref_quit = new QPushButton(pref_tab);
        pref_quit->setObjectName(QString::fromUtf8("pref_quit"));
        pref_quit->setMinimumSize(QSize(75, 23));
        pref_quit->setMaximumSize(QSize(75, 23));

        gridLayout_8->addWidget(pref_quit, 0, 0, 1, 1);

        label_23 = new QLabel(pref_tab);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setMinimumSize(QSize(225, 23));

        gridLayout_8->addWidget(label_23, 0, 1, 1, 1);


        gridLayout_5->addLayout(gridLayout_8, 2, 0, 1, 2);

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

        tabWidget->setCurrentIndex(2);


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
        tabWidget->setTabText(tabWidget->indexOf(launch_tab), QApplication::translate("MainWindow", "Launch", nullptr));
        cfg_quit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        all_upd->setText(QApplication::translate("MainWindow", "All", nullptr));
        botta_upd->setText(QApplication::translate("MainWindow", "BotTA", nullptr));
        edain_upd->setText(QApplication::translate("MainWindow", "Edain", nullptr));
        rotwk_upd->setText(QApplication::translate("MainWindow", "RotWK", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(update_tab), QApplication::translate("MainWindow", "Update Configs", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Edain", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Installed", nullptr));
        edain_installed->setText(QString());
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Mounting", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Automatic", nullptr));
        auto_mount->setText(QString());
        label_13->setText(QApplication::translate("MainWindow", "Mounting Executable", nullptr));
        mount_exec->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Image", nullptr));
        mount_image->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label_22->setText(QApplication::translate("MainWindow", "Mounting Options", nullptr));
        label_21->setText(QApplication::translate("MainWindow", "Unmounting Options", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Specify Image When Unmounting", nullptr));
        imspec_umount->setText(QString());
        groupBox->setTitle(QApplication::translate("MainWindow", "Base Game", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Game Path", nullptr));
        game_path_chooser->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        label->setText(QApplication::translate("MainWindow", "Swap .dat File", nullptr));
        dat_swap->setText(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "BotTA", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Installed", nullptr));
        botta_installed->setText(QString());
        label_3->setText(QApplication::translate("MainWindow", "BotTA Path", nullptr));
        botta_path_chooser->setText(QApplication::translate("MainWindow", "Choose...", nullptr));
        pref_save->setText(QApplication::translate("MainWindow", "Save", nullptr));
        pref_quit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        label_23->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(pref_tab), QApplication::translate("MainWindow", "Preferences", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
