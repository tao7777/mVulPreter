 void mk_request_free(struct session_request *sr)
 {
     if (sr->fd_file > 0) {
        mk_vhost_close(sr);
     }
 
     if (sr->headers.location) {
        mk_mem_free(sr->headers.location);
    }

    if (sr->uri_processed.data != sr->uri.data) {
        mk_ptr_free(&sr->uri_processed);
    }

    if (sr->real_path.data != sr->real_path_static) {
        mk_ptr_free(&sr->real_path);
    }
}
