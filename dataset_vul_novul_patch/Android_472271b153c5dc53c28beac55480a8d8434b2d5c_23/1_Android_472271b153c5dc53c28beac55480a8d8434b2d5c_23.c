int btpan_tap_send(int tap_fd, const BD_ADDR src, const BD_ADDR dst, UINT16 proto, const char* buf,
                    UINT16 len, BOOLEAN ext, BOOLEAN forward)
{
    UNUSED(ext);
    UNUSED(forward);
 if (tap_fd != INVALID_FD)
 {
        tETH_HDR eth_hdr;
        memcpy(&eth_hdr.h_dest, dst, ETH_ADDR_LEN);
        memcpy(&eth_hdr.h_src, src, ETH_ADDR_LEN);
        eth_hdr.h_proto = htons(proto);
 char packet[TAP_MAX_PKT_WRITE_LEN + sizeof(tETH_HDR)];
        memcpy(packet, &eth_hdr, sizeof(tETH_HDR));
 if (len > TAP_MAX_PKT_WRITE_LEN)
 {
            LOG_ERROR("btpan_tap_send eth packet size:%d is exceeded limit!", len);
 return -1;
 }

         memcpy(packet + sizeof(tETH_HDR), buf, len);
 
         /* Send data to network interface */
        int ret = write(tap_fd, packet, len + sizeof(tETH_HDR));
         BTIF_TRACE_DEBUG("ret:%d", ret);
         return ret;
     }
 return -1;

}
