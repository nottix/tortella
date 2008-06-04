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
void destroyapp (GtkWidget *widget, gpointer gdata)
{
  g_print ("Quitting...\n");
  controller_exit();
  gtk_main_quit();
  exit(0);
}

gint destroywindow(GtkWidget *widget, gpointer gdata)
{
  g_print("Closing window\n");
  //gtk_main_quit();
  gdk_window_destroy(GTK_WINDOW(widget)); //reinserita la semplice chiusura della finestra e non dell'intera applicazione
	//Aggiungere il leave dalla chat o dalla conversazione privata
  return(FALSE);
}

/*-- This function allows the program to exit properly when the window is closed --*/
gint ClosingAppWindow (GtkWidget *widget, gpointer gdata) {
	g_print ("Quitting...\n");
	gtk_main_quit();
	return (FALSE);
}

gint add_chat_to_list(u_int8 chat_id, char *chat_name)
{
  gchar *msg = g_strdup_printf (to_string(chat_id));
	gchar *msg1 = g_strdup_printf (chat_name);
	printf("msg: %s\n", msg1);
        gtk_list_store_append (chat_model, &chat_iter);
	gtk_list_store_set (GTK_LIST_STORE(chat_model), &chat_iter, 0, msg, 1, msg1, -1);
	printf("msg: %s\n", msg);
	g_free (msg);
	
	//gchar *msg1 = g_strdup_printf (chat_name);
	//printf("msg: %s\n", msg1);
    // gtk_list_store_append (GTK_LIST_STORE (chat_model), &chat_iter);
   //  gtk_list_store_set_(GTK_LIST_STORE (chat_model), &chat_iter, msg1, -1);
	g_free (msg1);
	return 0;
}

gint add_user_to_chat_list(u_int8 chat_id, u_int8 id, char *user, u_int1 status)
{
	gchar *msg = g_strdup_printf(user);
	if(tree_model_hashtable==NULL) {
		logger(INFO, "[add_user_to_chat_list]Hashtable NULL\n");
		return -1;
	}
	tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(chat_id));
	
	if(mod!=NULL) {
		logger(INFO, "[add_user_to_chat_list]Tree model non NULL\n");
		gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 0, msg, -1);
		g_free(msg);
		
		msg = g_strdup_printf(to_string(id));
		//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 1, msg, -1);
		g_free(msg);
		
		msg = g_strdup_printf(to_string((u_int8)status));
		//gtk_list_store_append(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
		gtk_list_store_set(GTK_LIST_STORE(mod->user_model), &(mod->user_iter), 2, msg, -1);
		g_free(msg);
	}
	else
		return -2;
	
	return 0;
}

gint remove_user_from_chat_list(int index, int user_id)
{
  tree_model *mod = (tree_model*)g_hash_table_lookup(tree_model_hashtable,(gconstpointer)to_string(index));
  gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(mod->user_model),&(mod->user_iter),NULL,(gint)user_id);
  gtk_list_store_remove(GTK_LIST_STORE(mod->user_model), &(mod->user_iter));
  return (FALSE);
}

gint clear_chat_list()
{
   gtk_list_store_clear(chat_model);
	return (FALSE);
}

gint clear_buffer(GtkTextView *widget)
{
 GtkTextBuffer *text;
 text = gtk_text_buffer_new(NULL);
 gtk_text_view_set_buffer(GTK_TEXT_VIEW(widget),text);
 return (FALSE);
}

gint add_to_buffer_new_message(GtkTextView *widget, gchar *msg)
{
  GtkTextBuffer *text = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(text,&iter);
  gtk_text_buffer_create_tag(text, "blue_fg", "foreground", "blue", NULL);
  char *first = strstr(msg, "\n");
  int len = first-msg;
  char *tmp = calloc(len+1, 1);
  strncpy(tmp, msg, len);
  gtk_text_buffer_insert_with_tags_by_name(text, &iter, tmp, -1, "blue_fg", "lmarg",  NULL);
  //gtk_text_buffer_insert(text,&iter,tmp,-1);
  msg+=len;
  gtk_text_buffer_get_end_iter(text,&iter),
  gtk_text_buffer_insert(text,&iter,msg,-1); 
  return (FALSE);
}


