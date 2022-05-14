static void btu_exec_tap_fd_read(void *p_param) {
 struct pollfd ufd;
 int fd = (int)p_param;

 if (fd == INVALID_FD || fd != btpan_cb.tap_fd)
 return;

 for (int i = 0; i < PAN_POOL_MAX && btif_is_enabled() && btpan_cb.flow; i++) {
        BT_HDR *buffer = (BT_HDR *)GKI_getpoolbuf(PAN_POOL_ID);
 if (!buffer) {
            BTIF_TRACE_WARNING("%s unable to allocate buffer for packet.", __func__);
 break;
 }
        buffer->offset = PAN_MINIMUM_OFFSET;
        buffer->len = GKI_get_buf_size(buffer) - sizeof(BT_HDR) - buffer->offset;

        UINT8 *packet = (UINT8 *)buffer + sizeof(BT_HDR) + buffer->offset;


         if (!btpan_cb.congest_packet_size) {
            ssize_t ret = read(fd, btpan_cb.congest_packet, sizeof(btpan_cb.congest_packet));
             switch (ret) {
                 case -1:
                     BTIF_TRACE_ERROR("%s unable to read from driver: %s", __func__, strerror(errno));
                    GKI_freebuf(buffer);
                    btsock_thread_add_fd(pan_pth, fd, 0, SOCK_THREAD_FD_RD, 0);
 return;
 case 0:
                    BTIF_TRACE_WARNING("%s end of file reached.", __func__);
                    GKI_freebuf(buffer);
                    btsock_thread_add_fd(pan_pth, fd, 0, SOCK_THREAD_FD_RD, 0);
 return;
 default:
                    btpan_cb.congest_packet_size = ret;
 break;
 }
 }

        memcpy(packet, btpan_cb.congest_packet, MIN(btpan_cb.congest_packet_size, buffer->len));
        buffer->len = MIN(btpan_cb.congest_packet_size, buffer->len);

 if (buffer->len > sizeof(tETH_HDR) && should_forward((tETH_HDR *)packet)) {
            tETH_HDR hdr;
            memcpy(&hdr, packet, sizeof(tETH_HDR));

            buffer->len -= sizeof(tETH_HDR);
            buffer->offset += sizeof(tETH_HDR);
 if (forward_bnep(&hdr, buffer) != FORWARD_CONGEST)
                btpan_cb.congest_packet_size = 0;
 } else {
            BTIF_TRACE_WARNING("%s dropping packet of length %d", __func__, buffer->len);
            btpan_cb.congest_packet_size = 0;
            GKI_freebuf(buffer);
 }


         ufd.fd = fd;
         ufd.events = POLLIN;
         ufd.revents = 0;
        if (poll(&ufd, 1, 0) <= 0 || IS_EXCEPTION(ufd.revents))
             break;
     }
    btsock_thread_add_fd(pan_pth, fd, 0, SOCK_THREAD_FD_RD, 0);
}
