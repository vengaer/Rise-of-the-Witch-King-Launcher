#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef GTK_GUI
#pragma once
#include "game_data.h"
#include <array>
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_game_path_chooser_clicked();
    void on_botta_path_chooser_clicked();
    void on_mount_exec_clicked();
    void on_mount_image_clicked();
    void on_auto_mount_stateChanged(int arg1);
    void on_botta_installed_stateChanged(int arg1);
    void on_pref_quit_clicked();
    void on_cfg_quit_clicked();
    void on_launch_quit_clicked();
    void on_botta_launch_clicked();
    void on_edain_launch_clicked();
    void on_rotwk_launch_clicked();
    void on_all_upd_clicked();
    void on_botta_upd_clicked();
    void on_edain_upd_clicked();
    void on_rotwk_upd_clicked();
    void on_pref_save_clicked();

private:
    Ui::MainWindow *ui;
    launcher_data data_{};
    std::array<char, 64> game_md5{};
    bool mounting_necessary_{true};
    bool config_exists_{};
    QString botta_toml{}, edain_toml{}, rotwk_toml{};
    QString game_path_{}, botta_path_{}, mount_exe_{}, mount_image_{};
    QString config_file_{};

    static int constexpr WRAP_SIZE{24u};
    static QString const NEW_DAT_MD5;
    static QString const GAME_EXE;

    void init();
    bool setup_launcher();
    void launch(configuration config) noexcept;
    void setup_toml_paths();

    static QString wrap_text(QString const& text);
};

#endif
#endif // MAINWINDOW_H
