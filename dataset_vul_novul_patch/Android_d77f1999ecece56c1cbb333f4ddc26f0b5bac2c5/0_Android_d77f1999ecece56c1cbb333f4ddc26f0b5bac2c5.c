 void btif_config_flush(void) {
  assert(config != NULL);
  assert(alarm_timer != NULL);

 
   alarm_cancel(alarm_timer);
 
  btif_config_write();
 }
