void btif_config_save(void) {

   assert(alarm_timer != NULL);
   assert(config != NULL);
 
  alarm_set(alarm_timer, CONFIG_SETTLE_PERIOD_MS, timer_config_save, NULL);
 }
