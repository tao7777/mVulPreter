static future_t *init(void) {
  pthread_mutex_init(&lock, NULL);
  config = config_new(CONFIG_FILE_PATH);
 if (!config) {
    LOG_WARN(LOG_TAG, "%s unable to load config file; attempting to transcode legacy file.", __func__);
    config = btif_config_transcode(LEGACY_CONFIG_FILE_PATH);
 if (!config) {
      LOG_WARN(LOG_TAG, "%s unable to transcode legacy file, starting unconfigured.", __func__);
      config = config_new_empty();
 if (!config) {
        LOG_ERROR(LOG_TAG, "%s unable to allocate a config object.", __func__);
 goto error;
 }
 }

 if (config_save(config, CONFIG_FILE_PATH))

       unlink(LEGACY_CONFIG_FILE_PATH);
   }
 
  btif_config_devcache_cleanup();

  alarm_timer = alarm_new();
 if (!alarm_timer) {
    LOG_ERROR(LOG_TAG, "%s unable to create alarm.", __func__);
 goto error;
 }

 return future_new_immediate(FUTURE_SUCCESS);

error:;
  alarm_free(alarm_timer);
  config_free(config);
  pthread_mutex_destroy(&lock);
  alarm_timer = NULL;
  config = NULL;
 return future_new_immediate(FUTURE_FAIL);
}
