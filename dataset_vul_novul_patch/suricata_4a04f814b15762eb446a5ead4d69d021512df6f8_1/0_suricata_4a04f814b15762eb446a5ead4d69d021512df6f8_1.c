DefragDoSturgesNovakTest(int policy, u_char *expected, size_t expected_len)
{
    int i;
    int ret = 0;

    DefragInit();

    /*
     * Build the packets.
     */

    int id = 1;
    Packet *packets[17];
    memset(packets, 0x00, sizeof(packets));

    /*
     * Original fragments.
      */
 
     /* A*24 at 0. */
    packets[0] = BuildTestPacket(IPPROTO_ICMP, id, 0, 1, 'A', 24);
 
     /* B*15 at 32. */
    packets[1] = BuildTestPacket(IPPROTO_ICMP, id, 32 >> 3, 1, 'B', 16);
 
     /* C*24 at 48. */
    packets[2] = BuildTestPacket(IPPROTO_ICMP, id, 48 >> 3, 1, 'C', 24);
 
     /* D*8 at 80. */
    packets[3] = BuildTestPacket(IPPROTO_ICMP, id, 80 >> 3, 1, 'D', 8);
 
     /* E*16 at 104. */
    packets[4] = BuildTestPacket(IPPROTO_ICMP, id, 104 >> 3, 1, 'E', 16);
 
     /* F*24 at 120. */
    packets[5] = BuildTestPacket(IPPROTO_ICMP, id, 120 >> 3, 1, 'F', 24);
 
     /* G*16 at 144. */
    packets[6] = BuildTestPacket(IPPROTO_ICMP, id, 144 >> 3, 1, 'G', 16);
 
     /* H*16 at 160. */
    packets[7] = BuildTestPacket(IPPROTO_ICMP, id, 160 >> 3, 1, 'H', 16);
 
     /* I*8 at 176. */
    packets[8] = BuildTestPacket(IPPROTO_ICMP, id, 176 >> 3, 1, 'I', 8);
 
     /*
      * Overlapping subsequent fragments.
      */
 
     /* J*32 at 8. */
    packets[9] = BuildTestPacket(IPPROTO_ICMP, id, 8 >> 3, 1, 'J', 32);
 
     /* K*24 at 48. */
    packets[10] = BuildTestPacket(IPPROTO_ICMP, id, 48 >> 3, 1, 'K', 24);
 
     /* L*24 at 72. */
    packets[11] = BuildTestPacket(IPPROTO_ICMP, id, 72 >> 3, 1, 'L', 24);
 
     /* M*24 at 96. */
    packets[12] = BuildTestPacket(IPPROTO_ICMP, id, 96 >> 3, 1, 'M', 24);
 
     /* N*8 at 128. */
    packets[13] = BuildTestPacket(IPPROTO_ICMP, id, 128 >> 3, 1, 'N', 8);
 
     /* O*8 at 152. */
    packets[14] = BuildTestPacket(IPPROTO_ICMP, id, 152 >> 3, 1, 'O', 8);
 
     /* P*8 at 160. */
    packets[15] = BuildTestPacket(IPPROTO_ICMP, id, 160 >> 3, 1, 'P', 8);
 
     /* Q*16 at 176. */
    packets[16] = BuildTestPacket(IPPROTO_ICMP, id, 176 >> 3, 0, 'Q', 16);
 
     default_policy = policy;
 
    /* Send all but the last. */
    for (i = 0; i < 9; i++) {
        Packet *tp = Defrag(NULL, NULL, packets[i], NULL);
        if (tp != NULL) {
            SCFree(tp);
            goto end;
        }
        if (ENGINE_ISSET_EVENT(packets[i], IPV4_FRAG_OVERLAP)) {
            goto end;
        }
    }
    int overlap = 0;
    for (; i < 16; i++) {
        Packet *tp = Defrag(NULL, NULL, packets[i], NULL);
        if (tp != NULL) {
            SCFree(tp);
            goto end;
        }
        if (ENGINE_ISSET_EVENT(packets[i], IPV4_FRAG_OVERLAP)) {
            overlap++;
        }
    }
    if (!overlap) {
        goto end;
    }

    /* And now the last one. */
    Packet *reassembled = Defrag(NULL, NULL, packets[16], NULL);
    if (reassembled == NULL) {
        goto end;
    }

    if (IPV4_GET_HLEN(reassembled) != 20) {
        goto end;
    }
    if (IPV4_GET_IPLEN(reassembled) != 20 + 192) {
        goto end;
    }

    if (memcmp(GET_PKT_DATA(reassembled) + 20, expected, expected_len) != 0) {
        goto end;
    }
    SCFree(reassembled);

    /* Make sure all frags were returned back to the pool. */
    if (defrag_context->frag_pool->outstanding != 0) {
        goto end;
    }

    ret = 1;
end:
    for (i = 0; i < 17; i++) {
        SCFree(packets[i]);
    }
    DefragDestroy();
    return ret;
}
