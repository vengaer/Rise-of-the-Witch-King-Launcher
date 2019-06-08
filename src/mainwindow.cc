#include "mainwindow.h"
#include "command.h"
#include "config.h"
#include "game_data.h"
#include "fsys.h"
#include "ui_mainwindow.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QResizeEvent>
#include <QSize>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent}, ui{new Ui::MainWindow}, launch_img_{"images/dol_guldur.jpg"}, upd_img_{"images/minas_tirith.jpg"} {
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
    game_hash[0] = '\0';
    
    ui->launch_img->setMinimumSize(1,1);
    ui->upd_img->setMinimumSize(1,1);

    config_file_ = botta_toml_ = edain_toml_ = rotwk_toml_ = QDir::currentPath();
    config_file_ += "/launcher.toml";
    botta_toml_ += "/toml/botta.toml";
    edain_toml_ += "/toml/edain.toml";
    rotwk_toml_ += "/toml/rotwk.toml";

    config_exists_ = setup_launcher();
    
    ui->dat_swap->setChecked(data_.swap_dat_file);

    update_gui_functionality();

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
        game_path_from_registry(data_.game_path);
        QMessageBox::information(this, tr("Welcome"), "Please go through the settings. The Launch and Update tabs will become available once you save your choices");
    }

    ui->kill_on_launch->setChecked(data_.kill_on_launch);

    game_path_ = data_.game_path;
    botta_path_ = data_.botta_path;
    mount_exe_ = data_.mount_exe;
    mount_image_ = data_.disc_image;

    if(data_.game_path[0])
        ui->game_path_chooser->setText(wrap_text(data_.game_path));

    if(config_exists_)
        setup_paths();

    md5sum(dat_file_location_.toLatin1().data(), &game_hash[0]);
}

void MainWindow::on_tabWidget_currentChanged(int index) {
    if(index < 2)
        resizeEvent(nullptr);
}

bool MainWindow::setup_launcher() {

    bool config_found = read_launcher_config(&data_, config_file_.toLatin1().data());

    if(config_found)    
        chdir(data_.game_path);

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
    update_all_configs();
}

void MainWindow::on_botta_upd_clicked() {
    update_single_config(botta);
}

void MainWindow::on_edain_upd_clicked() {
    update_single_config(edain);
}

void MainWindow::on_rotwk_upd_clicked() {
    update_single_config(rotwk);
}

