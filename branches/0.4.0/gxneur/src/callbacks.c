#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>

#include "misc.h"
#include "interface.h"
#include "support.h"

#include "callbacks.h"

GtkWidget *wDict;
		
void on_button6_clicked(GtkButton *button, gpointer  user_data)
{
	if (button||user_data){};
	gchar *text = xneur_get_dict(1);
	gchar *text_path = xneur_get_dict_path(1);
	if (text == NULL) 
		return;

	wDict = create_dialog1();
	gtk_widget_show (wDict);
		
	GtkTextBuffer *buffer;	
	GtkWidget *widgetPtrToBefound;
	widgetPtrToBefound = lookup_widget(wDict, "textview1");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgetPtrToBefound));
	gtk_text_buffer_set_text(buffer, text, strlen(text));
	
	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	gtk_entry_set_text (GTK_ENTRY (widgetPtrToBefound), text_path);		
}

void
on_button7_clicked (GtkButton *button, gpointer user_data)
{
	if (button||user_data){};
	gchar *text = xneur_get_dict(2);
	gchar *text_path = xneur_get_dict_path(2);
	if (text == NULL) 
		return;

	wDict = create_dialog1();
	gtk_widget_show (wDict);
		
	GtkTextBuffer *buffer;	
	GtkWidget *widgetPtrToBefound;
	widgetPtrToBefound = lookup_widget(wDict, "textview1");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgetPtrToBefound));
	gtk_text_buffer_set_text(buffer, text, strlen(text));
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(widgetPtrToBefound), buffer); 
		
	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	gtk_entry_set_text (GTK_ENTRY (widgetPtrToBefound), text_path);	 
}


void
on_button8_clicked (GtkButton *button, gpointer user_data)
{
	if (button||user_data){};
	gchar *text = xneur_get_dict(3);
	gchar *text_path = xneur_get_dict_path(3);	
	if (text == NULL) 
		return;

	wDict = create_dialog1();
	gtk_widget_show (wDict);
		
	GtkTextBuffer *buffer;	
	GtkWidget *widgetPtrToBefound;
	widgetPtrToBefound = lookup_widget(wDict, "textview1");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgetPtrToBefound));
	gtk_text_buffer_set_text(buffer, text, strlen(text));

	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	gtk_entry_set_text (GTK_ENTRY (widgetPtrToBefound), text_path);			
}


void
on_button9_clicked (GtkButton *button, gpointer  user_data)
{
	if (button||user_data){};
	gchar *text = xneur_get_dict(4);
	gchar *text_path = xneur_get_dict_path(4);
	if (text == NULL) 
		return;
	
	wDict = create_dialog1();
	gtk_widget_show (wDict);
	
	GtkTextBuffer *buffer;	
	GtkWidget *widgetPtrToBefound;
	widgetPtrToBefound = lookup_widget(wDict, "textview1");
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (widgetPtrToBefound));
	gtk_text_buffer_set_text(buffer, text, strlen(text)); 

	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	gtk_entry_set_text (GTK_ENTRY (widgetPtrToBefound), text_path);			
}

// Save dictionary
void
on_okbutton1_clicked (GtkButton *button, gpointer user_data)
{
	if (button||user_data){};
	GtkWidget *widgetPtrToBefound;
	widgetPtrToBefound = lookup_widget(wDict, "textview1");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgetPtrToBefound));
	GtkTextIter begin, end;
	gtk_text_buffer_get_bounds(buffer, &begin, &end);
	char *text = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
	
	widgetPtrToBefound = lookup_widget(wDict, "entry10");
	char *path = g_strdup_printf("%s",gtk_entry_get_text (GTK_ENTRY(widgetPtrToBefound)));
	
	FILE *stream;
	if ((stream = fopen (path, "w")) != NULL) 
	{
		fputs(text, stream);
		fclose (stream);		
	}
	
	gtk_widget_destroy(wDict);
}
