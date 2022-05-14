void btsnoop_net_write(const void *data, size_t length) {
#if (!defined(BT_NET_DEBUG) || (BT_NET_DEBUG != TRUE))
 return; // Disable using network sockets for security reasons
#endif

 
   pthread_mutex_lock(&client_socket_lock_);
   if (client_socket_ != -1) {
    if (send(client_socket_, data, length, 0) == -1 && errno == ECONNRESET) {
       safe_close_(&client_socket_);
     }
   }
  pthread_mutex_unlock(&client_socket_lock_);
}
