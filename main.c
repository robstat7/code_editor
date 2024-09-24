#include <gtk/gtk.h>

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
	GtkWidget *win;
	GtkApplication *application;

	application = GTK_APPLICATION (app);
	win = gtk_application_window_new(GTK_APPLICATION (application));
	gtk_window_set_title (GTK_WINDOW (win), "Code Editor");
	gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
	gtk_window_present (GTK_WINDOW (win));
}



int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
