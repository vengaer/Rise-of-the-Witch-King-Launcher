#include "glade_gui.h"
#include "command.h"
#include "config.h"
#include "fsys.h"
#include "game_data.h"
#include <gtk/gtk.h>
#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>

#define ROTWKL_DEVEL

#define UNUSED(x) (void)(x)

#define PROTECT(lock) \
for(int* i_lock = &lock; i_lock && *i_lock == 0; i_lock = NULL) \
    for(*i_lock = 1; i_lock; *i_lock = 0, i_lock = NULL)

static void gui_update_rotwk(GtkButton* button, gpointer data);
static void gui_update_edain(GtkButton* button, gpointer data);
static void gui_update_botta(GtkButton* button, gpointer data);
static void gui_update_all(GtkButton* button, gpointer data);
static void gui_launch_rotwk(GtkButton* button, gpointer data);
static void gui_launch_edain(GtkButton* button, gpointer data);
static void gui_launch_botta(GtkButton* button, gpointer data);
static void gui_toggle_dat_swap(GtkSwitch* toggle, gpointer data);
static void gui_botta_toggle(GtkSwitch* gswitch, gpointer data);
static void gui_mount_toggle(GtkSwitch* gswitch, gpointer data);
static void gui_save_preferences(GtkButton* button, gpointer data);
static void gui_close_game_path_dialog(GtkButton* button, gpointer data);

static void gui_launch(configuration config);
static bool gui_setup_config(void);

static char const CONFIG_FILE[] = "rotwkl.toml";
static launcher_data ld;
static char launch_cmd[256];
static char rotwk_toml[128];
static char edain_toml[128];
static char botta_toml[128];
static bool swap_dat_file;
static char game_csum[64];
static bool mounting_necessary;
static int lock_gui;
static bool config_exists;

static GObject *game_dir, *botta_dir;
static GObject *edain_toggle, *botta_toggle, *mount_toggle;
static GObject *mount_exe, *image;
static GObject *mount_flag, *umount_flag, *umount_imspec_toggle;
static GObject *pref_switcher;
static GObject *game_path_dialog;
static GObject *game_path_dialog_label;

