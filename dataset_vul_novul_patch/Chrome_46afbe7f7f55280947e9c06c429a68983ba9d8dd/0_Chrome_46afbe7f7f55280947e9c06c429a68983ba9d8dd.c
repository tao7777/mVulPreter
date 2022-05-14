 elm_main(int argc, char *argv[])
 {
     int args = 1;
     unsigned char quitOption = 0;
     Browser_Window *window;
    char *window_size_string = NULL;
 
     Ecore_Getopt_Value values[] = {
         ECORE_GETOPT_VALUE_STR(evas_engine_name),
        ECORE_GETOPT_VALUE_STR(window_size_string),
         ECORE_GETOPT_VALUE_BOOL(quitOption),
         ECORE_GETOPT_VALUE_BOOL(frame_flattening_enabled),
         ECORE_GETOPT_VALUE_BOOL(quitOption),
        ECORE_GETOPT_VALUE_BOOL(quitOption),
        ECORE_GETOPT_VALUE_BOOL(quitOption),
        ECORE_GETOPT_VALUE_NONE
    };

    if (!ewk_init())
        return EXIT_FAILURE;

    ewk_view_smart_class_set(miniBrowserViewSmartClass());

    ecore_app_args_set(argc, (const char **) argv);
    args = ecore_getopt_parse(&options, values, argc, argv);

    if (args < 0)
        return quit(EINA_FALSE, "ERROR: could not parse options.\n");

    if (quitOption)
        return quit(EINA_TRUE, NULL);

    if (evas_engine_name)
        elm_config_preferred_engine_set(evas_engine_name);
#if defined(WTF_USE_ACCELERATED_COMPOSITING) && defined(HAVE_ECORE_X)
    else {
        evas_engine_name = "opengl_x11";
        elm_config_preferred_engine_set(evas_engine_name);
    }
#endif

     Ewk_Context *context = ewk_context_default_get();
     ewk_context_favicon_database_directory_set(context, NULL);
 
    if (window_size_string)
        parse_window_size(window_size_string, &window_width, &window_height);

     if (args < argc) {
         char *url = url_from_user_input(argv[args]);
         window = window_create(url);
        free(url);
    } else
        window = window_create(DEFAULT_URL);

    if (!window)
        return quit(EINA_FALSE, "ERROR: could not create browser window.\n");

    windows = eina_list_append(windows, window);

    elm_run();

    return quit(EINA_TRUE, NULL);
}
