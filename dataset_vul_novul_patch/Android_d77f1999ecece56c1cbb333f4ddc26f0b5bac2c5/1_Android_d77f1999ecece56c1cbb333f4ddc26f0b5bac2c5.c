 void btif_config_flush(void) {
  assert(config != NULL);
  assert(alarm_timer != NULL);

 
   alarm_cancel(alarm_timer);
 
  pthread_mutex_lock(&lock);
  config_save(config, CONFIG_FILE_PATH);
  pthread_mutex_unlock(&lock);
 }