void gui_init(int* argc, char*** argv) {
    GtkBuilder* builder;
    GtkWidget* window;
    GError* error = NULL;
    GObject *quit1, *quit2, *quit3;
    GObject *dat_swap;
    GObject *rotwk_upd, *edain_upd, *botta_upd, *all_upd;
    GObject *rotwk_launch, *edain_launch, *botta_launch;
    GObject *pref_save;
    GObject *main_stack;
    GObject *pref_pane;
    GObject *game_path_dialog_close;

    lock_gui = false;
    game_csum[0] = '\0';

    config_exists = gui_setup_config();
    
    gtk_init(argc, argv);
    builder = gtk_builder_new();
    
    if(gtk_builder_add_from_file(builder, "gui/launcher.glade", &error) == 0) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    rotwk_upd              = gtk_builder_get_object(builder, "rotwk_upd");
    edain_upd              = gtk_builder_get_object(builder, "edain_upd");
    botta_upd              = gtk_builder_get_object(builder, "botta_upd");
    all_upd                = gtk_builder_get_object(builder, "all_upd");
    rotwk_launch           = gtk_builder_get_object(builder, "rotwk_launch");
    edain_launch           = gtk_builder_get_object(builder, "edain_launch");
    botta_launch           = gtk_builder_get_object(builder, "botta_launch");
    quit1                  = gtk_builder_get_object(builder, "quit1");
    quit2                  = gtk_builder_get_object(builder, "quit2");
    quit3                  = gtk_builder_get_object(builder, "quit3");
    dat_swap               = gtk_builder_get_object(builder, "dat_swap");
    game_dir               = gtk_builder_get_object(builder, "game_dir");
    botta_dir              = gtk_builder_get_object(builder, "botta_dir");
    edain_toggle           = gtk_builder_get_object(builder, "edain_toggle");
    botta_toggle           = gtk_builder_get_object(builder, "botta_toggle");
    mount_toggle           = gtk_builder_get_object(builder, "mount_toggle");
    mount_exe              = gtk_builder_get_object(builder, "mount_exe");
    image                  = gtk_builder_get_object(builder, "image");
    mount_flag             = gtk_builder_get_object(builder, "mount_flag");
    umount_flag            = gtk_builder_get_object(builder, "umount_flag");
    umount_imspec_toggle   = gtk_builder_get_object(builder, "umount_imspec_toggle");
    pref_save              = gtk_builder_get_object(builder, "pref_save");
    main_stack             = gtk_builder_get_object(builder, "main_stack");
    pref_pane              = gtk_builder_get_object(builder, "pref_pane");
    pref_switcher          = gtk_builder_get_object(builder, "pref_switcher");
    game_path_dialog       = gtk_builder_get_object(builder, "game_path_dialog");
    game_path_dialog_close = gtk_builder_get_object(builder, "game_path_dialog_close");
    game_path_dialog_label = gtk_builder_get_object(builder, "game_path_label");
    
    gtk_switch_set_active(GTK_SWITCH(dat_swap), ld.swap_dat_file);

    gtk_switch_set_active(GTK_SWITCH(edain_toggle), ld.edain_available);
    gtk_switch_set_active(GTK_SWITCH(botta_toggle), ld.botta_available);
    gtk_switch_set_active(GTK_SWITCH(mount_toggle), ld.automatic_mount);
    gtk_switch_set_active(GTK_SWITCH(mount_toggle), ld.umount_imspec);

    gtk_widget_set_sensitive(GTK_WIDGET(botta_dir), ld.botta_available);
    gtk_widget_set_sensitive(GTK_WIDGET(mount_exe), ld.automatic_mount);
    gtk_widget_set_sensitive(GTK_WIDGET(image), ld.automatic_mount);
    gtk_widget_set_sensitive(GTK_WIDGET(mount_flag), ld.automatic_mount);
    gtk_widget_set_sensitive(GTK_WIDGET(umount_flag), ld.automatic_mount);
    gtk_widget_set_sensitive(GTK_WIDGET(umount_imspec_toggle), ld.automatic_mount);

    if(config_exists) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(game_dir), ld.game_path);
        if(ld.botta_available)
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(botta_dir), ld.botta_path);
        if(ld.automatic_mount) {
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(mount_exe), ld.mount_exe);
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(image), ld.disc_image);
            gtk_entry_set_text(GTK_ENTRY(mount_flag), ld.mount_flags);
            gtk_entry_set_text(GTK_ENTRY(umount_flag), ld.umount_flags);
        }
    }
    else {
        gtk_stack_set_visible_child(GTK_STACK(main_stack), GTK_WIDGET(pref_pane));
        gtk_widget_set_sensitive(GTK_WIDGET(pref_switcher), false);
    }

    g_object_unref(builder);

    g_signal_connect(GTK_BUTTON(rotwk_upd), "clicked", G_CALLBACK(gui_update_rotwk), NULL);
    g_signal_connect(GTK_BUTTON(edain_upd), "clicked", G_CALLBACK(gui_update_edain), NULL);
    g_signal_connect(GTK_BUTTON(botta_upd), "clicked", G_CALLBACK(gui_update_botta), NULL);
    g_signal_connect(GTK_BUTTON(all_upd), "clicked", G_CALLBACK(gui_update_all), NULL);
    g_signal_connect(GTK_BUTTON(rotwk_launch), "clicked", G_CALLBACK(gui_launch_rotwk), NULL);
    g_signal_connect(GTK_BUTTON(edain_launch), "clicked", G_CALLBACK(gui_launch_edain), NULL);
    g_signal_connect(GTK_BUTTON(botta_launch), "clicked", G_CALLBACK(gui_launch_botta), NULL);
    g_signal_connect(GTK_BUTTON(quit1), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_BUTTON(quit2), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_BUTTON(quit3), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_SWITCH(dat_swap), "state-set", G_CALLBACK(gui_toggle_dat_swap), NULL);
    g_signal_connect(GTK_SWITCH(botta_toggle), "state-set", G_CALLBACK(gui_botta_toggle), NULL);
    g_signal_connect(GTK_SWITCH(mount_toggle), "state-set", G_CALLBACK(gui_mount_toggle), NULL);
    g_signal_connect(GTK_BUTTON(pref_save), "clicked", G_CALLBACK(gui_save_preferences), NULL);
    g_signal_connect(GTK_BUTTON(game_path_dialog_close), "clicked", G_CALLBACK(gui_close_game_path_dialog), NULL);

    gtk_widget_show(window);
    gtk_main();
}

