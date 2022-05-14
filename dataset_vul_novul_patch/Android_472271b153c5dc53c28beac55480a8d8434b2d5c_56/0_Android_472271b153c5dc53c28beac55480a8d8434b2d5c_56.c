bool config_save(const config_t *config, const char *filename) {
  assert(config != NULL);

   assert(filename != NULL);
   assert(*filename != '\0');
 
  // Steps to ensure content of config file gets to disk:
  //
  // 1) Open and write to temp file (e.g. bt_config.conf.new).
  // 2) Sync the temp file to disk with fsync().
  // 3) Rename temp file to actual config file (e.g. bt_config.conf).
  //    This ensures atomic update.
  // 4) Sync directory that has the conf file with fsync().
  //    This ensures directory entries are up-to-date.
  int dir_fd = -1;
  FILE *fp = NULL;

  // Build temp config file based on config file (e.g. bt_config.conf.new).
  static const char *temp_file_ext = ".new";
  const int filename_len = strlen(filename);
  const int temp_filename_len = filename_len + strlen(temp_file_ext) + 1;
  char *temp_filename = osi_calloc(temp_filename_len);
  snprintf(temp_filename, temp_filename_len, "%s%s", filename, temp_file_ext);

  // Extract directory from file path (e.g. /data/misc/bluedroid).
  char *temp_dirname = osi_strdup(filename);
  const char *directoryname = dirname(temp_dirname);
  if (!directoryname) {
    LOG_ERROR("%s error extracting directory from '%s': %s", __func__, filename, strerror(errno));
    goto error;
   }
 
  dir_fd = TEMP_FAILURE_RETRY(open(directoryname, O_RDONLY));
  if (dir_fd < 0) {
    LOG_ERROR("%s unable to open dir '%s': %s", __func__, directoryname, strerror(errno));
    goto error;
  }
 
  fp = fopen(temp_filename, "wt");
   if (!fp) {
     LOG_ERROR("%s unable to write file '%s': %s", __func__, temp_filename, strerror(errno));
     goto error;
 }

 
   for (const list_node_t *node = list_begin(config->sections); node != list_end(config->sections); node = list_next(node)) {
     const section_t *section = (const section_t *)list_node(node);
    if (fprintf(fp, "[%s]\n", section->name) < 0) {
      LOG_ERROR("%s unable to write to file '%s': %s", __func__, temp_filename, strerror(errno));
      goto error;
    }
 
     for (const list_node_t *enode = list_begin(section->entries); enode != list_end(section->entries); enode = list_next(enode)) {
       const entry_t *entry = (const entry_t *)list_node(enode);
      if (fprintf(fp, "%s = %s\n", entry->key, entry->value) < 0) {
        LOG_ERROR("%s unable to write to file '%s': %s", __func__, temp_filename, strerror(errno));
        goto error;
      }
     }
 
    if (list_next(node) != list_end(config->sections)) {
      if (fputc('\n', fp) == EOF) {
        LOG_ERROR("%s unable to write to file '%s': %s", __func__, temp_filename, strerror(errno));
        goto error;
      }
    }
   }
 
  // Sync written temp file out to disk. fsync() is blocking until data makes it to disk.
  if (fsync(fileno(fp)) < 0) {
    LOG_WARN("%s unable to fsync file '%s': %s", __func__, temp_filename, strerror(errno));
  }

  if (fclose(fp) == EOF) {
    LOG_ERROR("%s unable to close file '%s': %s", __func__, temp_filename, strerror(errno));
    goto error;
  }
  fp = NULL;
 
   if (chmod(temp_filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {
    LOG_ERROR("%s unable to change file permissions '%s': %s", __func__, filename, strerror(errno));

     goto error;
   }
 
  // Rename written temp file to the actual config file.
   if (rename(temp_filename, filename) == -1) {
     LOG_ERROR("%s unable to commit file '%s': %s", __func__, filename, strerror(errno));
     goto error;
   }
 
  // This should ensure the directory is updated as well.
  if (fsync(dir_fd) < 0) {
    LOG_WARN("%s unable to fsync dir '%s': %s", __func__, directoryname, strerror(errno));
  }

  if (close(dir_fd) < 0) {
    LOG_ERROR("%s unable to close dir '%s': %s", __func__, directoryname, strerror(errno));
    goto error;
  }

   osi_free(temp_filename);
  osi_free(temp_dirname);
   return true;
 
error:
  // This indicates there is a write issue.  Unlink as partial data is not acceptable.
   unlink(temp_filename);
  if (fp)
    fclose(fp);
  if (dir_fd != -1)
    close(dir_fd);
   osi_free(temp_filename);
  osi_free(temp_dirname);
   return false;
 }
