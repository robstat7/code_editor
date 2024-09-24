#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *win;
	GtkWidget *tv;
	GtkTextBuffer *tb;
	GtkApplication *application;

	application = GTK_APPLICATION (app);
	win = gtk_application_window_new(GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (win), "Code Editor");
	gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);

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
