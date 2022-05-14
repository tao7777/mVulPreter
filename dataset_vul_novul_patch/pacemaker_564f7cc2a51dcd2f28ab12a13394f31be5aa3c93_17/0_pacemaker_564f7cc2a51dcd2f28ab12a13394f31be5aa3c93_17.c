crm_recv_remote_msg(void *session, gboolean encrypted)
crm_parse_remote_buffer(char **msg_buf)
 {
    char *buf = NULL;
    char *start = NULL;
    char *end = NULL;
     xmlNode *xml = NULL;
 
    if (*msg_buf == NULL) {
        return NULL;
    }

    /* take ownership of the buffer */
    buf = *msg_buf;
    *msg_buf = NULL;

    /* MSGS are separated by a '\r\n\r\n'. Split a message off the buffer and return it. */
    start = buf;
    end = strstr(start, REMOTE_MSG_TERMINATOR);

    while (!xml && end) {

        /* grab the message */
        end[0] = '\0';
        end += strlen(REMOTE_MSG_TERMINATOR);

        xml = string2xml(start);
        if (xml == NULL) {
            crm_err("Couldn't parse: '%.120s'", start);
        }
        start = end;
        end = strstr(start, REMOTE_MSG_TERMINATOR);
    }

    if (xml && start) {
        /* we have msgs left over, save it until next time */
        *msg_buf = strdup(start);
        free(buf);
    } else if (!xml) {
        /* no msg present */
        *msg_buf = buf;
    }

    return xml;
}

/*!
 * \internal
 * \brief Determine if a remote session has data to read
 *
 * \retval 0, timeout occured.
 * \retval positive, data is ready to be read
 * \retval negative, session has ended
 */
int
crm_recv_remote_ready(void *session, gboolean encrypted, int timeout /* ms */)
{
    struct pollfd fds = { 0, };
    int sock = 0;
    void *sock_ptr = NULL;
    int rc = 0;
    time_t start;

    if (encrypted) {
#ifdef HAVE_GNUTLS_GNUTLS_H
        gnutls_session *tls_session = session;
        sock_ptr = gnutls_transport_get_ptr(*tls_session);
#else
        CRM_ASSERT(encrypted == FALSE);
#endif
    } else {
        sock_ptr = session;
    }

    sock = GPOINTER_TO_INT(sock_ptr);
    if (sock <= 0) {
        return -ENOTCONN;
    }

    start = time(NULL);
    errno = 0;
    do {
        fds.fd = sock;
        fds.events = POLLIN;

        /* If we got an EINTR while polling, and we have a
         * specific timeout we are trying to honor, attempt
         * to adjust the timeout to the closest second. */
        if (errno == EINTR && (timeout > 0)) {
            timeout = timeout - ((time(NULL) - start) * 1000);
            if (timeout < 1000) {
                timeout = 1000;
            }
        }

        rc = poll(&fds, 1, timeout);
    } while (rc < 0 && errno == EINTR);

    return rc;
}

char *
crm_recv_remote_raw(void *session, gboolean encrypted, size_t max_recv, size_t *recv_len, int *disconnected)
{
    char *reply = NULL;
    if (recv_len) {
        *recv_len = 0;
    }

    if (disconnected) {
        *disconnected = 0;
    }

     if (encrypted) {
 #ifdef HAVE_GNUTLS_GNUTLS_H
        reply = crm_recv_tls(session, max_recv, recv_len, disconnected);
 #else
         CRM_ASSERT(encrypted == FALSE);
 #endif
     } else {
        reply = crm_recv_plaintext(GPOINTER_TO_INT(session), max_recv, recv_len, disconnected);
     }
     if (reply == NULL || strlen(reply) == 0) {
         crm_trace("Empty reply");
    }
 
    return reply;
}

/*!
 * \internal
 * \brief Read data off the socket until at least one full message is present or timeout occures.
 * \retval TRUE message read
 * \retval FALSE full message not read
 */

gboolean
crm_recv_remote_msg(void *session, char **recv_buf, gboolean encrypted, int total_timeout /*ms */, int *disconnected)
{
    int ret;
    size_t request_len = 0;
    time_t start = time(NULL);
    char *raw_request = NULL;
    int remaining_timeout = 0;

    if (total_timeout == 0) {
        total_timeout = 10000;
    } else if (total_timeout < 0) {
        total_timeout = 60000;
    }
    *disconnected = 0;

    remaining_timeout = total_timeout;
    while ((remaining_timeout > 0) && !(*disconnected)) {

        /* read some more off the tls buffer if we still have time left. */
        crm_trace("waiting to receive remote msg, starting timeout %d, remaining_timeout %d", total_timeout, remaining_timeout);
        ret = crm_recv_remote_ready(session, encrypted, remaining_timeout);
        raw_request = NULL;

        if (ret == 0) {
            crm_err("poll timed out (%d ms) while waiting to receive msg", remaining_timeout);
            return FALSE;

        } else if (ret < 0) {
            if (errno != EINTR) {
                crm_debug("poll returned error while waiting for msg, rc: %d, errno: %d", ret, errno);
                *disconnected = 1;
                return FALSE;
            }
            crm_debug("poll EINTR encountered during poll, retrying");
        } else {
            raw_request = crm_recv_remote_raw(session, encrypted, 0, &request_len, disconnected);
        }

        remaining_timeout = remaining_timeout - ((time(NULL) - start) * 1000);

        if (!raw_request) {
            crm_debug("Empty msg received after poll");
            continue;
        }

        if (*recv_buf) {
            int old_len = strlen(*recv_buf);

            crm_trace("Expanding recv buffer from %d to %d", old_len, old_len+request_len);

            *recv_buf = realloc(*recv_buf, old_len + request_len + 1);
            memcpy(*recv_buf + old_len, raw_request, request_len);
            *(*recv_buf+old_len+request_len) = '\0';
            free(raw_request);
        } else {
            *recv_buf = raw_request;
        }

        if (strstr(*recv_buf, REMOTE_MSG_TERMINATOR)) {
            return TRUE;
         }
     }
 
    return FALSE;
 }
