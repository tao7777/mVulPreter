int main(int argc, char **argv) {
 const char *test_name = NULL;
 bool skip_sanity_suite = false;

 for (int i = 1; i < argc; ++i) {
 if (!strcmp("--help", argv[i])) {
      print_usage(argv[0]);
 return 0;
 }

 if (!strcmp("--insanity", argv[i])) {
      skip_sanity_suite = true;
 continue;
 }

 if (!is_valid(argv[i])) {
      printf("Error: invalid test name.\n");
      print_usage(argv[0]);
 return -1;
 }

 if (test_name != NULL) {
      printf("Error: invalid arguments.\n");
      print_usage(argv[0]);
 return -1;
 }

    test_name = argv[i];
 }

 if (is_shell_running()) {
    printf("Run 'adb shell stop' before running %s.\n", argv[0]);
 return -1;
 }

 config_t *config = config_new(CONFIG_FILE_PATH);
 if (!config) {
    printf("Error: unable to open stack config file.\n");
    print_usage(argv[0]);
 return -1;
 }

 for (const config_section_node_t *node = config_section_begin(config); node != config_section_end(config); node = config_section_next(node)) {
 const char *name = config_section_name(node);
 if (config_has_key(config, name, "LinkKey") && string_to_bdaddr(name, &bt_remote_bdaddr)) {
 break;
 }
 }

  config_free(config);

 if (bdaddr_is_empty(&bt_remote_bdaddr)) {
    printf("Error: unable to find paired device in config file.\n");
    print_usage(argv[0]);
 return -1;
 }

 if (!hal_open(callbacks_get_adapter_struct())) {
    printf("Unable to open Bluetooth HAL.\n");
 return 1;
 }

 if (!btsocket_init()) {
    printf("Unable to initialize Bluetooth sockets.\n");
 return 2;
 }

 if (!pan_init()) {
    printf("Unable to initialize PAN.\n");
 return 3;
 }

 if (!gatt_init()) {
    printf("Unable to initialize GATT.\n");
 return 4;
 }

  watchdog_running = true;
  pthread_create(&watchdog_thread, NULL, watchdog_fn, NULL);

 static const char *DEFAULT  = "\x1b[0m";
 static const char *GREEN = "\x1b[0;32m";
 static const char *RED   = "\x1b[0;31m";

 if (!isatty(fileno(stdout))) {
    DEFAULT = GREEN = RED = "";
 }

 int pass = 0;
 int fail = 0;
 int case_num = 0;

 if (!skip_sanity_suite) {
 for (size_t i = 0; i < sanity_suite_size; ++i) {
 if (!test_name || !strcmp(test_name, sanity_suite[i].function_name)) {
        callbacks_init();
 if (sanity_suite[i].function()) {
          printf("[%4d] %-64s [%sPASS%s]\n", ++case_num, sanity_suite[i].function_name, GREEN, DEFAULT);
 ++pass;
 } else {
          printf("[%4d] %-64s [%sFAIL%s]\n", ++case_num, sanity_suite[i].function_name, RED, DEFAULT);
 ++fail;
 }
        callbacks_cleanup();
 ++watchdog_id;
 }
 }
 }

 if (fail) {
    printf("\n%sSanity suite failed with %d errors.%s\n", RED, fail, DEFAULT);
    hal_close();
 return 4;
 }


   for (size_t i = 0; i < test_suite_size; ++i) {
     if (!test_name || !strcmp(test_name, test_suite[i].function_name)) {
       callbacks_init();
      CALL_AND_WAIT(bt_interface->enable(false), adapter_state_changed);
       if (test_suite[i].function()) {
         printf("[%4d] %-64s [%sPASS%s]\n", ++case_num, test_suite[i].function_name, GREEN, DEFAULT);
         ++pass;
 } else {
        printf("[%4d] %-64s [%sFAIL%s]\n", ++case_num, test_suite[i].function_name, RED, DEFAULT);
 ++fail;
 }
      CALL_AND_WAIT(bt_interface->disable(), adapter_state_changed);
      callbacks_cleanup();
 ++watchdog_id;
 }
 }

  printf("\n");

 if (fail) {
    printf("%d/%d tests failed. See above for failed test cases.\n", fail, sanity_suite_size + test_suite_size);
 } else {
    printf("All tests passed!\n");
 }

  watchdog_running = false;
  pthread_join(watchdog_thread, NULL);

  hal_close();

 return 0;
}
