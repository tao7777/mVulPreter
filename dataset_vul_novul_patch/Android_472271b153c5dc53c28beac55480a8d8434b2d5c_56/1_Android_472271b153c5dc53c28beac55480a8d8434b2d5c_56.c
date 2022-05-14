bool config_save(const config_t *config, const char *filename) {
  assert(config != NULL);

   assert(filename != NULL);
   assert(*filename != '\0');
 
  char *temp_filename = osi_calloc(strlen(filename) + 5);
  if (!temp_filename) {
    LOG_ERROR("%s unable to allocate memory for filename.", __func__);
    return false;
   }
 
  strcpy(temp_filename, filename);
  strcat(temp_filename, ".new");
 
  FILE *fp = fopen(temp_filename, "wt");
   if (!fp) {
     LOG_ERROR("%s unable to write file '%s': %s", __func__, temp_filename, strerror(errno));
     goto error;
 }

 
   for (const list_node_t *node = list_begin(config->sections); node != list_end(config->sections); node = list_next(node)) {
     const section_t *section = (const section_t *)list_node(node);
    fprintf(fp, "[%s]\n", section->name);
 
     for (const list_node_t *enode = list_begin(section->entries); enode != list_end(section->entries); enode = list_next(enode)) {
       const entry_t *entry = (const entry_t *)list_node(enode);
      fprintf(fp, "%s = %s\n", entry->key, entry->value);
     }
 
    if (list_next(node) != list_end(config->sections))
      fputc('\n', fp);
   }
 
  fflush(fp);
  fclose(fp);
 
   if (chmod(temp_filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {
    LOG_ERROR("%s unable to change file permissions '%s': %s", __func__, filename, strerror(errno));

     goto error;
   }
 
   if (rename(temp_filename, filename) == -1) {
     LOG_ERROR("%s unable to commit file '%s': %s", __func__, filename, strerror(errno));
     goto error;
   }
 
   osi_free(temp_filename);
   return true;
 
error:;
   unlink(temp_filename);
   osi_free(temp_filename);
   return false;
 }