//EVENTO CHE SI SCATENA ALLA SELEZIONE DELLA CHAT, PER ORA FA SOLO UN PRINTF, in seguito dovrà aprire la gui della chat
gint open_chat(GtkWidget *widget, GdkEventButton *event, gpointer func_data) {
	if (event->type==GDK_2BUTTON_PRESS ||
			event->type==GDK_3BUTTON_PRESS) {
		printf("I feel %s clicked with button %d\n",
				event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
						event->button);
		
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
		GList *lis = gtk_tree_selection_get_selected_rows(selection, &chat_model);
		int i=0;
		u_int8 chat_id;
		for(; i<g_list_length(lis); i++) {
			printf("selected row: %s\n", (gchar*)g_list_nth_data(lis, i));
			//chat_id = atol(
		}
		//printf("size: %d\n", g_list_length(lis));
		//gpointer data = gtk_tree_selection_get_user_data(selection);
		//printf("data: %s\n", (char*)data);
		
		//if(search_chat_local())
		/* prende l'ID della chat e richiede la connessione al controller */
		open_chatroom_gui(1); 
	}

	return FALSE;
}

void view_onRowActivated (GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {
  GtkTreeModel *model;
  GtkTreeIter   iter;

  g_print ("A row has been double-clicked!\n");

  model = gtk_tree_view_get_model(treeview);

  if (gtk_tree_model_get_iter(model, &iter, path))
  {
     gchar *name;

     gtk_tree_model_get(model, &iter, 0, &name, -1);

     g_print ("Double-clicked row contains name %s\n", name);

     chat *elem = get_chat(atoll(name));
     if(elem!=NULL) {
    	 
    	 open_chatroom_gui(elem->id);
		 usleep(200);
		 controller_connect_users(g_hash_table_get_values(elem->users));
		 sleep(1);//FIXIT
		 controller_join_chat(elem->id);
     }
     g_free(name);
  }
}

gint open_conversation(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer userdata) {
	/*if (event->type==GDK_2BUTTON_PRESS ||
			event->type==GDK_3BUTTON_PRESS) {
		printf("I feel %s clicked with button %d\n",
				event->type==GDK_2BUTTON_PRESS ? "double" : "triple",
						event->button);
		open_pm_gui();  
	}*/
  GtkTreeModel *model;
  GtkTreeIter   iter;

  g_print ("A row has been double-clicked!\n");

  model = gtk_tree_view_get_model(treeview);

  if (gtk_tree_model_get_iter(model, &iter, path))
  {
     gchar *user_id;
	 gchar *name; 
	 gtk_tree_model_get(model, &iter, 0, &name, -1);
     gtk_tree_model_get(model, &iter, 1, &user_id, -1);

     g_print ("Double-clicked row contains name %s\n", user_id);

     //chat *elem = get_chat(atoll(name));
     if(user_id > 0 ) {
		//g_hash_table_insert(tree_model_hashtable, (gpointer)user_id, (gpointer)model); //PROVA NELLA STESSA HASHTABLE
    	open_pm_gui(atoll(user_id),name);
    	// open_chatroom_gui(elem->id);
		// usleep(200);
		// controller_connect_users(g_hash_table_get_values(elem->users));
		// sleep(1);//FIXIT
		// controller_join_chat(elem->id);
     }
    // g_free(name);
  }
	return FALSE;
}

gint open_about(GtkWidget *widget, gpointer gdata)
{
    GtkWidget *window;
    GtkWidget *label1;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (window), "delete_event",
		      G_CALLBACK (destroywindow), NULL);
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 25);
    
    label1 = gtk_label_new("\tTorTella Chat\nCreated by TorTella Team\nAll rights reserved 2007/2008");
    gtk_container_add (GTK_CONTAINER (window), label1);
    gtk_widget_show(label1); 
    gtk_widget_show(window);
    gtk_main();
    return 0;
}

