static void on_page_prepare(GtkNotebook *assistant, GtkWidget *page, gpointer user_data)
{

    /* This suppresses [Last] button: assistant thinks that
     * we never have this page ready unless we are on it
     * -> therefore there is at least one non-ready page
     * -> therefore it won't show [Last]
     */

    /* If processing is finished and if it was terminated because of an error
     * the event progress page is selected. So, it does not make sense to show
     * the next step button and we MUST NOT clear warnings.
     */
    if (!is_processing_finished())
    {
        /* some pages hide it, so restore it to it's default */
        show_next_step_button();
        clear_warnings();
    }

    gtk_widget_hide(g_btn_detail);
    gtk_widget_hide(g_btn_onfail);
    if (!g_expert_mode)
        gtk_widget_hide(g_btn_repeat);
    /* Save text fields if changed */
    /* Must be called before any GUI operation because the following two
     * functions causes recreating of the text items tabs, thus all updates to
      * these tabs will be lost */
     save_items_from_notepad();
     save_text_from_text_view(g_tv_comment, FILENAME_COMMENT);
 
     if (pages[PAGENO_SUMMARY].page_widget == page)
     {
        if (!g_expert_mode)
        {
            /* Skip intro screen */
            int n = select_next_page_no(pages[PAGENO_SUMMARY].page_no, NULL);
            log_info("switching to page_no:%d", n);
            gtk_notebook_set_current_page(assistant, n);
            return;
        }
    }

    if (pages[PAGENO_EDIT_ELEMENTS].page_widget == page)
    {
        if (highlight_forbidden())
        {
            add_sensitive_data_warning();
            show_warnings();
            gtk_expander_set_expanded(g_exp_search, TRUE);
        }
        else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_rb_custom_search), TRUE);

        show_warnings();
    }

    if (pages[PAGENO_REVIEW_DATA].page_widget == page)
    {
        update_ls_details_checkboxes(g_event_selected);
        gtk_widget_set_sensitive(g_btn_next, gtk_toggle_button_get_active(g_tb_approve_bt));
    }

    if (pages[PAGENO_EDIT_COMMENT].page_widget == page)
    {
        gtk_widget_show(g_btn_detail);
        gtk_widget_set_sensitive(g_btn_next, false);
        on_comment_changed(gtk_text_view_get_buffer(g_tv_comment), NULL);
    }

    if (pages[PAGENO_EVENT_PROGRESS].page_widget == page)
    {
        log_info("g_event_selected:'%s'", g_event_selected);
        if (g_event_selected
         && g_event_selected[0]
        ) {
            clear_warnings();
            start_event_run(g_event_selected);
        }
    }

    if(pages[PAGENO_EVENT_SELECTOR].page_widget == page)
    {
        if (!g_expert_mode && !g_auto_event_list)
            hide_next_step_button();
    }
}