static void gui_update_rotwk(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        update_config_file(rotwk_toml);
        lock_gui = false;
    }
}

static void gui_update_edain(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        update_config_file(edain_toml);
    }
}

static void gui_update_botta(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        update_config_file(botta_toml);
    }
}

static void gui_update_all(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        gui_update_rotwk(button, data);
        if(ld.edain_available)
            gui_update_edain(button, data);
        if(ld.botta_available)
            gui_update_botta(button, data);
    }
}

static void gui_launch_rotwk(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        
        gui_launch(rotwk);
    }
}

static void gui_launch_edain(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        if(!ld.edain_available)
            return;

        gui_launch(edain);
    }
}

static void gui_launch_botta(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);

        if(!ld.botta_available)
            return;

        gui_launch(botta);
    }
}

static void gui_toggle_dat_swap(GtkSwitch* toggle, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(data);
        swap_dat_file = gtk_switch_get_active(toggle);
    }
}

static void gui_botta_toggle(GtkSwitch* gswitch, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(data);
        gtk_widget_set_sensitive(GTK_WIDGET(botta_dir), gtk_switch_get_active(gswitch));
    }
}

static void gui_mount_toggle(GtkSwitch* gswitch, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(data);
        gtk_widget_set_sensitive(GTK_WIDGET(mount_exe), gtk_switch_get_active(gswitch));
        gtk_widget_set_sensitive(GTK_WIDGET(image), gtk_switch_get_active(gswitch));
        gtk_widget_set_sensitive(GTK_WIDGET(mount_flag), gtk_switch_get_active(gswitch));
        gtk_widget_set_sensitive(GTK_WIDGET(umount_flag), gtk_switch_get_active(gswitch));
        gtk_widget_set_sensitive(GTK_WIDGET(umount_imspec_toggle), gtk_switch_get_active(gswitch));
    }
}  

static void gui_save_preferences(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(data);
        UNUSED(button);

        char const* game_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(game_dir));
        if(game_path) {
            strcpy(ld.game_path, game_path);
        }
        else {
            gtk_label_set_text(GTK_LABEL(game_path_dialog_label), "Please specify the game path ");
            gtk_widget_set_visible(GTK_WIDGET(game_path_dialog), true);
            lock_gui = false;
            return;
        }

#ifndef ROTWKL_DEVEL
        char exe_path[256];
        strcpy(exe_path, game_path);
        strcat(exe_path, "/lotrbfme2ep1.exe");
        if(!file_exists(exe_path)) {
            gtk_label_set_text(GTK_LABEL(game_path_dialog_label), "Could not locate lotrbfme2ep1.exe in specified path");
            gtk_widget_set_visible(GTK_WIDGET(game_path_dialog), true);
            lock_gui = false;
            return;
        }