gint set_to_online(GtkWidget *widget, gpointer gdata)
{
	g_print("Online...\n");
	//controller_change_status (0);
	return(FALSE);
}

gint set_to_busy(GtkWidget *widget, gpointer gdata)
{
	g_print("Busy...\n");
	//controller_change_status(1);
	return (FALSE);
}

gint set_to_away(GtkWidget *widget, gpointer gdata)
{
	g_print("Away...\n");
	//controller_change_status(2);
	return (FALSE);
}

gint search_chat_button(GtkWidget *widget, gpointer gdata)
{
	
	g_print("Search Chat: %s\n", gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	controller_search(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	clear_chat_list();
	GList *chats;
	int i=0, counter=6;
	chat *chat_val;
	while(counter--) { //TODO: trovare metodo migliore di stampa risultati
		chats = search_all_local_chat(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
		for(; i<g_list_length(chats); i++) {
			chat_val = (chat*)g_list_nth_data(chats, i);
			add_chat_to_list(chat_val->id, chat_val->title);
			
		}
		usleep(200);
	}
	return(FALSE);
}

gint create_chat_button(GtkWidget *widget, gpointer gdata) {
	g_print("Create Chat: %s\n", gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
	return controller_create(gtk_entry_get_text(GTK_ENTRY(bar_textfield)));
}

gint send_text_message(GtkWidget *widget, GdkEventKey *event, gpointer gdata)
{

	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {
		g_print("Enter pressed...\n");
		//Controlla se sono selezionati utenti a cui inviare
		//...
		//Altrimenti invia a tutti
		u_int8 chat_id = atoll((char*)gdata);
		GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		g_print("Enter pressed...\n");
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		g_print("Enter pressed...\n");
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		g_print("Enter pressed...\n");
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
		logger(INFO, "[send_text_message]Msg: %s to %lld\n", msg, chat_id);
		if(controller_send_chat_users(chat_id, strlen(msg), msg)>=0) {
			clear_buffer(GTK_TEXT_VIEW(widget));
			time_t actual_time = time(NULL);
			char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
			add_msg_to_chat(chat_id, send_msg);
			//tree_model *model_str = get_tree_model(chat_id);
			//add_to_buffer_new_message(GTK_TEXT_VIEW(model_str->text_area), msg);
		}
		return TRUE;
	}
	return (FALSE);
}

gint send_pm_message(GtkWidget *widget, GdkEventKey *event, gpointer gdata) //Prova per il pm, dà seg fault
{

	if(event->type == GDK_KEY_PRESS && event->keyval == GDK_Return) {
		g_print("Enter pressed...\n");
		//Controlla se sono selezionati utenti a cui inviare
		//...
		//Altrimenti invia a tutti
		u_int8 chat_id = atoll((char*)gdata);
		GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
		g_print("Enter pressed...\n");
		GtkTextIter start ;
		gtk_text_buffer_get_start_iter(buf, &start);
		g_print("Enter pressed...\n");
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(buf, &end);
		g_print("Enter pressed...\n");
		char *msg = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
		logger(INFO, "[send_pm_message]Msg: %s to %lld\n", msg, chat_id);
		if(controller_send_pm(strlen(msg), msg, chat_id)>=0) { //qui c'è il seg_fault, l'hash table è null
			clear_buffer(GTK_TEXT_VIEW(widget));
			time_t actual_time = time(NULL);
			char *send_msg = prepare_msg(actual_time, servent_get_local()->nick, msg, strlen(msg));
			add_msg_to_chat(chat_id, send_msg); //sostituire con la text view corretta
			//tree_model *model_str = get_tree_model(chat_id);
			//add_to_buffer_new_message(GTK_TEXT_VIEW(model_str->text_area), msg);
		}
		return TRUE;
	}
	return (FALSE);
}

int add_msg_to_chat(u_int8 chat_id, char *msg) {
	tree_model *model_str = get_tree_model(chat_id);
	if(model_str==NULL)
		return -1;
	add_to_buffer_new_message(GTK_TEXT_VIEW(model_str->text_area), msg);
	return 0;
}

tree_model *get_tree_model(u_int8 chat_id) {
	return (tree_model*)g_hash_table_lookup(tree_model_hashtable, to_string(chat_id));
}

GtkWidget *create_users_list(u_int8 index )
{

    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    GtkListStore *model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
   // GtkTreeIter iter;
    GtkCellRenderer *cell;
    //GtkTreeViewColumn *column[3];
    tree_model *model_str = calloc(1, sizeof(tree_model));
    model_str->user_model = model;
    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, 
				    GTK_POLICY_AUTOMATIC);
   
   // model = gtk_list_store_new (1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
                                           tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
    gtk_widget_show (tree_view);
   
    /* Add some messages to the window */
    /*for (i = 0; i < 10; i++) {
        
        gchar *msg = g_strdup_printf ("User #%d", i);
        gtk_list_store_append (GTK_LIST_STORE (model_str->user_model), &(model_str->user_iter));
        gtk_list_store_set (GTK_LIST_STORE (model_str->user_model), 
	                    &(model_str->user_iter),
                            0, msg,
	                    -1);
	g_free (msg);
    }*/
   
    cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Nickname", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	 gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 1, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Status", cell, "text", 2, NULL);
    
	GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

   // gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[0]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[1]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[2]));
   
     g_signal_connect(G_OBJECT(tree_view),
                     "row-activated",
                     G_CALLBACK(open_conversation),
                     to_string(index));  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
   if(tree_model_hashtable == NULL) {
		logger(INFO, "[create_users_list] Hashtable created\n");
	   tree_model_hashtable = g_hash_table_new(g_str_hash, g_str_equal);
   }

   logger(INFO, "[create_users_list]chat ID: %lld\n", index);
   g_hash_table_insert(tree_model_hashtable, (gpointer)to_string(index), (gpointer)model_str);

   return scrolled_window;
}

GtkWidget *create_chat_list(u_int8 index )
{

    GtkWidget *scrolled_window;
    GtkWidget *tree_view;
    GtkCellRenderer *cell;
   // GtkTreeViewColumn *column[2];

    int i;
   
    /* Create a new scrolled window, with scrollbars only if needed */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				    GTK_POLICY_AUTOMATIC, 
				    GTK_POLICY_AUTOMATIC);
   
    chat_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    tree_view = gtk_tree_view_new ();
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
                                           tree_view);
    gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (chat_model));
    gtk_widget_show (tree_view);
   
    /* Add some messages to the window */
    /*for (i = 0; i < 10; i++) {
        
        gchar *msg = g_strdup_printf ("Chat #%d", i);
        gtk_list_store_append (GTK_LIST_STORE (chat_model), &chat_iter);
        gtk_list_store_set (GTK_LIST_STORE (chat_model), 
	                    &chat_iter,
                            0, msg,
	                    -1);
	g_free (msg);
    }*/
   
    cell = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "ID", cell, "text", 0, NULL);
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(tree_view), -1, "Title", cell, "text", 1, NULL);
   // GtkTreeSelection *select;
 //   select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
  //  gtk_tree_selection_set_mode(select, GTK_SELECTION_MULTIPLE);

    //gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[0]));
	//gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), GTK_TREE_VIEW_COLUMN (column[1]));
