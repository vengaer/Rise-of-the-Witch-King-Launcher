#include "glade_gui.h"
#include "command.h"
#include "config.h"
#include "fsys.h"
#include "game_data.h"
#include <gtk/gtk.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

#define UNUSED(x) (void)(x)

static void gui_update_rotwk(GtkWidget* widget, gpointer data);
static void gui_update_edain(GtkWidget* widget, gpointer data);
static void gui_update_botta(GtkWidget* widget, gpointer data);
static void gui_update_all(GtkWidget* widget, gpointer data);
static void gui_launch_rotwk(GtkWidget* widget, gpointer data);
static void gui_launch_edain(GtkWidget* widget, gpointer data);
static void gui_launch_botta(GtkWidget* widget, gpointer data);
static void gui_toggle_dat_swap(GtkToggleButton* toggle, gpointer data);

static void gui_launch(configuration config);
static bool gui_setup_config(void);

static char const CONFIG_FILE[] = "rotwkl.toml";
static launcher_data ld;
static char launch_cmd[256];
static char rotwk_toml[128];
static char edain_toml[128];
static char botta_toml[128];
static bool swap_dat_file = true;
static char game_csum[64];
static bool mounting_necessary;
static bool lock_gui;

void gui_init(int* argc, char*** argv) {
    GtkBuilder* builder;
    GtkWidget* window;
    GError* error = NULL;
    GObject *quit1, *quit2, *dat_swap;
    GObject *rotwk_upd, *edain_upd, *botta_upd, *all_upd;
    GObject *rotwk_launch, *edain_launch, *botta_launch;

    lock_gui = false;

    //if(!gui_setup_config())
        //return;
    gui_setup_config();
    
    gtk_init(argc, argv);
    builder = gtk_builder_new();
    
    if(gtk_builder_add_from_file(builder, "alt_launch.glade", &error) == 0) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    rotwk_upd    = gtk_builder_get_object(builder, "rotwk_upd");
    edain_upd    = gtk_builder_get_object(builder, "edain_upd");
    botta_upd    = gtk_builder_get_object(builder, "botta_upd");
    all_upd      = gtk_builder_get_object(builder, "all_upd");
    rotwk_launch = gtk_builder_get_object(builder, "rotwk_launch");
    edain_launch = gtk_builder_get_object(builder, "edain_launch");
    botta_launch = gtk_builder_get_object(builder, "botta_launch");
    quit1        = gtk_builder_get_object(builder, "quit1");
    quit2        = gtk_builder_get_object(builder, "quit2");
    dat_swap     = gtk_builder_get_object(builder, "dat_swap");
    
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(dat_swap), ld.swap_dat_file);

    g_object_unref(builder);

    g_signal_connect(rotwk_upd, "clicked", G_CALLBACK(gui_update_rotwk), NULL);
    g_signal_connect(edain_upd, "clicked", G_CALLBACK(gui_update_edain), NULL);
    g_signal_connect(botta_upd, "clicked", G_CALLBACK(gui_update_botta), NULL);
    g_signal_connect(all_upd, "clicked", G_CALLBACK(gui_update_all), NULL);
    g_signal_connect(rotwk_launch, "clicked", G_CALLBACK(gui_launch_rotwk), NULL);
    g_signal_connect(edain_launch, "clicked", G_CALLBACK(gui_launch_edain), NULL);
    g_signal_connect(botta_launch, "clicked", G_CALLBACK(gui_launch_botta), NULL);
    g_signal_connect(quit1, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(quit2, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_TOGGLE_BUTTON(dat_swap), "toggled", G_CALLBACK(gui_toggle_dat_swap), NULL);

    gtk_widget_show(window);
    gtk_main();
}

static void gui_update_rotwk(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);
        update_config_file(rotwk_toml);
        lock_gui = false;
    }
}

static void gui_update_edain(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);
        update_config_file(edain_toml);
        lock_gui = false;
    }
}

static void gui_update_botta(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);
        update_config_file(botta_toml);
        lock_gui = false;
    }
}

static void gui_update_all(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {  
        lock_gui = true;
        gui_update_rotwk(widget, data);
        if(ld.edain_available)
            gui_update_edain(widget, data);
        if(ld.botta_available)
            gui_update_botta(widget, data);
        lock_gui = false;
    }
}

static void gui_launch_rotwk(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);
        
        gui_launch(rotwk);
        lock_gui = false;
    }
}

static void gui_launch_edain(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);
        if(!ld.edain_available)
            return;

        gui_launch(edain);
        lock_gui = false;
    }
}

static void gui_launch_botta(GtkWidget* widget, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(widget);
        UNUSED(data);

        if(!ld.botta_available)
            return;

        gui_launch(botta);
        lock_gui = false;
    }

}

static void gui_toggle_dat_swap(GtkToggleButton* toggle, gpointer data) {
    if(!lock_gui) {
        lock_gui = true;
        UNUSED(data);
        swap_dat_file = gtk_toggle_button_get_active(toggle);
        lock_gui = false;
    }
}

static void gui_launch(configuration config) {
    bool mounting_successful;
    #pragma omp parallel num_threads(2) 
    {
    #pragma omp single 
    {
    #pragma omp task
    {
        if(swap_dat_file != ld.swap_dat_file)
            write_launcher_config(&ld, CONFIG_FILE);
    }
        
        gtk_main_quit();
        if(config == rotwk)
            set_active_configuration(rotwk_toml, swap_dat_file);
        else if(config == edain)
            set_active_configuration(edain_toml, swap_dat_file);
        else
            set_active_configuration(botta_toml, swap_dat_file);

        if(ld.automatic_mount) {
            if(system(ld.mount_cmd) != 0) {
                fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
                mounting_successful = false;
            }
            else
                mounting_successful = true;
        }

        if(mounting_successful) {
            if(config == botta) {
                char botta_launch_cmd[256];
                strcpy(botta_launch_cmd, launch_cmd);
                strcat(botta_launch_cmd, " -mod ");
                strcat(botta_launch_cmd, ld.botta_path);

                if(system(botta_launch_cmd) != 0)
                    fprintf(stderr, "Failed to launch game.\n");
            }
            else {
                if(system(launch_cmd) != 0)
                    fprintf(stderr, "Failed to launch game.\n");
            }
            
            while(game_running())
                sleep_for(SLEEP_TIME);
            
            if(ld.automatic_mount) {
                if(system(ld.umount_cmd) != 0)
                    fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
            }
        }
    }
    }

}   


static bool gui_setup_config(void) {
    bool config_found;

    #pragma omp parallel num_threads(2)
    {
    #pragma omp single
    {
    #pragma omp task
    {   
        md5sum("game.dat", game_csum);
        mounting_necessary = strcmp(game_csum, NEW_DAT_CSUM);
    }
        config_found = read_launcher_config(&ld, CONFIG_FILE);

        cd_to_game_path(&ld);
        
        if(config_found) {
            construct_from_rel_path(&ld, launch_cmd, "/lotrbfme2ep1.exe");
            construct_from_rel_path(&ld, rotwk_toml, "/toml/rotwk.toml");
            construct_from_rel_path(&ld, edain_toml, "/toml/edain.toml");
            construct_from_rel_path(&ld, botta_toml, "/toml/botta.toml");
        }
    }
    }
    return config_found;
}
