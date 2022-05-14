static void tv_details_row_activated(
                        GtkTreeView       *tree_view,
                        GtkTreePath       *tree_path_UNUSED,
                        GtkTreeViewColumn *column,
                        gpointer           user_data)
{
    gchar *item_name;
    struct problem_item *item = get_current_problem_item_or_NULL(tree_view, &item_name);
    if (!item || !(item->flags & CD_FLAG_TXT))
        goto ret;
    if (!strchr(item->content, '\n')) /* one line? */
        goto ret; /* yes */

    gint exitcode;
    gchar *arg[3];
    arg[0] = (char *) "xdg-open";
    arg[1] = concat_path_file(g_dump_dir_name, item_name);
    arg[2] = NULL;

    const gboolean spawn_ret = g_spawn_sync(NULL, arg, NULL,
                                 G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL,
                                 NULL, NULL, NULL, NULL, &exitcode, NULL);

    if (spawn_ret == FALSE || exitcode != EXIT_SUCCESS)
    {
        GtkWidget *dialog = gtk_dialog_new_with_buttons(_("View/edit a text file"),
            GTK_WINDOW(g_wnd_assistant),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            NULL, NULL);
        GtkWidget *vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
        GtkWidget *textview = gtk_text_view_new();

        gtk_dialog_add_button(GTK_DIALOG(dialog), _("_Save"), GTK_RESPONSE_OK);
        gtk_dialog_add_button(GTK_DIALOG(dialog), _("_Cancel"), GTK_RESPONSE_CANCEL);

        gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
        gtk_widget_set_size_request(scrolled, 640, 480);
        gtk_widget_show(scrolled);

#if ((GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION < 7) || (GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION == 7 && GTK_MICRO_VERSION < 8))
        /* http://developer.gnome.org/gtk3/unstable/GtkScrolledWindow.html#gtk-scrolled-window-add-with-viewport */
        /* gtk_scrolled_window_add_with_viewport has been deprecated since version 3.8 and should not be used in newly-written code. */
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), textview);
#else
        /* gtk_container_add() will now automatically add a GtkViewport if the child doesn't implement GtkScrollable. */
        gtk_container_add(GTK_CONTAINER(scrolled), textview);
#endif

        gtk_widget_show(textview);

         load_text_to_text_view(GTK_TEXT_VIEW(textview), item_name);
 
         if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
             save_text_from_text_view(GTK_TEXT_VIEW(textview), item_name);
 
         gtk_widget_destroy(textview);
         gtk_widget_destroy(scrolled);
        gtk_widget_destroy(dialog);
    }

    free(arg[1]);
 ret:
    g_free(item_name);
}
