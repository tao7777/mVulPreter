static void local_socket_close(asocket* s) {
    adb_mutex_lock(&socket_list_lock);
    local_socket_close_locked(s);
    adb_mutex_unlock(&socket_list_lock);
}
