#ifndef GTK_GUI
#include "mainwindow.h"
#include "command.h"
#include "config.h"
#include "fsys.h"
#include "ui_mainwindow.h"
#include <memory>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_game_path_chooser_clicked() {
    QString start_dir = game_path_ == "" ? "C://" : game_path_;
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory"), start_dir);
    if(path != "") {
        game_path_ = path;
        ui->game_path_chooser->setText(wrap_text(game_path_));
    }
}

void MainWindow::on_botta_path_chooser_clicked() {
    QString start_dir = botta_path_ == "" ? "C://" : botta_path_;
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory"), start_dir);
    if(path != "") {
        botta_path_ = path;
        ui->botta_path_chooser->setText(wrap_text(botta_path_));
    }
}

void MainWindow::on_mount_exec_clicked() {
    QString start_path = mount_exe_ == "" ? "C://Program Files" : mount_exe_;
    QString exe = QFileDialog::getOpenFileName(this, tr("Choose file"), start_path, "Executables (*.exe))");
    if(exe != "") {
        mount_exe_ = exe;
        ui->mount_exec->setText(wrap_text(mount_exe_));
    }
}

void MainWindow::on_mount_image_clicked() {
    QString start_path = mount_image_ == "" ? "C://" : mount_image_;
    QString img = QFileDialog::getOpenFileName(this, tr("Choose image"), start_path, "All files (*.*);; Iso files (*.iso);; Mds files (*.mds)");
    if(img != "") {
        mount_image_ = img; 
        ui->mount_image->setText(wrap_text(mount_image_));
    }
}

void MainWindow::on_auto_mount_stateChanged(int arg1) {
    ui->mount_exec->setEnabled(arg1);
    ui->mount_image->setEnabled(arg1);
    ui->mount_opt->setEnabled(arg1);
    ui->umount_opt->setEnabled(arg1);
    ui->imspec_umount->setEnabled(arg1);
}

void MainWindow::on_botta_installed_stateChanged(int arg1) {
    ui->botta_path_chooser->setEnabled(arg1);
}

void MainWindow::init() {
    game_md5[0] = '\0';
    config_file_ = QDir::currentPath() + "/launcher.toml";
    config_exists_ = setup_launcher();
    
    ui->dat_swap->setChecked(data_.swap_dat_file);

    ui->botta_path_chooser->setEnabled(data_.botta_available);
    ui->botta_launch->setEnabled(data_.botta_available);
    ui->botta_upd->setEnabled(data_.botta_available);

    ui->edain_launch->setEnabled(data_.edain_available);
    ui->edain_upd->setEnabled(data_.edain_available);

    ui->mount_exec->setEnabled(data_.automatic_mount);
    ui->mount_image->setEnabled(data_.automatic_mount);
    ui->mount_opt->setEnabled(data_.automatic_mount);
    ui->umount_opt->setEnabled(data_.automatic_mount);
    ui->imspec_umount->setEnabled(data_.automatic_mount);

    if(config_exists_) {
        ui->edain_installed->setChecked(data_.edain_available);
        if(data_.botta_available) {
            ui->botta_installed->setChecked(data_.botta_available);
            ui->botta_path_chooser->setText(wrap_text(data_.botta_path));
        }
        if(data_.automatic_mount) {
            ui->auto_mount->setChecked(data_.automatic_mount);
            ui->mount_exec->setText(wrap_text(data_.mount_exe));
            ui->mount_image->setText(wrap_text(data_.disc_image));
            ui->mount_opt->setText(wrap_text(data_.mount_flags));
            ui->umount_opt->setText(wrap_text(data_.umount_flags));
            ui->imspec_umount->setChecked(data_.umount_imspec);
        }
        ui->tabWidget->setCurrentWidget(ui->launch_tab);
    }
    else {
        ui->tabWidget->setCurrentWidget(ui->pref_tab);
        ui->tabWidget->setTabEnabled(0, false);
        ui->tabWidget->setTabEnabled(1, false);
        ui->dat_swap->setChecked(true);
    }
    game_path_from_registry(data_.game_path);

    game_path_ = data_.game_path;
    botta_path_ = data_.botta_path;
    mount_exe_ = data_.mount_exe;
    mount_image_ = data_.disc_image;

    if(data_.game_path[0])
        ui->game_path_chooser->setText(wrap_text(data_.game_path));

    if(config_exists_)
        setup_toml_paths();
}

bool MainWindow::setup_launcher() {
    bool config_found;
    
    #pragma omp parallel num_threads(2)
    {
    #pragma omp single
    {
    #pragma omp task if(!game_md5[0])
    {
        md5sum("game.dat", std::addressof(game_md5[0]));
        mounting_necessary_ = strcmp(std::addressof(game_md5[0]), NEW_DAT_MD5.toLatin1().data());
    }
        config_found = read_launcher_config(&data_, config_file_.toLatin1().data());
        if(config_found)    
            cd_to_game_path(&data_);
    }
    }
    return config_found;
}

void MainWindow::on_pref_quit_clicked() {
    QApplication::quit();
}

void MainWindow::on_cfg_quit_clicked() {
    QApplication::quit();
}

void MainWindow::on_launch_quit_clicked() {
    QApplication::quit();
}

void MainWindow::on_botta_launch_clicked() {
    launch(botta);
}

void MainWindow::on_edain_launch_clicked() {
    launch(edain);
}

void MainWindow::on_rotwk_launch_clicked() {
    launch(rotwk);
}

