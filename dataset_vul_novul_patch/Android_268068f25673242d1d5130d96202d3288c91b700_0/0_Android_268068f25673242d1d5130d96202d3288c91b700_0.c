asocket* find_local_socket(unsigned local_id, unsigned peer_id) {

     asocket* s;
     asocket* result = NULL;
 
    std::lock_guard<std::recursive_mutex> lock(local_socket_list_lock);
     for (s = local_socket_list.next; s != &local_socket_list; s = s->next) {
         if (s->id != local_id) {
             continue;
 }
 if (peer_id == 0 || (s->peer && s->peer->id == peer_id)) {
            result = s;

         }
         break;
     }
 
     return result;
 }