#endif

        ld.edain_available = gtk_switch_get_active(GTK_SWITCH(edain_toggle));
        ld.botta_available = gtk_switch_get_active(GTK_SWITCH(botta_toggle));
        ld.swap_dat_file   = swap_dat_file;
        
        if(ld.botta_available) {
            char const* botta_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(botta_dir));
            
            if(botta_path)
                strcpy(ld.botta_path, botta_path);
        }

        ld.automatic_mount = gtk_switch_get_active(GTK_SWITCH(mount_toggle));
        if(ld.automatic_mount) {
            char const* mexec = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(mount_exe));
            char const* mimg = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(image));
            char const* mflags = gtk_entry_get_text(GTK_ENTRY(mount_flag));
            char const* umflags = gtk_entry_get_text(GTK_ENTRY(umount_flag));
            
            ld.umount_imspec = gtk_switch_get_active(GTK_SWITCH(umount_imspec_toggle));
            if(!mexec) {
                gtk_label_set_text(GTK_LABEL(game_path_dialog_label), "Please specify mounting executable");
                gtk_widget_set_visible(GTK_WIDGET(game_path_dialog), true);
                lock_gui = false;
                return;
            }
            if(!mimg) {
                gtk_label_set_text(GTK_LABEL(game_path_dialog_label), "Please specify disc image");
                gtk_widget_set_visible(GTK_WIDGET(game_path_dialog), true);
                lock_gui = false;
                return;
            }

            strcpy(ld.mount_exe, mexec);
            strcpy(ld.disc_image, mimg);
            strcpy(ld.mount_flags, mflags);
            strcpy(ld.umount_flags, umflags);
            construct_mount_command(ld.mount_cmd, mexec, mflags, mimg);
            construct_umount_command(ld.umount_cmd, mexec, umflags, mimg, ld.umount_imspec);
        }

        write_launcher_config(&ld, CONFIG_FILE);
        if(!config_exists) {
            config_exists = true;
            gtk_widget_set_sensitive(GTK_WIDGET(pref_switcher), true);
            gui_setup_config();
        }
    }
}   

static void gui_launch(configuration config) {
    PROTECT(lock_gui) {
        bool mounting_successful;
        #pragma omp parallel num_threads(2) 
        {
        #pragma omp single 
        {
        #pragma omp task
        {
            if(swap_dat_file != ld.swap_dat_file) {
                ld.swap_dat_file = swap_dat_file;
                write_launcher_config(&ld, CONFIG_FILE);
            }
        }
            
            gtk_main_quit();
            if(config == rotwk)
                set_active_configuration(rotwk_toml, swap_dat_file);
            else if(config == edain)
                set_active_configuration(edain_toml, swap_dat_file);
            else
                set_active_configuration(botta_toml, swap_dat_file);

            if(mounting_necessary && ld.automatic_mount) {
                if(system(ld.mount_cmd) != 0) {
                    fprintf(stderr, "'%s' returned an error\n", ld.mount_cmd);
                    mounting_successful = false;
                }
                else
                    mounting_successful = true;
            }

            if(!mounting_necessary || mounting_successful) {
                if(config == botta) {
                    char botta_launch_cmd[256];
                    strcpy(botta_launch_cmd, launch_cmd);
                    strcat(botta_launch_cmd, " -mod ");
                    strcat(botta_launch_cmd, ld.botta_path);

                    if(system(botta_launch_cmd) != 0)
                        fprintf(stderr, "Failed to launch game.\n");
                }
                else if(system(launch_cmd) != 0)
                        fprintf(stderr, "Failed to launch game.\n");
                
                while(game_running())
                    sleep_for(SLEEP_TIME);
                
                if(mounting_necessary && ld.automatic_mount) {
                    if(system(ld.umount_cmd) != 0)
                        fprintf(stderr, "'%s' returned an error\n", ld.umount_cmd);
                }
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
    #pragma omp task if(!game_csum[0])
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

static void gui_close_game_path_dialog(GtkButton* button, gpointer data) {
    PROTECT(lock_gui) {
        UNUSED(button);
        UNUSED(data);
        gtk_widget_set_visible(GTK_WIDGET(game_path_dialog), false);
        
    }
}   

