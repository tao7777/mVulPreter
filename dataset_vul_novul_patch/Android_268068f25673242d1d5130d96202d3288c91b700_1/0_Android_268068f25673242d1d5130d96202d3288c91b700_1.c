 void install_local_socket(asocket* s) {
    std::lock_guard<std::recursive_mutex> lock(local_socket_list_lock);
 
     s->id = local_socket_next_id++;
 
     if (local_socket_next_id == 0) {
        fatal("local socket id overflow");
     }
 
     insert_local_socket(s, &local_socket_list);
 }
