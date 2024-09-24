#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *win;
	GtkWidget *tv;
	GtkTextBuffer *tb;
	GtkApplication *application;
	GMenu *menubar, *menu;
	GMenuItem *menu_item_file, *menu_item_open, *menu_item_save_as, *menu_item_save;

	application = GTK_APPLICATION (app);
	win = gtk_application_window_new(GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (win), "Code Editor");
	gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);

	// Adding File menu to the menubar
	menubar = g_menu_new ();
	menu_item_file = g_menu_item_new ("File", NULL);

	menu = g_menu_new ();

	// create "open" submenu in the "file" menu
	menu_item_open = g_menu_item_new("Open...", "app.open");
	g_menu_append_item(menu, menu_item_open);
	g_object_unref(menu_item_open);

	// Creating Save As submenu in the File menu
	menu_item_save_as = g_menu_item_new ("Save As...", "app.saveas");
	g_menu_append_item (menu, menu_item_save_as);
	g_object_unref (menu_item_save_as);

	// Creating Save submenu in the File menu
	menu_item_save = g_menu_item_new ("Save", "app.save");
	g_menu_append_item (menu, menu_item_save);
	g_object_unref (menu_item_save);
	
	g_menu_item_set_submenu (menu_item_file, G_MENU_MODEL (menu));
	g_object_unref (menu);

	g_menu_append_item (menubar, menu_item_file);
	g_object_unref (menu_item_file);
	
	gtk_application_set_menubar (GTK_APPLICATION (application), G_MENU_MODEL (menubar)); 

	tv = gtk_text_view_new ();
	tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);

	gtk_container_add(GTK_CONTAINER(win), tv);
	gtk_window_present (GTK_WINDOW (win));
	gtk_widget_show_all(win);
}

int main(int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
