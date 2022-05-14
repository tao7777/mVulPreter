static void timer_config_save(UNUSED_ATTR void *data) {
   assert(config != NULL);
   assert(alarm_timer != NULL);
 
  static const size_t CACHE_MAX = 256;
  const char *keys[CACHE_MAX];
  size_t num_keys = 0;
  size_t total_candidates = 0;
 
   pthread_mutex_lock(&lock);
  for (const config_section_node_t *snode = config_section_begin(config); snode != config_section_end(config); snode = config_section_next(snode)) {
    const char *section = config_section_name(snode);
    if (!string_is_bdaddr(section))
      continue;
    if (config_has_key(config, section, "LinkKey") ||
        config_has_key(config, section, "LE_KEY_PENC") ||
        config_has_key(config, section, "LE_KEY_PID") ||
        config_has_key(config, section, "LE_KEY_PCSRK") ||
        config_has_key(config, section, "LE_KEY_LENC") ||
        config_has_key(config, section, "LE_KEY_LCSRK"))
      continue;
    if (num_keys < CACHE_MAX)
      keys[num_keys++] = section;
    ++total_candidates;
  }
  if (total_candidates > CACHE_MAX * 2)
    while (num_keys > 0)
      config_remove_section(config, keys[--num_keys]);
   config_save(config, CONFIG_FILE_PATH);
   pthread_mutex_unlock(&lock);
 }
