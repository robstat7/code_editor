#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <gtksourceview/gtksource.h>

char *current_filename[21];
char *filename;

void on_switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data) {
    GtkWidget *win = GTK_WIDGET(user_data);
    const gchar *title = NULL;

    // Get the label of the currently active tab
    GtkWidget *label = gtk_notebook_get_tab_label(notebook, page);
    if (GTK_IS_LABEL(label)) {
        title = gtk_label_get_text(GTK_LABEL(label));
    }

    // Set the window title to the label of the active tab
    if (title) {
        gtk_window_set_title(GTK_WINDOW(win), title);
    } else {
        gtk_window_set_title(GTK_WINDOW(win), "Untitled");
    }
}

static void on_new_tab_clicked(GtkWidget *widget, gpointer notebook)
{
	GtkWidget *scrolled_window, *label, *sv;
	GtkSourceBuffer *sb;
	gint page_num;
	GtkSourceLanguageManager *lang_manager;
    	GtkSourceLanguage *language;
	GtkCssProvider *provider;
	GtkStyleContext *context;
	
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	// Create a new source buffer and view
    	sb = gtk_source_buffer_new(NULL);
    	sv = gtk_source_view_new_with_buffer(sb);
	// tv = gtk_text_view_new ();
	// tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	// gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv), GTK_WRAP_WORD_CHAR);

	// Enable line numbers
    	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sv), TRUE);

	// Set the language for syntax highlighting
    lang_manager = gtk_source_language_manager_get_default();
    language = gtk_source_language_manager_get_language(lang_manager, "c");  // For C programming

    // Apply the language to the source buffer
    gtk_source_buffer_set_language(sb, language);

    // Enable syntax highlighting
    gtk_source_buffer_set_highlight_syntax(sb, TRUE);

	// Add the source view to the scrolled window
	gtk_container_add(GTK_CONTAINER(scrolled_window), sv);

	// Create a label for the tab
    	label = gtk_label_new("Untitled");
	
	// Add the scrolled window (with text view) as a new tab
	page_num = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, label);
	gtk_widget_show_all(scrolled_window);

	// printf("page_num = %d\n", page_num);

	current_filename[page_num] = "Untitled";


	/* Change default font and color throughout the textview widget */
	provider = gtk_css_provider_new ();
	gtk_css_provider_load_from_data (provider,
	                                 "textview {"
	                                 " font: 14px consolas regular;"
	                                 "  color: black;"
	                                 "}",
	                                 -1,
					 NULL);
	context = gtk_widget_get_style_context (sv);
	gtk_style_context_add_provider (context,
	                                GTK_STYLE_PROVIDER (provider),
	                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);



	// Switch to the newly added tab
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_num);

	// connect submenu items to the actions
	//connect_actions(app, tv);
}



static gboolean open_file(char *filename, GtkTextBuffer *tb)
{	
	FILE *file;
	unsigned long file_size;
	char *text;

	file = fopen(filename, "r");
	// Measure the file and allocate memory
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);
	
	text = (char*)malloc(file_size);

	// Save the contents of the file to a variable
	fread(text, sizeof(char), file_size, file);

	gtk_text_buffer_set_text(tb, text, file_size);

	free(text);
	fclose(file);
}

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
	GtkWidget *dialog, *notebook, *page;
	GtkFileChooser *chooser;
	GtkWidget *win;
	GtkTextBuffer *tb;
	gint current_page;
    
    
    	win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

    	notebook = gtk_widget_get_ancestor(tv, GTK_TYPE_NOTEBOOK);

	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

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
		current_filename[current_page] = filename;
        	g_print("cur_fn: %s\n", current_filename[current_page]);
	}

    }
   
	// Update the notebook page label
	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), page, current_filename[current_page]);

    // Destroy the dialog after use
    gtk_widget_destroy(dialog);
}

void ce_text_view_save(GtkWidget *tv)
{
	GtkWidget *notebook;
	gint current_page;

	GtkTextBuffer *tb = gtk_text_view_get_buffer (GTK_TEXT_VIEW (tv));
	GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

    	notebook = gtk_widget_get_ancestor(tv, GTK_TYPE_NOTEBOOK);
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	if(!gtk_text_buffer_get_modified(tb)) {
		return;		/* no need to save it */
	} else if(strncmp(current_filename[current_page],"Untitled", 9) == 0) {
		ce_text_view_saveas(tv);
	} else {
        	g_print("File to save: %s\n", current_filename[current_page]);
		save_file(current_filename[current_page], tb);
	}
}

