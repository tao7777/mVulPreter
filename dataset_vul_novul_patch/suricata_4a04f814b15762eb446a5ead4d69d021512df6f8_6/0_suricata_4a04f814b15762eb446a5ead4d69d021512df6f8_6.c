static int DefragMfIpv6Test(void)
{
    int retval = 0;
    int ip_id = 9;
    Packet *p = NULL;
 
     DefragInit();
 
    Packet *p1 = IPV6BuildTestPacket(IPPROTO_ICMPV6, ip_id, 2, 1, 'C', 8);
    Packet *p2 = IPV6BuildTestPacket(IPPROTO_ICMPV6, ip_id, 0, 1, 'A', 8);
    Packet *p3 = IPV6BuildTestPacket(IPPROTO_ICMPV6, ip_id, 1, 0, 'B', 8);
     if (p1 == NULL || p2 == NULL || p3 == NULL) {
         goto end;
     }

    p = Defrag(NULL, NULL, p1, NULL);
    if (p != NULL) {
        goto end;
    }

    p = Defrag(NULL, NULL, p2, NULL);
    if (p != NULL) {
        goto end;
    }

    /* This should return a packet as MF=0. */
    p = Defrag(NULL, NULL, p3, NULL);
    if (p == NULL) {
        goto end;
    }

    /* For IPv6 the expected length is just the length of the payload
     * of 2 fragments, so 16. */
    if (IPV6_GET_PLEN(p) != 16) {
        goto end;
    }

    retval = 1;
end:
    if (p1 != NULL) {
        SCFree(p1);
    }
    if (p2 != NULL) {
        SCFree(p2);
    }
    if (p3 != NULL) {
        SCFree(p3);
    }
    if (p != NULL) {
        SCFree(p);
    }
    DefragDestroy();
     return retval;
 }
