#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#pragma once
#include "game_data.h"
#include <array>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>

namespace Ui {
    class MainWindow;
}

void gui_error_diag(char const* info);

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
    void on_tabWidget_currentChanged(int index);
    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;
    launcher_data data_{};
    std::array<char, 64> game_hash{};
    QString dat_file_location_{};
    bool config_exists_{};
    QString botta_toml_{}, edain_toml_{}, rotwk_toml_{};
    QString game_path_{}, botta_path_{}, mount_exe_{}, mount_image_{};
    QString config_file_{};
    QPixmap launch_img_, upd_img_;
    QLabel* active_img_{nullptr};

    static int constexpr WRAP_SIZE{24};
    static QSize constexpr INITIAL_IMSIZE{400, 220};
    static QString const GAME_EXE;
    static QString const BOTTA_LNK;
    static QString const WINDOW_TITLE;
    static std::array<QString, 5> const ROTWK_VERSIONS;

    void init();
    bool setup_launcher();
    void launch(configuration config);
    void setup_paths();
    void update_gui_functionality();

    void update_single_config(configuration config);
    void update_all_configs();

    static QString wrap_text(QString const& text);
    static void monitor_progress(QWidget* parent);
};

#endif // MAINWINDOW_H
