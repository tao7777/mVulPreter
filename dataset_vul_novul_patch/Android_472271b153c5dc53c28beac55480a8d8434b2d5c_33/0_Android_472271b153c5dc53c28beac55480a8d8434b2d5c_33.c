int bta_co_rfc_data_outgoing_size(void *user_data, int *size) {
  pthread_mutex_lock(&slot_lock);

 uint32_t id = (uintptr_t)user_data;
 int ret = false;
 *size = 0;
 rfc_slot_t *slot = find_rfc_slot_by_id(id);

   if (!slot)
     goto out;
 
  if (TEMP_FAILURE_RETRY(ioctl(slot->fd, FIONREAD, size)) == 0) {
     ret = true;
   } else {
     LOG_ERROR("%s unable to determine bytes remaining to be read on fd %d: %s", __func__, slot->fd, strerror(errno));
    cleanup_rfc_slot(slot);
 }

out:;
  pthread_mutex_unlock(&slot_lock);
 return ret;
}
