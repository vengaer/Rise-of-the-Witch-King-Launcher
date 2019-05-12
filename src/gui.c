#include "gui.h"

static void gui_update_config(char const* filename);
static void gui_active_config(char const* filename); 

void gui_run(int argc, char** argv) {
    GtkBuilder* builder;
    GObject* window;
    GObject* button;
    GError* error = NULL;
    
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    if(gtk_builder_add_from_file(builder, "xml/main.ui", &error) == 0) {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return;
    }
    
    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    button = gtk_builder_get_object(builder, "rotwk_run");
    g_signal_connect(button, "clicked", G_CALLBACK(gui_active_config), "toml/rotwk.toml");

    button = gtk_builder_get_object(builder, "edain_run");
    g_signal_connect(button, "clicked", G_CALLBACK(gui_active_config), "toml/edain.toml");

    button = gtk_builder_get_object(builder, "botta_update");
    g_signal_connect(button, "clicked", G_CALLBACK(gui_active_config), "toml/botta.toml");
    
    button = gtk_builder_get_object(builder, "edain_update");
    g_signal_connect(button, "clicked", G_CALLBACK(gui_update_config), "toml/edain.toml");

    button = gtk_builder_get_object(builder, "botta_update");
    g_signal_connect(button, "clicked", G_CALLBACK(gui_update_config), "toml/botta.toml");

    button = gtk_builder_get_object(builder, "quit");
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();
}

static void gui_update_config(char const* filename) {
    update_config_file(filename);
}

static void gui_active_config(char const* filename) {
    active_configuration(filename);
}
