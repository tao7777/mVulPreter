static Browser_Window *window_create(const char *url)
{
    Browser_Window *app_data = malloc(sizeof(Browser_Window));
    if (!app_data) {
        info("ERROR: could not create browser window.\n");
        return NULL;
    }

    /* Create window */
    app_data->window = elm_win_add(NULL, "minibrowser-window", ELM_WIN_BASIC);
    elm_win_title_set(app_data->window, APP_NAME);
    evas_object_smart_callback_add(app_data->window, "delete,request", on_window_deletion, &app_data);

    /* Create window background */
    Evas_Object *bg = elm_bg_add(app_data->window);
    elm_bg_color_set(bg, 193, 192, 191);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(app_data->window, bg);
    evas_object_show(bg);

    /* Create vertical layout */
    Evas_Object *vertical_layout = elm_box_add(app_data->window);
    elm_box_padding_set(vertical_layout, 0, 2);
    evas_object_size_hint_weight_set(vertical_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(app_data->window, vertical_layout);
    evas_object_show(vertical_layout);

    /* Create horizontal layout for top bar */
    Evas_Object *horizontal_layout = elm_box_add(app_data->window);
    elm_box_horizontal_set(horizontal_layout, EINA_TRUE);
    evas_object_size_hint_weight_set(horizontal_layout, EVAS_HINT_EXPAND, 0.0);
    evas_object_size_hint_align_set(horizontal_layout, EVAS_HINT_FILL, 0.0);
    elm_box_pack_end(vertical_layout, horizontal_layout);
    evas_object_show(horizontal_layout);

    /* Create Back button */
    app_data->back_button = create_toolbar_button(app_data->window, "arrow_left");
    evas_object_smart_callback_add(app_data->back_button, "clicked", on_back_button_clicked, app_data);
    elm_object_disabled_set(app_data->back_button, EINA_TRUE);
    evas_object_size_hint_weight_set(app_data->back_button, 0.0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(app_data->back_button, 0.0, 0.5);
    elm_box_pack_end(horizontal_layout, app_data->back_button);
    evas_object_show(app_data->back_button);

    /* Create Forward button */
    app_data->forward_button = create_toolbar_button(app_data->window, "arrow_right");
    evas_object_smart_callback_add(app_data->forward_button, "clicked", on_forward_button_clicked, app_data);
    elm_object_disabled_set(app_data->forward_button, EINA_TRUE);
    evas_object_size_hint_weight_set(app_data->forward_button, 0.0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(app_data->forward_button, 0.0, 0.5);
    elm_box_pack_end(horizontal_layout, app_data->forward_button);
    evas_object_show(app_data->forward_button);

    /* Create URL bar */
    app_data->url_bar = elm_entry_add(app_data->window);
    elm_entry_scrollable_set(app_data->url_bar, EINA_TRUE);
    elm_entry_scrollbar_policy_set(app_data->url_bar, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
    elm_entry_single_line_set(app_data->url_bar, EINA_TRUE);
    elm_entry_cnp_mode_set(app_data->url_bar, ELM_CNP_MODE_PLAINTEXT);
    elm_entry_text_style_user_push(app_data->url_bar, "DEFAULT='font_size=18'");
    evas_object_smart_callback_add(app_data->url_bar, "activated", on_url_bar_activated, app_data);
    evas_object_smart_callback_add(app_data->url_bar, "clicked", on_url_bar_clicked, app_data);
    evas_object_size_hint_weight_set(app_data->url_bar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(app_data->url_bar, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(horizontal_layout, app_data->url_bar);
    evas_object_show(app_data->url_bar);

    /* Create Refresh button */
    Evas_Object *refresh_button = create_toolbar_button(app_data->window, "refresh");
    evas_object_smart_callback_add(refresh_button, "clicked", on_refresh_button_clicked, app_data);
    evas_object_size_hint_weight_set(refresh_button, 0.0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(refresh_button, 1.0, 0.5);
    elm_box_pack_end(horizontal_layout, refresh_button);
    evas_object_show(refresh_button);

    /* Create Home button */
    Evas_Object *home_button = create_toolbar_button(app_data->window, "home");
    evas_object_smart_callback_add(home_button, "clicked", on_home_button_clicked, app_data);
    evas_object_size_hint_weight_set(home_button, 0.0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(home_button, 1.0, 0.5);
    elm_box_pack_end(horizontal_layout, home_button);
    evas_object_show(home_button);

    /* Create webview */
    Ewk_View_Smart_Class *ewkViewClass = miniBrowserViewSmartClass();
    ewkViewClass->run_javascript_alert = on_javascript_alert;
    ewkViewClass->run_javascript_confirm = on_javascript_confirm;
    ewkViewClass->run_javascript_prompt = on_javascript_prompt;

    Evas *evas = evas_object_evas_get(app_data->window);
    Evas_Smart *smart = evas_smart_class_new(&ewkViewClass->sc);
    app_data->webview = ewk_view_smart_add(evas, smart, ewk_context_default_get());
    ewk_view_theme_set(app_data->webview, THEME_DIR "/default.edj");

    Ewk_Settings *settings = ewk_view_settings_get(app_data->webview);
    ewk_settings_file_access_from_file_urls_allowed_set(settings, EINA_TRUE);
    ewk_settings_frame_flattening_enabled_set(settings, frame_flattening_enabled);
    ewk_settings_developer_extras_enabled_set(settings, EINA_TRUE);

    evas_object_smart_callback_add(app_data->webview, "authentication,request", on_authentication_request, app_data);
    evas_object_smart_callback_add(app_data->webview, "close,window", on_close_window, app_data);
    evas_object_smart_callback_add(app_data->webview, "create,window", on_new_window, app_data);
    evas_object_smart_callback_add(app_data->webview, "download,failed", on_download_failed, app_data);
    evas_object_smart_callback_add(app_data->webview, "download,finished", on_download_finished, app_data);
    evas_object_smart_callback_add(app_data->webview, "download,request", on_download_request, app_data);
    evas_object_smart_callback_add(app_data->webview, "file,chooser,request", on_file_chooser_request, app_data);
    evas_object_smart_callback_add(app_data->webview, "icon,changed", on_view_icon_changed, app_data);
    evas_object_smart_callback_add(app_data->webview, "load,error", on_error, app_data);
    evas_object_smart_callback_add(app_data->webview, "load,progress", on_progress, app_data);
    evas_object_smart_callback_add(app_data->webview, "title,changed", on_title_changed, app_data);
    evas_object_smart_callback_add(app_data->webview, "url,changed", on_url_changed, app_data);
    evas_object_smart_callback_add(app_data->webview, "back,forward,list,changed", on_back_forward_list_changed, app_data);
    evas_object_smart_callback_add(app_data->webview, "tooltip,text,set", on_tooltip_text_set, app_data);
    evas_object_smart_callback_add(app_data->webview, "tooltip,text,unset", on_tooltip_text_unset, app_data);

    evas_object_event_callback_add(app_data->webview, EVAS_CALLBACK_KEY_DOWN, on_key_down, app_data);
    evas_object_event_callback_add(app_data->webview, EVAS_CALLBACK_MOUSE_DOWN, on_mouse_down, app_data);

    evas_object_size_hint_weight_set(app_data->webview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(app_data->webview, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(vertical_layout, app_data->webview);
    evas_object_show(app_data->webview);

     if (url)
         ewk_view_url_set(app_data->webview, url);
 
    evas_object_resize(app_data->window, window_width, window_height);
     evas_object_show(app_data->window);
 
     view_focus_set(app_data, EINA_TRUE);
 
     return app_data;
 }
