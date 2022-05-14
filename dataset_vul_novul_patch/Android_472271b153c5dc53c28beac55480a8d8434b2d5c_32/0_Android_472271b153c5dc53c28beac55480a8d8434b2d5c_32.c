int bta_co_rfc_data_outgoing(void *user_data, uint8_t *buf, uint16_t size) {
  pthread_mutex_lock(&slot_lock);

 uint32_t id = (uintptr_t)user_data;
 int ret = false;
 rfc_slot_t *slot = find_rfc_slot_by_id(id);

   if (!slot)
     goto out;
 
  int received = TEMP_FAILURE_RETRY(recv(slot->fd, buf, size, 0));
   if(received == size) {
     ret = true;
   } else {
    LOG_ERROR("%s error receiving RFCOMM data from app: %s", __func__, strerror(errno));
    cleanup_rfc_slot(slot);
 }

out:;
  pthread_mutex_unlock(&slot_lock);
 return ret;
}
