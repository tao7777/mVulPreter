void close_all_sockets(atransport* t) {
    asocket* s;

 /* this is a little gross, but since s->close() *will* modify
    ** the list out from under you, your options are limited.
    */
    std::lock_guard<std::recursive_mutex> lock(local_socket_list_lock);

 restart:
     for (s = local_socket_list.next; s != &local_socket_list; s = s->next) {
         if (s->transport == t || (s->peer && s->peer->transport == t)) {
            s->close(s);
             goto restart;
         }
     }
}
