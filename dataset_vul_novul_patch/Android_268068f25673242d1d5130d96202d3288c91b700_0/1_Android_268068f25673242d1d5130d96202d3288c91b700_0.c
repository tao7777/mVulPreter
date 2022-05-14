asocket* find_local_socket(unsigned local_id, unsigned peer_id) {

     asocket* s;
     asocket* result = NULL;
 
    adb_mutex_lock(&socket_list_lock);
     for (s = local_socket_list.next; s != &local_socket_list; s = s->next) {
         if (s->id != local_id) {
             continue;
 }
 if (peer_id == 0 || (s->peer && s->peer->id == peer_id)) {
            result = s;

         }
         break;
     }
    adb_mutex_unlock(&socket_list_lock);
 
     return result;
 }
