int mk_request_error(int http_status, struct client_session *cs,
                     struct session_request *sr) {
    int ret, fd;
    mk_ptr_t message, *page = 0;
    struct error_page *entry;
    struct mk_list *head;
    struct file_info finfo;

    mk_header_set_http_status(sr, http_status);

    /*
     * We are nice sending error pages for clients who at least respect
     * the especification
     */
    if (http_status != MK_CLIENT_LENGTH_REQUIRED &&
        http_status != MK_CLIENT_BAD_REQUEST &&
        http_status != MK_CLIENT_REQUEST_ENTITY_TOO_LARGE) {

        /* Lookup a customized error page */
        mk_list_foreach(head, &sr->host_conf->error_pages) {
            entry = mk_list_entry(head, struct error_page, _head);
            if (entry->status != http_status) {
                continue;
            }

            /* validate error file */
            ret = mk_file_get_info(entry->real_path, &finfo);
            if (ret == -1) {
                break;
            }

            /* open file */
            fd = open(entry->real_path, config->open_flags);
            if (fd == -1) {
                 break;
             }
 
            sr->fd_file = fd;
             sr->bytes_to_send = finfo.size;
             sr->headers.content_length = finfo.size;
             sr->headers.real_length    = finfo.size;

            memcpy(&sr->file_info, &finfo, sizeof(struct file_info));

            mk_header_send(cs->socket, cs, sr);
            return mk_http_send_file(cs, sr);
        }
    }

    mk_ptr_reset(&message);

    switch (http_status) {
    case MK_CLIENT_BAD_REQUEST:
        page = mk_request_set_default_page("Bad Request",
                                           sr->uri,
                                           sr->host_conf->host_signature);
        break;

    case MK_CLIENT_FORBIDDEN:
        page = mk_request_set_default_page("Forbidden",
                                           sr->uri,
                                           sr->host_conf->host_signature);
        break;

    case MK_CLIENT_NOT_FOUND:
        mk_string_build(&message.data, &message.len,
                        "The requested URL was not found on this server.");
        page = mk_request_set_default_page("Not Found",
                                           message,
                                           sr->host_conf->host_signature);
        mk_ptr_free(&message);
        break;

    case MK_CLIENT_REQUEST_ENTITY_TOO_LARGE:
        mk_string_build(&message.data, &message.len,
                        "The request entity is too large.");
        page = mk_request_set_default_page("Entity too large",
                                           message,
                                           sr->host_conf->host_signature);
        mk_ptr_free(&message);
        break;

    case MK_CLIENT_METHOD_NOT_ALLOWED:
        page = mk_request_set_default_page("Method Not Allowed",
                                           sr->uri,
                                           sr->host_conf->host_signature);
        break;

    case MK_CLIENT_REQUEST_TIMEOUT:
    case MK_CLIENT_LENGTH_REQUIRED:
        break;

    case MK_SERVER_NOT_IMPLEMENTED:
        page = mk_request_set_default_page("Method Not Implemented",
                                           sr->uri,
                                           sr->host_conf->host_signature);
        break;

    case MK_SERVER_INTERNAL_ERROR:
        page = mk_request_set_default_page("Internal Server Error",
                                           sr->uri,
                                           sr->host_conf->host_signature);
        break;

    case MK_SERVER_HTTP_VERSION_UNSUP:
        mk_ptr_reset(&message);
        page = mk_request_set_default_page("HTTP Version Not Supported",
                                           message,
                                           sr->host_conf->host_signature);
        break;
    }

    if (page) {
        sr->headers.content_length = page->len;
    }

    sr->headers.location = NULL;
    sr->headers.cgi = SH_NOCGI;
    sr->headers.pconnections_left = 0;
    sr->headers.last_modified = -1;

    if (!page) {
        mk_ptr_reset(&sr->headers.content_type);
    }
    else {
        mk_ptr_set(&sr->headers.content_type, "text/html\r\n");
    }

    mk_header_send(cs->socket, cs, sr);

    if (page) {
        if (sr->method != MK_HTTP_METHOD_HEAD)
            mk_socket_send(cs->socket, page->data, page->len);

        mk_ptr_free(page);
        mk_mem_free(page);
    }

    /* Turn off TCP_CORK */
    mk_server_cork_flag(cs->socket, TCP_CORK_OFF);
    return EXIT_ERROR;
}
