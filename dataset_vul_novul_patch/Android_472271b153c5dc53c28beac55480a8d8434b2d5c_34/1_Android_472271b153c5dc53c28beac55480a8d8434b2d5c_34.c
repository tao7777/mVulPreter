void btsock_rfc_signaled(UNUSED_ATTR int fd, int flags, uint32_t user_id) {
  pthread_mutex_lock(&slot_lock);

 rfc_slot_t *slot = find_rfc_slot_by_id(user_id);
 if (!slot)
 goto out;

 bool need_close = false;

 if (flags & SOCK_THREAD_FD_RD && !slot->f.server) {

     if (slot->f.connected) {
       int size = 0;
      if (!(flags & SOCK_THREAD_FD_EXCEPTION) || (ioctl(slot->fd, FIONREAD, &size) == 0 && size))
        pthread_mutex_unlock(&slot_lock);
         BTA_JvRfcommWrite(slot->rfc_handle, slot->id);
     } else {
       LOG_ERROR("%s socket signaled for read while disconnected, slot: %d, channel: %d", __func__, slot->id, slot->scn);
       need_close = true;
 }
 }

 if (flags & SOCK_THREAD_FD_WR) {
 if (!slot->f.connected || !flush_incoming_que_on_wr_signal(slot)) {
      LOG_ERROR("%s socket signaled for write while disconnected (or write failure), slot: %d, channel: %d", __func__, slot->id, slot->scn);
      need_close = true;
 }
 }


   if (need_close || (flags & SOCK_THREAD_FD_EXCEPTION)) {
     int size = 0;
    if (need_close || ioctl(slot->fd, FIONREAD, &size) != 0 || !size)
       cleanup_rfc_slot(slot);
   }
 
out:;
  pthread_mutex_unlock(&slot_lock);
}