void ce_open(GtkWidget *tv)
{
	GtkWidget *win, *filechooserdialog, *notebook, *page;
	GtkTextBuffer *tb;
	gint current_page;

    	win = gtk_widget_get_ancestor(GTK_WIDGET (tv), GTK_TYPE_WINDOW);

	notebook = gtk_widget_get_ancestor(tv, GTK_TYPE_NOTEBOOK);
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

	filechooserdialog = gtk_file_chooser_dialog_new("FileChooserDialog", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);

	gtk_dialog_run(GTK_DIALOG(filechooserdialog));

	current_filename[current_page] = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooserdialog));

	g_print("Selected file: %s", current_filename[current_page]);

	open_file(current_filename[current_page], tb);

	// update the notebook page label
	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), page, current_filename[current_page]);

	// update the window title too
	gtk_window_set_title(GTK_WINDOW (win), current_filename[current_page]);

	gtk_widget_destroy(filechooserdialog);
}

void ce_new(GtkWidget *tv)
{
	GtkTextBuffer *tb;
	GtkTextIter start, end;
	GtkWidget *win, *page, *notebook;
	gint current_page;

    	win = gtk_widget_get_ancestor (GTK_WIDGET (tv), GTK_TYPE_WINDOW);

	notebook = gtk_widget_get_ancestor(tv, GTK_TYPE_NOTEBOOK);
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

	gtk_text_buffer_get_start_iter (tb, &start);
	gtk_text_buffer_get_end_iter(tb, &end);

	gtk_text_buffer_delete(tb, &start, &end);

	current_filename[current_page] = "Untitled";

	// update the notebook page label
	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), page, current_filename[current_page]);

	// update the window title too
	gtk_window_set_title(GTK_WINDOW (win), current_filename[current_page]);
}

static void save_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
	GtkWidget *notebook, *scrolled_window, *tv;

	printf("Save submenu clicked!!!\n");
	
	notebook = user_data;

	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	// Get the current page widget (which is the scrolled window)
	scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);

	// Get the child of the scrolled window (which is the text view)
	tv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	ce_text_view_save(tv);
}

static void saveas_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
	GtkWidget *notebook, *scrolled_window, *tv;

	printf("Saveas submenu clicked!!!\n");
	
	notebook = user_data;

	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	// Get the current page widget (which is the scrolled window)
	scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);

	// Get the child of the scrolled window (which is the text view)
	tv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	ce_text_view_saveas(tv);
}

static void open_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
	GtkWidget *notebook, *scrolled_window, *tv;

	printf("Open submenu clicked!!!\n");
	
	notebook = user_data;

	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	// Get the current page widget (which is the scrolled window)
	scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);

	// Get the child of the scrolled window (which is the text view)
	tv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	ce_open(tv);
}

static void new_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
        GtkWidget *notebook, *scrolled_window, *tv;
                                                                                           
        printf("New submenu clicked!!!\n");
        
        notebook = user_data;
                                                                                           
        current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
                                                                                           
        // Get the current page widget (which is the scrolled window)
        scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
                                                                                           
        // Get the child of the scrolled window (which is the text view)
        tv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	ce_new(tv);
}

static void undo_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
	GtkWidget *notebook, *scrolled_window, *sv;
	GtkSourceUndoManager *undo_manager;
	GtkSourceBuffer *sb;

	printf("Undo submenu clicked!!!\n");
	
	notebook = user_data;
											   
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
											   
	// Get the current page widget (which is the scrolled window)
	scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
											   
	// Get the child of the scrolled window (which is the source view)
	sv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sv)));

	// Get the undo manager from the source buffer
	undo_manager = gtk_source_buffer_get_undo_manager(sb);

	if (gtk_source_undo_manager_can_undo(undo_manager)) {
        gtk_source_undo_manager_undo(undo_manager);
    }

}

static void redo_activated(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	gint current_page;
	GtkWidget *notebook, *scrolled_window, *sv;
	GtkSourceUndoManager *undo_manager;
	GtkSourceBuffer *sb;

	printf("Undo submenu clicked!!!\n");
	
	notebook = user_data;
											   
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
											   
	// Get the current page widget (which is the scrolled window)
	scrolled_window = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), current_page);
											   
	// Get the child of the scrolled window (which is the source view)
	sv = gtk_bin_get_child(GTK_BIN(scrolled_window));

	sb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sv)));

	// Get the undo manager from the source buffer
	undo_manager = gtk_source_buffer_get_undo_manager(sb);

	if (gtk_source_undo_manager_can_redo(undo_manager)) {
        gtk_source_undo_manager_redo(undo_manager);
    }

}

