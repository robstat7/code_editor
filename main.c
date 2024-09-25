#include <gtk/gtk.h>
#include <stdio.h>

char *current_filename = NULL;
char *filename;

static gboolean save_file(char *filename, GtkTextBuffer *tb)
{
	GtkTextIter start, end;
	gchar* text;
	GError *err = NULL;
	gboolean result;

	gtk_text_buffer_get_start_iter (tb, &start);
	gtk_text_buffer_get_end_iter(tb, &end);
	text = gtk_text_buffer_get_text (tb, &start, &end, FALSE);
	gtk_text_buffer_set_modified (tb, FALSE);

    	result = g_file_set_contents (filename, text, -1, &err);

	if (result == FALSE)
	{
    		/* error saving file, show message to user */
    		// error_message (err->message);
    		g_error_free (err);
	}
	else
	{
		gtk_text_buffer_set_modified (tb, FALSE);
	}


	g_free (text);
	
	return result;
}


void ce_text_view_saveas(GtkWidget *tv)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	GtkWidget *win;
	GtkTextBuffer *tb;
    
    
    win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

    // Create the file chooser dialog
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         GTK_WINDOW(win),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Show the dialog and wait for a response
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        // Get the filename
        chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Here you can handle the file saving logic
        g_print("File to save as: %s\n", filename);


	tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	
	if(save_file(filename, tb)) {
		current_filename = filename;
        	g_print("cur_fn: %s\n", current_filename);
	}

    }

    // Destroy the dialog after use
    gtk_widget_destroy(dialog);
}

void ce_text_view_save(GtkWidget *tv)
{
	GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

	if(!gtk_text_buffer_get_modified(tb)) {
		return;		/* no need to save it */
	} else if(current_filename == NULL) {
		ce_text_view_saveas(tv);
	} else {
        	g_print("File to save: %s\n", current_filename);
		save_file(current_filename, tb);
	}
}

static void save_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	printf("Save submenu clicked!!!\n");
	
	GtkWidget *tv = user_data;

	ce_text_view_save(tv);
}

static void saveas_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	printf("Save as button clicked!!!\n");

	GtkWidget *tv = user_data;
  
	ce_text_view_saveas (tv);
}

static void connect_actions(GtkApplication *app, GtkWidget *tv)
{
	GSimpleAction *act_saveas, *act_save;

	// create an action saveas
	act_saveas = g_simple_action_new ("saveas", NULL);
	g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_saveas));

	// create an action save
	act_save = g_simple_action_new ("save", NULL);
	g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_save));


	// connect the action saveas 
	g_signal_connect (act_saveas, "activate", G_CALLBACK (saveas_activated), tv);

	// connect the action save
	g_signal_connect (act_save, "activate", G_CALLBACK (save_activated), tv);

}


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

	// connect submenu items to the actions
	connect_actions(app, tv);
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
