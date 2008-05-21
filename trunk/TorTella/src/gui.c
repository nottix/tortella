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

/*-- This function allows the program to exit properly when the window is closed --*/
gint destroyapp (GtkWidget *widget, gpointer gdata)
{
  g_print ("Quitting...\n");
  gtk_main_quit();
  return (FALSE);
}

/*-- This function allows the program to exit properly when the window is closed --*/
gint ClosingAppWindow (GtkWidget *widget, gpointer gdata)
{
  g_print ("Quitting...\n");
  gtk_main_quit();
  return (FALSE);
}

//EVENTO CHE SI SCATENA ALLA SELEZIONE DELLA CHAT, PER ORA FA SOLO UN PRINTF, in seguito dovrà aprire la gui della chat
gint open_chat(GtkWidget *widget, GdkEventButton *event, gpointer func_data)  
{
  if (event->type==GDK_2BUTTON_PRESS ||
        event->type==GDK_3BUTTON_PRESS) {
    printf("I feel %s clicked with button %d\n",
           event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
           event->button);
           open_chatroom_gui();  
  }

  return FALSE;
}

gint open_conversation(GtkWidget *widget, GdkEventButton *event, gpointer func_data)  
{
  if (event->type==GDK_2BUTTON_PRESS ||
        event->type==GDK_3BUTTON_PRESS) {
    printf("I feel %s clicked with button %d\n",
           event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
           event->button);
           open_pm_gui();  
  }

  return FALSE;
}

GtkWidget *create_list(int index)
{

    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    GtkListStore *model;
    GtkTreeIter iter;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;

    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, 
				    GTK_POLICY_AUTOMATIC);
   
    model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
                                           tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);
   
    /* Add some messages to the window */
    for (i = 0; i < 10; i++) {
        
        gchar *msg = g_strdup_printf ("Chat #%d", i);
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), 
	                    &iter,
                            0, msg,
	                    -1);
	g_free (msg);
    }
   
    cell = gtk_cell_renderer_text_new ();

    column = gtk_tree_view_column_new_with_attributes ("Messages",
                                                       cell,
                                                       "text", 0,
                                                       NULL);
	
	GtkTreeSelection *select;
	select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);
  
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
	  		         GTK_TREE_VIEW_COLUMN (column));
   if(index==0)
   {   g_signal_connect(G_OBJECT(tree_view),
                     "button_press_event",
                     G_CALLBACK(open_chat),
                     NULL);  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
   }
   if(index==1)
   {  g_signal_connect(G_OBJECT(tree_view),
                     "button_press_event",
                     G_CALLBACK(open_conversation),
                     NULL);  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
   }
    return scrolled_window;
}

GtkWidget *create_menu(void)
{
  GtkWidget *menubar;
  GtkWidget *menuFile;
  GtkWidget *menuEdit;
  GtkWidget *menuHelp;
  GtkWidget *menuitem;
  GtkWidget *menu;
  /*-- Create the menu bar --*/
  menubar = gtk_menu_bar_new();
   /*---------------- Create File menu items ------------------*/

  menuFile = gtk_menu_item_new_with_label ("File");
  gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuFile);
  gtk_widget_show(menuFile);

  /*-- Create File submenu  --*/
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuFile), menu);

  /*-- Create New menu item under File submenu --*/
  menuitem = gtk_menu_item_new_with_label ("New");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);

  /*-- Create Open menu item under File submenu --*/
  menuitem = gtk_menu_item_new_with_label ("Open");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);

  /*-- Create Exit menu item under File submenu --*/
  menuitem = gtk_menu_item_new_with_label ("Exit");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (ClosingAppWindow), NULL);
  gtk_widget_show (menuitem);
  /*---------------- End File menu declarations ----------------*/

  /*---------------- Create Edit menu items --------------------*/

  menuEdit = gtk_menu_item_new_with_label ("Stato");
  gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuEdit);
  gtk_widget_show(menuEdit);

  /*-- Create File submenu --*/
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuEdit), menu);

  /*-- Create Undo menu item under Edit submenu --*/
  menuitem = gtk_menu_item_new_with_label ("Online");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);

  /*-- Create Copy menu item under File submenu --*/
  menuitem = gtk_menu_item_new_with_label ("Busy");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);

  /*-- Create Cut menu item under File submenu --*/
  menuitem = gtk_menu_item_new_with_label ("Away");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);
  /*---------------- End Edit menu declarations ----------------*/

  /*---------------- Start Help menu declarations ----------------*/
  menuHelp = gtk_menu_item_new_with_label ("Help");
  gtk_menu_bar_append (GTK_MENU_BAR(menubar), menuHelp);
  gtk_widget_show(menuHelp);

  /*-- Create Help submenu --*/
  menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuHelp), menu);

  /*-- Create About menu item under Help submenu --*/
  menuitem = gtk_menu_item_new_with_label ("About");
  gtk_menu_append(GTK_MENU(menu), menuitem);
  gtk_widget_show (menuitem);
  /*---------------- End Help menu declarations ----------------*/
  return menubar;
}

