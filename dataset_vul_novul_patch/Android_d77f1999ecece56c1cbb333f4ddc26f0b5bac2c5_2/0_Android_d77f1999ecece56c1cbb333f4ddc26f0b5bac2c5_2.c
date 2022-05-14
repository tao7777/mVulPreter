static void timer_config_save(UNUSED_ATTR void *data) {
static void timer_config_save_cb(UNUSED_ATTR void *data) {
  btif_config_write();
}

static void btif_config_write(void) {
   assert(config != NULL);
   assert(alarm_timer != NULL);
 
  btif_config_devcache_cleanup();
 
   pthread_mutex_lock(&lock);
   config_save(config, CONFIG_FILE_PATH);
   pthread_mutex_unlock(&lock);
 }
