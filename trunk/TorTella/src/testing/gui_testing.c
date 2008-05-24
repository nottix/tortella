/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "gui.h"
 
 int main (int argc, char *argv[])
{
  /*-- Declare the GTK Widgets used in the program --*/
  GtkWidget *window;
  GtkWidget *menu;
  GtkWidget *vbox;
  GtkWidget *handlebox;  
  GtkWidget *list;
	GtkWidget *searchbar;

  /*--  Initialize GTK --*/
  gtk_init (&argc, &argv);

  /*-- Create the new window --*/
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /*-- Create the vbox --*/
  vbox = gtk_vbox_new(FALSE, 5);
  list = create_chat_list(0);
  /*-- Create the handlebox --*/
  handlebox = gtk_handle_box_new();

  /*-- Create the menu bar --*/
  menu = create_menu();

  /*-- Connect the window to the destroyapp function  --*/
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroyapp), NULL);

  

  /*-- Add the menubar to the handlebox --*/
  gtk_container_add(GTK_CONTAINER(handlebox), menu);

  /*-- Pack the handlebox into the top of the vbox.  
   *-- You must use gtk_box_pack_start here instead of gtk_container_add
   *-- or the menu will get larger as the window is enlarged
   */
	
	searchbar = create_searchbar();
	
	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), searchbar, FALSE, TRUE, 5);
	
    gtk_container_add(GTK_CONTAINER(vbox),list);
	

	
		

	
	//gtk_box_pack_start(GTK_BOX(vbox), searchbar, FALSE, TRUE, 0);  

  /*-- Add the vbox to the main window --*/
  gtk_container_add(GTK_CONTAINER(window), vbox);

 
  /*-- Set window border to zero so that text area takes up the whole window --*/
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /*-- Set the window to be 640 x 480 pixels --*/
  gtk_window_set_default_size (GTK_WINDOW(window), 300, 400);
   
  /*-- Set the window title --*/
  gtk_window_set_title(GTK_WINDOW (window), "Lista Chat");
 
  
  /*-- Display the widgets --*/
  gtk_widget_show(handlebox);
  gtk_widget_show(vbox);
  //gtk_widget_show(menuitem);
  gtk_widget_show(searchbar);   
  gtk_widget_show(menu);   
  gtk_widget_show(window);
  gtk_widget_show(list);

  /*-- Start the GTK event loop --*/
  gtk_main();

  /*-- Return 0 if exit is successful --*/
  return 0;
}
