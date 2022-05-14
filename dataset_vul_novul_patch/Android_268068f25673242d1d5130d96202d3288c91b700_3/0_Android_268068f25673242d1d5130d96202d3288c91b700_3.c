static void local_socket_close_locked(asocket* s) {
static void local_socket_close(asocket* s) {
    D("entered local_socket_close. LS(%d) fd=%d", s->id, s->fd);
    std::lock_guard<std::recursive_mutex> lock(local_socket_list_lock);
     if (s->peer) {
         D("LS(%d): closing peer. peer->id=%d peer->fd=%d", s->id, s->peer->id, s->peer->fd);
         /* Note: it's important to call shutdown before disconnecting from
         * the peer, this ensures that remote sockets can still get the id
         * of the local socket they're connected to, to send a CLOSE()
         * protocol event. */

         if (s->peer->shutdown) {
             s->peer->shutdown(s->peer);
         }
        s->peer->peer = nullptr;
        s->peer->close(s->peer);
        s->peer = nullptr;
     }
 
     /* If we are already closing, or if there are no
    ** pending packets, destroy immediately
    */
 if (s->closing || s->has_write_error || s->pkt_first == NULL) {
 int id = s->id;
        local_socket_destroy(s);
        D("LS(%d): closed", id);
 return;
 }

 /* otherwise, put on the closing list
    */
    D("LS(%d): closing", s->id);
    s->closing = 1;
    fdevent_del(&s->fde, FDE_READ);
    remove_socket(s);
    D("LS(%d): put on socket_closing_list fd=%d", s->id, s->fd);
    insert_local_socket(s, &local_socket_closing_list);
    CHECK_EQ(FDE_WRITE, s->fde.state & FDE_WRITE);
}