static void connect_actions(GtkApplication *app, GtkWidget *notebook)
{
	GSimpleAction *act_redo, *act_saveas, *act_save, *act_new, *act_undo;

	// create an action new 
	act_new = g_simple_action_new("new", NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_new));

	// create an action saveas
	act_saveas = g_simple_action_new ("saveas", NULL);
	g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_saveas));

	// create an action save
	act_save = g_simple_action_new ("save", NULL);
	g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_save));

	// create an action open
	GSimpleAction *act_open = g_simple_action_new("open", NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_open));

	// create an action undo
	act_undo = g_simple_action_new("undo", NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_undo));

	// create an action redo 
	act_redo = g_simple_action_new("redo", NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(act_redo));

	// connect the action new
	g_signal_connect(act_new, "activate", G_CALLBACK (new_activated), notebook);

	// connect the action open
	g_signal_connect (act_open, "activate", G_CALLBACK (open_activated), notebook);

	// connect the action saveas 
	g_signal_connect (act_saveas, "activate", G_CALLBACK (saveas_activated), notebook);

	// connect the action save
	g_signal_connect (act_save, "activate", G_CALLBACK (save_activated), notebook);

	// connect the action undo 
	g_signal_connect (act_undo, "activate", G_CALLBACK (undo_activated), notebook);

	// connect the action redo 
	g_signal_connect (act_redo, "activate", G_CALLBACK (redo_activated), notebook);
}


static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *win, *notebook;
	GtkWidget *toolbar, *vbox, *new_tab_tool_img;
	GtkApplication *application;
	GMenu *menubar, *menu;
	GMenuItem *menu_item_file, *menu_item_edit, *menu_item_open, *menu_item_save_as, *menu_item_save, *menu_item_undo, *menu_item_redo, *menu_item;
	GtkToolItem *new_tab_button;

	application = GTK_APPLICATION (app);
	win = gtk_application_window_new(GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (win), "Code Editor");
	gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);


	// Create a vertical box to pack the toolbar and other widgets
    	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    	gtk_container_add(GTK_CONTAINER(win), vbox);


	// Adding File menu to the menubar
	menubar = g_menu_new ();
	menu_item_file = g_menu_item_new ("File", NULL);

	menu = g_menu_new ();

	// Creating New submenu in the File menu
	menu_item = g_menu_item_new ("New", "app.new");
	g_menu_append_item (menu, menu_item);
	g_object_unref (menu_item);

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


	// Adding edit menu to the menubar	
	menu_item_edit = g_menu_item_new ("Edit", NULL);
	menu = g_menu_new ();

	// Creating undo submenu in the edit menu
	menu_item_undo = g_menu_item_new ("Undo		C+z", "app.undo");
	g_menu_append_item (menu, menu_item_undo);
	g_object_unref (menu_item_undo);

	// Creating redo submenu in the edit menu
	menu_item_redo = g_menu_item_new ("Redo		C+Z", "app.redo");
	g_menu_append_item (menu, menu_item_redo);
	g_object_unref (menu_item_redo);

	g_menu_item_set_submenu (menu_item_edit, G_MENU_MODEL (menu));
	g_object_unref (menu);

	g_menu_append_item (menubar, menu_item_edit);
	g_object_unref (menu_item_edit);

	
	gtk_application_set_menubar (GTK_APPLICATION (application), G_MENU_MODEL (menubar)); 
	

	toolbar = gtk_toolbar_new();

	// Set toolbar style (optional: can be icons, text, or both)
    	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);


	// Create "New Tab" button
	new_tab_tool_img = gtk_image_new_from_file("resources/new_tab.png");
    	new_tab_button = gtk_tool_button_new(new_tab_tool_img, "New Tab");
    	gtk_tool_button_set_label(GTK_TOOL_BUTTON(new_tab_button), "New Tab");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_tab_button, -1);

	// Add the toolbar to the vertical box
    	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);


	// Create a notebook for multiple tabs
    	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);


	// Connect the 'switch-page' signal to change the window title when tabs are switched
    	g_signal_connect(notebook, "switch-page", G_CALLBACK(on_switch_page), win);

	// Create the first tab manually
    	on_new_tab_clicked(NULL, notebook);
	
	// connect the new tab button to the callback function
	g_signal_connect(new_tab_button, "clicked", G_CALLBACK(on_new_tab_clicked), notebook);

	gtk_window_present (GTK_WINDOW (win));
	gtk_widget_show_all(win);

	// connect submenu items to the actions
	connect_actions(app, notebook);
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
