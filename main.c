#include <gtk/gtk.h>
#include <stdio.h>

static void save_dialog(GObject *source_object, GAsyncResult *res, gpointer data)
{
}

static void saveas_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	// printf("Save as button clicked!!!\n");

	GtkWidget *tv = user_data;
	GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);
  	GtkFileDialog *dialog;

  	dialog = gtk_file_dialog_new ();
  	gtk_file_dialog_save (dialog, GTK_WINDOW (win), NULL, save_dialog, tv);
  	g_object_unref (dialog);
}

static void connect_actions(GApplication *app, GtkWidget *tv)
{
	// create an action saveas
	GSimpleAction *act_saveas = g_simple_action_new ("saveas", NULL);
	g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_saveas));
	
	// connect the action saveas 
	g_signal_connect (act_saveas, "activate", G_CALLBACK (saveas_activated), tv);
}

static void app_activate (GApplication *app)
{
	GtkWidget *win;
	GtkWidget *tv;
	GtkTextBuffer *tb;
	
	GtkApplication *application = GTK_APPLICATION (app);
	
	// Adding File menu to the menubar
	GMenu *menubar = g_menu_new ();
	GMenuItem *menu_item_file = g_menu_item_new ("File", NULL);
	// Creating Save As submenu in the File menu
	GMenu *menu = g_menu_new ();
	GMenuItem *menu_item_save_as = g_menu_item_new ("Save As...", "app.saveas");
	g_menu_append_item (menu, menu_item_save_as);
	g_object_unref (menu_item_save_as);
	g_menu_item_set_submenu (menu_item_file, G_MENU_MODEL (menu));
	g_object_unref (menu);
	g_menu_append_item (menubar, menu_item_file);
	g_object_unref (menu_item_file);
	
	gtk_application_set_menubar (GTK_APPLICATION (application), G_MENU_MODEL (menubar)); 
	
	win = gtk_application_window_new (GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (win), "Code Editor");
	gtk_window_set_default_size (GTK_WINDOW (win), 400, 300);
	
	tv = gtk_text_view_new ();
	tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);
	
	gtk_window_set_child (GTK_WINDOW (win), tv);
	
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (win), TRUE);
	gtk_window_present (GTK_WINDOW (win));

	// connect submenu items to the actions
	connect_actions(app, tv);
}

int main (int argc, char **argv)
{
	GtkApplication *app;
	int stat;
	
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
	stat = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	return stat;
}