//    g_signal_connect(G_OBJECT(tree_view),
//                     "button_press_event",
//                     G_CALLBACK(open_chat),
//                     NULL);  //EVENTO CHE AL DOPPIO CLICK SU UNA CHAT DOVREBBE APRIRE LA GUI DELLA CHAT SELEZIONATA
//
    g_signal_connect(tree_view, "row-activated", (GCallback) view_onRowActivated, NULL);

   return scrolled_window;
}

GtkWidget *create_menu(void) {
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
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (set_to_online), NULL);
	gtk_widget_show (menuitem);

	/*-- Create Copy menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Busy");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (set_to_busy), NULL);
	gtk_widget_show (menuitem);

	/*-- Create Cut menu item under File submenu --*/
	menuitem = gtk_menu_item_new_with_label ("Away");
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (set_to_away), NULL);
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
	gtk_signal_connect(GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (open_about), NULL);
	gtk_widget_show (menuitem);
	/*---------------- End Help menu declarations ----------------*/
	return menubar;
}

GtkWidget *create_text(u_int8 chat_id, int type, int msg_type)
{
	logger(INFO, "[create_text]chat ID: %lld\n", chat_id);
	GtkWidget *scrolled_window;
	GtkWidget *view = gtk_text_view_new ();
	GtkTextBuffer *buffer;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	// gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window),gtk_adjustment_new(0,0,1,1,1)); //NON FUNZIONA
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);

	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	 
	//insert_text (buffer);
    if(type == BOTTOM) {
    	gtk_text_view_set_editable(GTK_TEXT_VIEW(view),TRUE);
    	logger(INFO, "[create_text]signal connect\n");
		if(msg_type == PM) {
			g_signal_connect(GTK_OBJECT(view),"key_press_event", G_CALLBACK(send_pm_message), (gpointer)to_string(chat_id)); //prova prova prova di pm
		}
		else if(msg_type == CHAT) {	
			g_signal_connect(GTK_OBJECT(view),"key_press_event", G_CALLBACK(send_text_message), (gpointer)to_string(chat_id)); //TODO blocca l'inserimento testo
		}
		gtk_text_view_set_editable(GTK_TEXT_VIEW(view),TRUE);
    }
    else if (type == TOP){
		if(msg_type == CHAT) {
    		gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);
    		tree_model *model_str = (tree_model*)g_hash_table_lookup(tree_model_hashtable, to_string(chat_id));
    		if(model_str!=NULL) {
    			logger(INFO, "[create_text]OK\n");
    			model_str->text_area = GTK_TEXT_VIEW(view);
    		}
    		else {
    			logger(INFO, "[create_text]NULL\n");
    			return NULL;
    		}
    	}
		else if(msg_type == PM) {
			//gestire la text area del pm
		}
	}	
	gtk_widget_show_all (scrolled_window);

	return scrolled_window;
}

