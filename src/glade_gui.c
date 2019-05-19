#include "glade_gui.h"
#include "command.h"
#include "config.h"
#include "fsys.h"
#include "game_data.h"
#include <stdbool.h>
#include <stdlib.h>

#define UNUSED(x) (void)(x)

    
static void gui_update_rotwk(GtkWidget* widget, gpointer data);
static void gui_update_edain(GtkWidget* widget, gpointer data);
static void gui_update_botta(GtkWidget* widget, gpointer data);
static void gui_launch_rotwk(GtkWidget* widget, gpointer data);
static void gui_launch_edain(GtkWidget* widget, gpointer data);
static void gui_launch_botta(GtkWidget* widget, gpointer data);
static void gui_quit(GtkWidget* widget, gpointer data);
static void gui_toggle_dat_swap(GtkWidget* widget, gpointer data);

static char const CONFIG_FILE[] = "rotwkl.toml";
static launcher_data ld;
static char launch_cmd[256];
static char rotwk_toml[128];
static char edain_toml[128];
static char botta_toml[128];
static bool swap_dat_file = true;

void gui_init(int* argc, char*** argv) {
    GtkBuilder* builder;
    GtkWidget* window;
    GError* error = NULL;
    GObject *quit, *dat_swap;
    GObject *rotwk_upd, *edain_upd, *botta_upd;
    GObject *rotwk_launch, *edain_launch, *botta_launch;

    read_launcher_config(&ld, CONFIG_FILE);
    //if(!read_launcher_config(&ld, CONFIG_FILE))
        //return;
    
    construct_from_rel_path(&ld, launch_cmd, "/lotrbfme2ep1.exe");
    construct_from_rel_path(&ld, rotwk_toml, "/toml/rotwk.toml");
    construct_from_rel_path(&ld, edain_toml, "/toml/edain.toml");
    construct_from_rel_path(&ld, botta_toml, "/toml/botta.toml");

    
    gtk_init(argc, argv);
    builder = gtk_builder_new();
    
    if(gtk_builder_add_from_file(builder, "launcher.glade", &error) == 0) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    rotwk_upd    = gtk_builder_get_object(builder, "rotwk_upd");
    edain_upd    = gtk_builder_get_object(builder, "edain_upd");
    botta_upd    = gtk_builder_get_object(builder, "botta_upd");
    rotwk_launch = gtk_builder_get_object(builder, "rotwk_launch");
    edain_launch = gtk_builder_get_object(builder, "edain_launch");
    botta_launch = gtk_builder_get_object(builder, "botta_launch");
    quit         = gtk_builder_get_object(builder, "quit");
    dat_swap     = gtk_builder_get_object(builder, "dat_swap");

    g_object_unref(builder);

    g_signal_connect(rotwk_upd, "clicked", G_CALLBACK(gui_update_rotwk), NULL);
    g_signal_connect(edain_upd, "clicked", G_CALLBACK(gui_update_edain), NULL);
    g_signal_connect(botta_upd, "clicked", G_CALLBACK(gui_update_botta), NULL);
    g_signal_connect(rotwk_launch, "clicked", G_CALLBACK(gui_launch_rotwk), NULL);
    g_signal_connect(edain_launch, "clicked", G_CALLBACK(gui_launch_edain), NULL);
    g_signal_connect(botta_launch, "clicked", G_CALLBACK(gui_launch_botta), NULL);
    g_signal_connect(quit, "clicked", G_CALLBACK(gui_quit), NULL);
    g_signal_connect(dat_swap, "toggled", G_CALLBACK(gui_toggle_dat_swap), NULL);

    gtk_widget_show(window);
    gtk_main();
}

static void gui_update_rotwk(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    update_config_file(rotwk_toml);
}

static void gui_update_edain(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    update_config_file(edain_toml);
}

static void gui_update_botta(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    update_config_file(botta_toml);
}

static void gui_launch_rotwk(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    gtk_main_quit();
    set_active_configuration(rotwk_toml, swap_dat_file);

    bool mounted = false;
    if(ld.automatic_mount) {
        if(system(ld.mount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
            return;
        }
        mounted = true;
    }

    if(system(launch_cmd) != 0)
        fprintf(stderr, "Failed to launch game.\n");

    while(game_running())
        sleep_for(SLEEP_TIME);
    
    if(mounted && ld.automatic_mount) {
        if(system(ld.umount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
            return;
        }
    }
}

static void gui_launch_edain(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    gtk_main_quit();
    set_active_configuration(edain_toml, swap_dat_file);

    bool mounted = false;
    if(ld.automatic_mount) {
        if(system(ld.mount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
            return;
        }
        mounted = true;
    }

    if(system(launch_cmd) != 0)
        fprintf(stderr, "Failed to launch game.\n");

    while(game_running())
        sleep_for(SLEEP_TIME);
    
    if(mounted && ld.automatic_mount) {
        if(system(ld.umount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
            return;
        }
    }
}

static void gui_launch_botta(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    gtk_main_quit();
    set_active_configuration(botta_toml, swap_dat_file);

    bool mounted = false;
    if(ld.automatic_mount) {
        if(system(ld.mount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
            return;
        }
        mounted = true;
    }

    char botta_launch_cmd[256];
    strcpy(botta_launch_cmd, launch_cmd);
    strcat(botta_launch_cmd, " -mod ");
    strcat(botta_launch_cmd, ld.botta_path);

    if(system(botta_launch_cmd) != 0)
        fprintf(stderr, "Failed to launch game.\n");

    while(game_running())
        sleep_for(SLEEP_TIME);
    
    if(mounted && ld.automatic_mount) {
        if(system(ld.umount_cmd) != 0) {
            fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
            return;
        }
    }

}

static void gui_quit(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    gtk_main_quit();
}

static void gui_toggle_dat_swap(GtkWidget* widget, gpointer data) {
    UNUSED(widget);
    UNUSED(data);
    swap_dat_file = !swap_dat_file;
}

