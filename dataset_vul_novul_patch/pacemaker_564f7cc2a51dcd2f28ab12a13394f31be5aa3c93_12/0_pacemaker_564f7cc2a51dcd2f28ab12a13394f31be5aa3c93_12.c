cib_recv_plaintext(int sock)
/*!
 * \internal
 * \brief Read bytes off non blocking socket.
 *
 * \param session - tls session to read
 * \param max_size - max bytes allowed to read for buffer. 0 assumes no limit
 *
 * \note only use with NON-Blocking sockets. Should only be used after polling socket.
 *       This function will return once max_size is met, the socket read buffer
 *       is empty, or an error is encountered.
 *
 * \retval '\0' terminated buffer on success
 */
static char *
crm_recv_plaintext(int sock, size_t max_size, size_t *recv_len, int *disconnected)
 {
     char *buf = NULL;
     ssize_t rc = 0;
     ssize_t len = 0;
    ssize_t chunk_size = max_size ? max_size : 1024;
    size_t buf_size = 0;
    size_t read_size = 0;
 
    if (sock <= 0) {
        if (disconnected) {
            *disconnected = 1;
        }
        goto done;
    }
 
    buf = calloc(1, chunk_size + 1);
    buf_size = chunk_size;
 
    while (TRUE) {
        errno = 0;
        read_size = buf_size - len;
 
        /* automatically grow the buffer when needed if max_size is not set.*/
        if (!max_size && (read_size < (chunk_size / 2))) {
            buf_size += chunk_size;
            crm_trace("Grow buffer by %d more bytes. buf is now %d bytes", (int)chunk_size, buf_size);
            buf = realloc(buf, buf_size + 1);
             CRM_ASSERT(buf != NULL);
 
            read_size = buf_size - len;
        }

        rc = read(sock, buf + len, chunk_size);

        if (rc > 0) {
            crm_trace("Got %d more bytes. errno=%d", (int)rc, errno);
             len += rc;
            /* always null terminate buffer, the +1 to alloc always allows for this.*/
            buf[len] = '\0';
        }
        if (max_size && (max_size == read_size)) {
            crm_trace("Buffer max read size %d met" , max_size);
            goto done;
        }
 
        if (rc > 0) {
            continue;
        } else if (rc == 0) {
            if (disconnected) {
                *disconnected = 1;
            }
            crm_trace("EOF encoutered during read");
            goto done;
        }

        /* process errors */
        if (errno == EINTR) {
            crm_trace("EINTER encoutered, retry socket read.");
        } else if (errno == EAGAIN) {
            crm_trace("non-blocking, exiting read on rc = %d", rc);
            goto done;
        } else if (errno <= 0) {
            if (disconnected) {
                *disconnected = 1;
            }
            crm_debug("Error receiving message: %d", (int)rc);
            goto done;
         }
     }
 
done:
    if (recv_len) {
        *recv_len = len;
    }
    if (!len) {
        free(buf);
        buf = NULL;
    }
    return buf;
 }