void MainWindow::on_pref_save_clicked() {
    QString exe = game_path_ + "/" + GAME_EXE;
    
    if(!file_exists(exe.toLatin1().data())) {
        QMessageBox::information(this, tr("Invalid Game Path"), "Could not locate lotrbfme2ep1.exe at specified path");
        return;
    }
    
    if(ui->botta_installed->isChecked()) {
        exe = botta_path_ + "/" + BOTTA_LNK;
        if(!file_exists(exe.toLatin1().data())) {
            QMessageBox::information(this, tr("Invalid BotTA Path"), "Could not locate the BotTa shortcut at specified path");
            return;
        }
    }

    data_.automatic_mount = ui->auto_mount->isChecked();
    if(data_.automatic_mount) {
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
    data_.kill_on_launch = ui->kill_on_launch->isChecked();

    data_.edain_available = ui->edain_installed->isChecked();

    update_gui_functionality();
    
    write_launcher_config(&data_, config_file_.toLatin1().data());

    if(!config_exists_) {
        config_exists_ = true;

        ui->launch_img->setPixmap(launch_img_.scaled(INITIAL_IMSIZE.width(), INITIAL_IMSIZE.height()));
        ui->upd_img->setPixmap(upd_img_.scaled(INITIAL_IMSIZE.width(), INITIAL_IMSIZE.height()));

        
        ui->tabWidget->setTabEnabled(0, true);
        ui->tabWidget->setTabEnabled(1, true);
        
        chdir(data_.game_path);
        update_all_configs();
    }
    setup_paths();

}

void MainWindow::resizeEvent(QResizeEvent*) {
    if(ui->tabWidget->currentIndex() > 1)
        return;

    if(!active_img_) {
        active_img_ = ui->tabWidget->currentIndex() == 0 ? ui->launch_img : ui->upd_img;

        ui->launch_img->setPixmap(launch_img_.scaled(INITIAL_IMSIZE.width(), INITIAL_IMSIZE.height()));
        ui->upd_img->setPixmap(upd_img_.scaled(INITIAL_IMSIZE.width(), INITIAL_IMSIZE.height()));
        return;
    }
    active_img_ = ui->tabWidget->currentIndex() == 0 ? ui->launch_img : ui->upd_img;

    ui->launch_img->setPixmap(launch_img_.scaled(active_img_->width() - 1, active_img_->height() - 1));
    ui->upd_img->setPixmap(upd_img_.scaled(active_img_->width() - 1, active_img_->height() - 1));
}

void MainWindow::launch(configuration config) noexcept {
    bool mounting_successful = false, mounting_necessary = false;

    if(data_.kill_on_launch)
        hide();

    if(config == rotwk) {
        md5sum(dat_file_location_.toLatin1().data(), &game_hash[0]);
        bool swap_required = strcmp(&game_hash[0], NEW_DAT_MD5.toLatin1().data());

        set_active_configuration(rotwk_toml_.toLatin1().data(), swap_required);
    }  
    else {
        if(config == edain)
            set_active_configuration(edain_toml_.toLatin1().data(), data_.swap_dat_file);
        else
            set_active_configuration(botta_toml_.toLatin1().data(), data_.swap_dat_file);

        md5sum(dat_file_location_.toLatin1().data(), &game_hash[0]);
        mounting_necessary = strcmp(&game_hash[0], NEW_DAT_MD5.toLatin1().data());
    }

    if(mounting_necessary && data_.automatic_mount) {
        if(system(data_.mount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", data_.mount_cmd);
            mounting_successful = false;
        }
        else
            mounting_successful = true;
    }
    
    if(!mounting_necessary || mounting_successful) {
        QString launch_cmd;    

        if(config == botta) {
            launch_cmd = data_.botta_path;
            launch_cmd += "/" + BOTTA_LNK;
        }
        else {
            launch_cmd = data_.game_path;
            launch_cmd += "/" + GAME_EXE;
        }

        if(system(launch_cmd.toLatin1().data()) != 0)
            fprintf(stderr, "Failed to launch game.\n");

        while(game_running())
            sleep_for(SLEEP_TIME);

        if(mounting_successful) {
            if(system(data_.umount_cmd) != 0)
                fprintf(stderr, "'%s' returned an error\n", data_.umount_cmd);
        }
    }

    if(data_.kill_on_launch)
        QApplication::quit();
}

void MainWindow::update_single_config(configuration config) {
    QString display;
    QString const* toml;
    bool new_dat_enabled = strcmp(&game_hash[0], NEW_DAT_MD5.toLatin1().data()) == 0;
    bool invert_dat;

    switch(config) {
        case edain:
            display = "Updating Edain config file...";
            toml = &edain_toml_;
            invert_dat = new_dat_enabled;
            break;
        case botta:
            display = "Updating BotTA config file...";
            toml = &botta_toml_;
            invert_dat = new_dat_enabled;
            break;
        case rotwk:
            display = "Updating RotWK config file...";
            toml = &rotwk_toml_;
            invert_dat = !new_dat_enabled;
            break;
        default:
            QMessageBox::information(this, tr("Error"), "Invalid option");
            return;
            break;
    }

    int tasks_running = 1;
    #pragma omp parallel num_threads(2)
    {
        #pragma omp master
        {
            prepare_progress();
            #pragma omp task
            {
                update_config_file(toml->toLatin1().data(), invert_dat);

                #pragma omp atomic
                --tasks_running;
            }

            int total = 1000;
            QProgressDialog dialog(display, "Cancel", 0, total, this);
            dialog.setWindowModality(Qt::WindowModal);
            dialog.setCancelButton(nullptr);

            dialog.show();
            int progress;

            while(tasks_running) {
                progress = static_cast<int>(track_progress() * total);
                QCoreApplication::processEvents();
                dialog.setValue(progress);
            }
            dialog.setValue(total);
            dialog.close();
            
            reset_progress();
        }
    }
}

void MainWindow::update_all_configs() {
    int tasks_running = 1;
    bool invert_dat = strcmp(&game_hash[0], NEW_DAT_MD5.toLatin1().data()) == 0;

    #pragma omp parallel num_threads(4)
    {
        #pragma omp master
        {
            prepare_progress();

            #pragma omp task if(data_.edain_available)
            {
                #pragma omp atomic
                ++tasks_running;

                update_config_file(edain_toml_.toLatin1().data(), invert_dat);
                
                #pragma omp atomic
                --tasks_running;
            }
            #pragma omp task if(data_.botta_available)
            {
                #pragma omp atomic
                ++tasks_running;
                
                update_config_file(botta_toml_.toLatin1().data(), invert_dat);

                #pragma omp atomic
                --tasks_running;
            }   
            #pragma omp task
            {
                update_config_file(rotwk_toml_.toLatin1().data(), !invert_dat);
                
                #pragma omp atomic
                --tasks_running
            }

            int total = 1000;
            QProgressDialog dialog("Updating config files...", "Cancel", 0, total, this);
            dialog.setWindowModality(Qt::WindowModal);
            dialog.setCancelButton(nullptr);

            dialog.show();
            int progress;

            while(tasks_running) {
                progress = static_cast<int>(track_progress() * total);
                QCoreApplication::processEvents();
                dialog.setValue(progress);
            }
            dialog.setValue(total);
            dialog.close();
            
            reset_progress();
        }
    }

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

void MainWindow::setup_paths() {
    dat_file_location_ = data_.game_path;
    dat_file_location_ += "/game.dat";
}

void MainWindow::update_gui_functionality() {
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
}

QString const MainWindow::NEW_DAT_MD5{NEW_DAT_CSUM};
QString const MainWindow::GAME_EXE{"lotrbfme2ep1.exe"};
QString const MainWindow::BOTTA_LNK{"BotTa.lnk"};