GtkWidget *create_text(int i )
{
   GtkWidget *scrolled_window;
   GtkWidget *view;
   GtkTextBuffer *buffer;

   view = gtk_text_view_new ();
   buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  // gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window),gtk_adjustment_new(0,0,1,1,1)); //NON FUNZIONA
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
		   	           GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
   if(i==1)
   	gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE); 
   gtk_container_add (GTK_CONTAINER (scrolled_window), view);
   //insert_text (buffer);

   gtk_widget_show_all (scrolled_window);

   return scrolled_window;
}

int open_chatroom_gui()
{
    /*-- Declare the GTK Widgets used in the program --*/
  GtkWidget *window;
  GtkWidget *menu;
  GtkWidget *vbox;
  GtkWidget *hbox; //
    GtkWidget *vbox2;
  GtkWidget *handlebox;  
  GtkWidget *text;
  GtkWidget *list;
  GtkWidget *chat;

  /*-- Create the new window --*/
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /*-- Create the vbox --*/
  vbox = gtk_vbox_new(FALSE, 0);
  
  hbox = gtk_hbox_new(FALSE, 0);
  vbox2 = gtk_vpaned_new();
    //  gtk_paned_set_position(GTK_PANED(vbox2), 500);  //TROVARE UN MODO CORRETTO DI SETTARE IL SECONDO PARAMETRO PER POSIZIONARE BENE IL SEPARATORE
  /*-- Create a text area --*/
  text = create_text(0);
  chat = create_text(1);
  /*-- Create the handlebox --*/
  handlebox = gtk_handle_box_new();

  menu = create_menu();


  /*-- Connect the window to the destroyapp function  --*/
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroyapp), NULL);

  
  /*-- Add the menubar to the handlebox --*/
  gtk_container_add(GTK_CONTAINER(handlebox), menu);

  /*-- Pack the handlebox into the top of the vbox.  
   *-- You must use gtk_box_pack_start here instead of gtk_container_add
   *-- or the menu will get larger as the window is enlarged
   */
  
  //gtk_box_pack_start(GTK_BOX(hbox), handlebox, FALSE, TRUE, 0);    
  
  gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);    
  
  gtk_container_add(GTK_CONTAINER(vbox),hbox);
  list = create_list (1);
  gtk_container_add(GTK_CONTAINER(hbox),vbox2);
  gtk_container_add (GTK_CONTAINER (hbox), list);
 // gtk_container_add(GTK_CONTAINER(vbox),sep);  
  /*-- Add the text area to the window --*/
  //gtk_container_add(GTK_CONTAINER(vbox), text);  
 // gtk_container_add(GTK_CONTAINER(hbox),vbox2);
  gtk_paned_add1(GTK_PANED(vbox2), chat);
 // gtk_container_add(GTK_CONTAINER(vbox2),sep);
  gtk_paned_add2(GTK_PANED(vbox2), text);
  /*-- Add the vbox to the main window --*/
  gtk_container_add(GTK_CONTAINER(window), vbox);

  /*-- Add some text to the window --*/