int open_chatroom_gui(u_int8 chat_id) {
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
	
	menu = create_menu();
	list = create_users_list (chat_id);
	
	text = create_text(chat_id, TOP, CHAT);
	chat = create_text(chat_id, BOTTOM, CHAT);

	/*-- Create the vbox --*/
	vbox = gtk_vbox_new(FALSE, 5);

	hbox = gtk_hbox_new(FALSE, 5);
	vbox2 = gtk_vbox_new(FALSE, 5);
	/*-- Create a text area --*/
	
	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();


	/*-- Connect the window to the destroyapp function  --*/
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroywindow), NULL);


	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */ 

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(vbox),hbox);
	
	gtk_container_add(GTK_CONTAINER(hbox),vbox2);
	gtk_container_add (GTK_CONTAINER (hbox), list);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	/*-- Add the vbox to the main window --*/
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/*-- Add some text to the window --*/
	//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

	/*-- Set window border to zero so that text area takes up the whole window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);

	/*-- Set the window to be 640 x 480 pixels --*/
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);

	/*-- Set the window title --*/
	gtk_window_set_title(GTK_WINDOW (window), "Chat Room");
	logger(INFO, "[open_chatroom_gui]chat_id created: %lld\n", chat_id);

	/*-- Display the widgets --*/
	gtk_widget_show(handlebox);
	gtk_widget_show(hbox);
	gtk_widget_show(vbox);
	gtk_widget_show(vbox2);
	gtk_widget_show(text);
	gtk_widget_show(list);
	gtk_widget_show(chat);
	gtk_widget_show(menu);
	gtk_widget_show(window);
	
	/*-- Start the GTK event loop --*/
	//gtk_main();

	/*-- Return 0 if exit is successful --*/
	return 0;
}

