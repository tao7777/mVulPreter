 int sfgets(void)
 {
     struct pollfd pfd;
     int pollret;
     ssize_t readnb;
     signed char seen_r = 0;
     
     if (scanned > (size_t) 0U) {       /* support pipelining */
         readnbd -= scanned;        
        memmove(cmd, cmd + scanned, readnbd);   /* safe */
        scanned = (size_t) 0U;
    }
    pfd.fd = clientfd;
#ifdef __APPLE_CC__
    pfd.events = POLLIN | POLLERR | POLLHUP;
#else
    pfd.events = POLLIN | POLLPRI | POLLERR | POLLHUP;
#endif
    while (scanned < cmdsize) {
        if (scanned >= readnbd) {      /* nothing left in the buffer */
            pfd.revents = 0;
            while ((pollret = poll(&pfd, 1U, idletime * 1000UL)) < 0 &&
                   errno == EINTR);
            if (pollret == 0) {
                return -1;
            }
            if (pollret <= 0 ||
                (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
                return -2;
            }
            if ((pfd.revents & (POLLIN | POLLPRI)) == 0) {
                continue;
            }
            if (readnbd >= cmdsize) {
                break;
            }
#ifdef WITH_TLS
            if (tls_cnx != NULL) {
                while ((readnb = SSL_read
                        (tls_cnx, cmd + readnbd, cmdsize - readnbd))
                       < (ssize_t) 0 && errno == EINTR);
            } else
#endif
            {
                while ((readnb = read(clientfd, cmd + readnbd,
                                      cmdsize - readnbd)) < (ssize_t) 0 &&
                       errno == EINTR);
            }
            if (readnb <= (ssize_t) 0) {
                return -2;
            }
            readnbd += readnb;
            if (readnbd > cmdsize) {
                return -2;
            }
        }
#ifdef RFC_CONFORMANT_LINES
        if (seen_r != 0) {
#endif
            if (cmd[scanned] == '\n') {
#ifndef RFC_CONFORMANT_LINES
                if (seen_r != 0) {
#endif
                    cmd[scanned - 1U] = 0;
#ifndef RFC_CONFORMANT_LINES
                } else {
                    cmd[scanned] = 0;
                }
#endif
                if (++scanned >= readnbd) {   /* non-pipelined command */
                    scanned = readnbd = (size_t) 0U;
                }
                return 0;
            }
            seen_r = 0;
#ifdef RFC_CONFORMANT_LINES
        }
#endif
        if (ISCTRLCODE(cmd[scanned])) {
            if (cmd[scanned] == '\r') {
                seen_r = 1;
            }
#ifdef RFC_CONFORMANT_PARSER                   /* disabled by default, intentionnaly */
            else if (cmd[scanned] == 0) {
                cmd[scanned] = '\n';
            }
#else
            /* replace control chars with _ */
            cmd[scanned] = '_';                
#endif
        }
        scanned++;
    }
    die(421, LOG_WARNING, MSG_LINE_TOO_LONG);   /* don't remove this */
    
    return 0;                         /* to please GCC */
}