void MainWindow::on_all_upd_clicked() {
    #pragma omp parallel num_threads(3)
    {
    #pragma omp single
    {
    #pragma omp task if(data_.edain_available)
    {
        update_config_file(edain_toml.toLatin1().data());
    }
    #pragma omp task if(data_.botta_available)
    {
        update_config_file(botta_toml.toLatin1().data());
    }   
        update_config_file(rotwk_toml.toLatin1().data());
    }
    }
}

void MainWindow::on_botta_upd_clicked() {
    update_config_file(botta_toml.toLatin1().data());
}

void MainWindow::on_edain_upd_clicked() {
    update_config_file(edain_toml.toLatin1().data());
}

void MainWindow::on_rotwk_upd_clicked() {
    update_config_file(rotwk_toml.toLatin1().data());
}

void MainWindow::on_pref_save_clicked() {
    QString exe = game_path_ + "/" + GAME_EXE;
    
    if(!file_exists(exe.toLatin1().data())) {
        QMessageBox::information(this, tr("Invalid Game Path"), "Could not locate lotrbfme2ep1.exe at specified path");
        return;
    }
    
    if(ui->botta_installed->isChecked()) {
        exe = botta_path_ + "/BotTa.lnk";
        if(!file_exists(exe.toLatin1().data())) {
            QMessageBox::information(this, tr("Invalid BotTA Path"), "Could not locate the BotTa shortcut at specified path");
            return;
        }
    }

    if(ui->auto_mount->isChecked()) {
        if(!file_exists(mount_exe_.toLatin1().data())) {
            QMessageBox::information(this, tr("File Does Not Exist"), "Could not locate specified mounting executable");
            return;
        }
        if(!file_exists(mount_image_.toLatin1().data())) {
            QMessageBox::information(this, tr("File Does Not Exist"), "Could not locate specified image file");
            return;
        }

        strcpy(data_.disc_image, mount_image_.toLatin1().data());
        strcpy(data_.mount_exe, mount_exe_.toLatin1().data());

        data_.automatic_mount = true;
        strcpy(data_.mount_flags, ui->mount_opt->text().toLatin1().data());
        strcpy(data_.umount_flags, ui->umount_opt->text().toLatin1().data());
        data_.umount_imspec = ui->imspec_umount->isChecked();
    
        construct_mount_command(data_.mount_cmd, data_.mount_exe, data_.mount_flags, data_.disc_image);
        construct_umount_command(data_.umount_cmd, data_.mount_exe, data_.umount_flags, data_.disc_image, data_.umount_imspec);
        
    }
    data_.botta_available = ui->botta_installed->isChecked();
    if(data_.botta_available)
        strcpy(data_.botta_path, botta_path_.toLatin1().data());

    strcpy(data_.game_path, game_path_.toLatin1().data());
    data_.swap_dat_file = ui->dat_swap->isChecked();
    data_.edain_available = ui->edain_installed->isChecked();
    
    write_launcher_config(&data_, config_file_.toLatin1().data());
    if(!config_exists_) {
        config_exists_ = true;
        ui->tabWidget->setTabEnabled(0, true);
        ui->tabWidget->setTabEnabled(1, true);
        
        cd_to_game_path(&data_);
        setup_toml_paths();
    }
}

void MainWindow::launch(configuration config) noexcept {
    bool mounting_successful;
    hide();
    // Hide gui
    if(config == rotwk) {
        set_active_configuration(rotwk_toml.toLatin1().data(), data_.swap_dat_file);
        mounting_necessary_ = false;
    }  
    else if(config == edain)
        set_active_configuration(edain_toml.toLatin1().data(), data_.swap_dat_file);
    else
        set_active_configuration(botta_toml.toLatin1().data(), data_.swap_dat_file);

    if(mounting_necessary_ && data_.automatic_mount) {
        if(system(data_.mount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", data_.mount_cmd);
            mounting_successful = false;
        }
        else
            mounting_successful = true;
    }
    
    if(!mounting_necessary_ || mounting_successful) {
        QString launch_cmd{data_.game_path};    
        launch_cmd += "/" + GAME_EXE;

        if(config == botta) {
            launch_cmd += " -mod ";
            launch_cmd += data_.botta_path;
        }
        if(system(launch_cmd.toLatin1().data()) != 0)
            fprintf(stderr, "Failed to launch game.\n");

        while(game_running())
            sleep_for(SLEEP_TIME);

        if(mounting_necessary_ && mounting_successful) {
            if(system(data_.umount_cmd) != 0)
                fprintf(stderr, "'%s' returned an error\n", data_.umount_cmd);
        }
    }

    QApplication::quit();
}

QString MainWindow::wrap_text(QString const& text) {
    QString wrapped;
    if(text.size() > WRAP_SIZE) {
        wrapped = text.mid(0, WRAP_SIZE);
        for(int i = 0; i < 3; i++)
            wrapped[WRAP_SIZE - 1 - i] = '.';
    }
    else
        wrapped = text;


    return wrapped;
}

void MainWindow::setup_toml_paths() {
    rotwk_toml = edain_toml = botta_toml = data_.game_path;
    botta_toml += "/toml/botta.toml";
    edain_toml += "/toml/edain.toml";
    rotwk_toml += "/toml/rotwk.toml";
}

QString const MainWindow::NEW_DAT_MD5{"fbff091f6139eb05dc012d834e3eeb74"};
QString const MainWindow::GAME_EXE{"lotrbfme2ep1.exe"};

#endif