//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

  /*-- Set window border to zero so that text area takes up the whole window --*/
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /*-- Set the window to be 640 x 480 pixels --*/
  gtk_window_set_default_size (GTK_WINDOW(window), 640, 200);

  /*-- Set the window title --*/
  gtk_window_set_title(GTK_WINDOW (window), "Chat Room");

  /*-- Display the widgets --*/
  gtk_widget_show(handlebox);
  gtk_widget_show(hbox);
  gtk_widget_show(vbox);
  gtk_widget_show(vbox2);
  gtk_widget_show(text);
  gtk_widget_show (list);
  gtk_widget_show (chat);
  gtk_widget_show(menu);
  gtk_widget_show(window);

  /*-- Start the GTK event loop --*/
  gtk_main();

  /*-- Return 0 if exit is successful --*/
  return 0;
}

int open_pm_gui()
{
   /*-- Declare the GTK Widgets used in the program --*/
  GtkWidget *window;
  GtkWidget *menu;
  GtkWidget *vbox;

    GtkWidget *vbox2;
  GtkWidget *handlebox;  
  GtkWidget *text;
  GtkWidget *chat;

  /*-- Create the new window --*/
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  /*-- Create the vbox --*/
  vbox = gtk_vbox_new(FALSE, 0);
  

  vbox2 = gtk_vpaned_new();
      gtk_paned_set_position(GTK_PANED(vbox2), 500);  //TROVARE UN MODO CORRETTO DI SETTARE IL SECONDO PARAMETRO PER POSIZIONARE BENE IL SEPARATORE
  /*-- Create a text area --*/
  text = create_text(0);
  chat = create_text(1);

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
  
  //gtk_box_pack_start(GTK_BOX(hbox), handlebox, FALSE, TRUE, 0);    
  
  gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);    
  gtk_container_add(GTK_CONTAINER(vbox),vbox2);
  gtk_paned_add1(GTK_PANED(vbox2), chat);
 // gtk_container_add(GTK_CONTAINER(vbox2),sep);
  gtk_paned_add2(GTK_PANED(vbox2), text);
  /*-- Add the vbox to the main window --*/
  gtk_container_add(GTK_CONTAINER(window), vbox);

  /*-- Add some text to the window --*/
//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

  /*-- Set window border to zero so that text area takes up the whole window --*/
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /*-- Set the window to be 640 x 480 pixels --*/
  gtk_window_set_default_size (GTK_WINDOW(window), 640, 200);

  /*-- Set the window title --*/
  gtk_window_set_title(GTK_WINDOW (window), "messaggio privato");

  /*-- Display the widgets --*/
  gtk_widget_show(handlebox);
  gtk_widget_show(vbox);
  gtk_widget_show(vbox2);
  gtk_widget_show(text);
  gtk_widget_show (chat);
  gtk_widget_show(menu);      
  gtk_widget_show(window);

  /*-- Start the GTK event loop --*/
  gtk_main();

  /*-- Return 0 if exit is successful --*/
  return 0; 
}

GtkWidget *create_searchbar(void) {
	GtkWidget *bar_container = gtk_hbox_new(FALSE, 5);
	
	GtkWidget *bar_textfield = gtk_entry_new();
	GtkWidget *bar_button = gtk_button_new();
	
	gtk_entry_set_width_chars(GTK_ENTRY(bar_textfield), (gint)40);
	gtk_button_set_label(GTK_BUTTON(bar_button), (const gchar*)"Search");
	
	gtk_container_add(GTK_CONTAINER(bar_container), bar_textfield);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_textfield, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_button, FALSE, TRUE, 5);
	
	gtk_widget_show(bar_textfield);
	gtk_widget_show(bar_button);
	
	return bar_container;
}