int open_pm_gui(u_int8 user_id, gchar *nickname) {
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
	vbox = gtk_vbox_new(FALSE, 5);


	vbox2 = gtk_vbox_new(FALSE, 5);
	//gtk_paned_set_position(GTK_PANED(vbox2), 500);  //TROVARE UN MODO CORRETTO DI SETTARE IL SECONDO PARAMETRO PER POSIZIONARE BENE IL SEPARATORE
	/*-- Create a text area --*/
	text = create_text(user_id, TOP, PM);
	chat = create_text(user_id, BOTTOM, PM); //Naturalmente non si vede nella window xkè l'id della chat non è 1

	/*-- Create the handlebox --*/
	handlebox = gtk_handle_box_new();

	/*-- Create the menu bar --*/
	menu = create_menu();

	/*-- Connect the window to the destroyapp function  --*/
	gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(destroywindow), NULL);


	/*-- Add the menubar to the handlebox --*/
	gtk_container_add(GTK_CONTAINER(handlebox), menu);

	/*-- Pack the handlebox into the top of the vbox.  
	 *-- You must use gtk_box_pack_start here instead of gtk_container_add
	 *-- or the menu will get larger as the window is enlarged
	 */

	//gtk_box_pack_start(GTK_BOX(hbox), handlebox, FALSE, TRUE, 0);    

	gtk_box_pack_start(GTK_BOX(vbox), handlebox, FALSE, TRUE, 0);    
	gtk_container_add(GTK_CONTAINER(vbox),vbox2);
	
	//gtk_paned_add1(GTK_PANED(vbox2), chat);
	//gtk_paned_add2(GTK_PANED(vbox2), text);
	gtk_container_add(GTK_CONTAINER(vbox2),text);
	gtk_box_pack_end(GTK_BOX(vbox2), chat, FALSE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/*-- Add some text to the window --*/
	//  gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, buffer, strlen(buffer));

	/*-- Set window border to zero so that text area takes up the whole window --*/
	gtk_container_border_width (GTK_CONTAINER (window), 0);

	/*-- Set the window to be 640 x 480 pixels --*/
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 400);

	/*-- Set the window title --*/
	gtk_window_set_title(GTK_WINDOW (window), nickname);

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

	bar_textfield = gtk_entry_new();
	GtkWidget *bar_button = gtk_button_new();
	GtkWidget *bar_create_button = gtk_button_new();

	gtk_entry_set_width_chars(GTK_ENTRY(bar_textfield), (gint)40);
	gtk_button_set_label(GTK_BUTTON(bar_button), (const gchar*)"Search");
     g_signal_connect (G_OBJECT (bar_button), "clicked",
		      G_CALLBACK (search_chat_button), NULL);
     
     gtk_button_set_label(GTK_BUTTON(bar_create_button), (const gchar*)"Create");
     g_signal_connect(G_OBJECT(bar_create_button), "clicked", G_CALLBACK(create_chat_button), NULL);
     
	gtk_container_add(GTK_CONTAINER(bar_container), bar_textfield);
	//gtk_box_pack_start(GTK_BOX(bar_container), bar_textfield, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_button, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(bar_container), bar_create_button, FALSE, TRUE, 5);

	gtk_widget_show(bar_create_button);
	gtk_widget_show(bar_textfield);
	gtk_widget_show(bar_button);

	return bar_container;
}

