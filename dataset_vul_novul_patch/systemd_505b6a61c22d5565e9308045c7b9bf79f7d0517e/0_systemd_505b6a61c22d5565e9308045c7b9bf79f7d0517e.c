void server_process_native_message(
                Server *s,
                const void *buffer, size_t buffer_size,
                struct ucred *ucred,
                struct timeval *tv,
                const char *label, size_t label_len) {

        struct iovec *iovec = NULL;
        unsigned n = 0, m = 0, j, tn = (unsigned) -1;
        const char *p;
        size_t remaining;
        int priority = LOG_INFO;
        char *identifier = NULL, *message = NULL;

        assert(s);
        assert(buffer || buffer_size == 0);

        p = buffer;
        remaining = buffer_size;

        while (remaining > 0) {
                const char *e, *q;

                e = memchr(p, '\n', remaining);

                if (!e) {
                        /* Trailing noise, let's ignore it, and flush what we collected */
                        log_debug("Received message with trailing noise, ignoring.");
                        break;
                }

                if (e == p) {
                        /* Entry separator */
                        server_dispatch_message(s, iovec, n, m, ucred, tv, label, label_len, NULL, priority);
                        n = 0;
                        priority = LOG_INFO;

                        p++;
                        remaining--;
                        continue;
                }

                if (*p == '.' || *p == '#') {
                        /* Ignore control commands for now, and
                         * comments too. */
                        remaining -= (e - p) + 1;
                        p = e + 1;
                        continue;
                }

                /* A property follows */

                if (n+N_IOVEC_META_FIELDS >= m) {
                        struct iovec *c;
                        unsigned u;

                        u = MAX((n+N_IOVEC_META_FIELDS+1) * 2U, 4U);
                        c = realloc(iovec, u * sizeof(struct iovec));
                        if (!c) {
                                log_oom();
                                break;
                        }

                        iovec = c;
                        m = u;
                }

                q = memchr(p, '=', e - p);
                if (q) {
                        if (valid_user_field(p, q - p)) {
                                size_t l;

                                l = e - p;

                                /* If the field name starts with an
                                 * underscore, skip the variable,
                                 * since that indidates a trusted
                                 * field */
                                iovec[n].iov_base = (char*) p;
                                iovec[n].iov_len = l;
                                n++;

                                /* We need to determine the priority
                                 * of this entry for the rate limiting
                                 * logic */
                                if (l == 10 &&
                                    memcmp(p, "PRIORITY=", 9) == 0 &&
                                    p[9] >= '0' && p[9] <= '9')
                                        priority = (priority & LOG_FACMASK) | (p[9] - '0');

                                else if (l == 17 &&
                                         memcmp(p, "SYSLOG_FACILITY=", 16) == 0 &&
                                         p[16] >= '0' && p[16] <= '9')
                                        priority = (priority & LOG_PRIMASK) | ((p[16] - '0') << 3);

                                else if (l == 18 &&
                                         memcmp(p, "SYSLOG_FACILITY=", 16) == 0 &&
                                         p[16] >= '0' && p[16] <= '9' &&
                                         p[17] >= '0' && p[17] <= '9')
                                        priority = (priority & LOG_PRIMASK) | (((p[16] - '0')*10 + (p[17] - '0')) << 3);

                                else if (l >= 19 &&
                                         memcmp(p, "SYSLOG_IDENTIFIER=", 18) == 0) {
                                        char *t;

                                        t = strndup(p + 18, l - 18);
                                        if (t) {
                                                free(identifier);
                                                identifier = t;
                                        }
                                } else if (l >= 8 &&
                                           memcmp(p, "MESSAGE=", 8) == 0) {
                                        char *t;

                                        t = strndup(p + 8, l - 8);
                                        if (t) {
                                                free(message);
                                                message = t;
                                        }
                                }
                        }

                        remaining -= (e - p) + 1;
                        p = e + 1;
                        continue;
                } else {
                        le64_t l_le;
                        uint64_t l;
                        char *k;

                        if (remaining < e - p + 1 + sizeof(uint64_t) + 1) {
                                log_debug("Failed to parse message, ignoring.");
                                break;
                        }

                        memcpy(&l_le, e + 1, sizeof(uint64_t));
                         memcpy(&l_le, e + 1, sizeof(uint64_t));
                         l = le64toh(l_le);
 
                        if (l > DATA_SIZE_MAX) {
                                log_debug("Received binary data block too large, ignoring.");
                                break;
                        }

                        if ((uint64_t) remaining < e - p + 1 + sizeof(uint64_t) + l + 1 ||
                             e[1+sizeof(uint64_t)+l] != '\n') {
                                 log_debug("Failed to parse message, ignoring.");
                                 break;
                        }

                        memcpy(k, p, e - p);
                        k[e - p] = '=';
                        memcpy(k + (e - p) + 1, e + 1 + sizeof(uint64_t), l);

                        if (valid_user_field(p, e - p)) {
                                iovec[n].iov_base = k;
                                iovec[n].iov_len = (e - p) + 1 + l;
                                n++;
                        } else
                                free(k);

                        remaining -= (e - p) + 1 + sizeof(uint64_t) + l + 1;
                        p = e + 1 + sizeof(uint64_t) + l + 1;
                }
        }

        if (n <= 0)
                goto finish;

        tn = n++;
        IOVEC_SET_STRING(iovec[tn], "_TRANSPORT=journal");

        if (message) {
                if (s->forward_to_syslog)
                        server_forward_syslog(s, priority, identifier, message, ucred, tv);

                if (s->forward_to_kmsg)
                        server_forward_kmsg(s, priority, identifier, message, ucred);

                if (s->forward_to_console)
                        server_forward_console(s, priority, identifier, message, ucred);
        }

        server_dispatch_message(s, iovec, n, m, ucred, tv, label, label_len, NULL, priority);

finish:
        for (j = 0; j < n; j++)  {
                if (j == tn)
                        continue;

                if (iovec[j].iov_base < buffer ||
                    (const uint8_t*) iovec[j].iov_base >= (const uint8_t*) buffer + buffer_size)
                        free(iovec[j].iov_base);
        }

        free(iovec);
        free(identifier);
        free(message);
}
