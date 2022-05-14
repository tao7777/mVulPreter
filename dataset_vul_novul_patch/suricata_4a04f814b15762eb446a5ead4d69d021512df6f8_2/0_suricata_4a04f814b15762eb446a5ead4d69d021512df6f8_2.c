DefragIPv4NoDataTest(void)
{
    DefragContext *dc = NULL;
    Packet *p = NULL;
    int id = 12;
    int ret = 0;

    DefragInit();

    dc = DefragContextNew();
    if (dc == NULL)
         goto end;
 
     /* This packet has an offset > 0, more frags set to 0 and no data. */
    p = BuildTestPacket(IPPROTO_ICMP, id, 1, 0, 'A', 0);
     if (p == NULL)
         goto end;
 
    /* We do not expect a packet returned. */
    if (Defrag(NULL, NULL, p, NULL) != NULL)
        goto end;

    /* The fragment should have been ignored so no fragments should
     * have been allocated from the pool. */
    if (dc->frag_pool->outstanding != 0)
        return 0;

    ret = 1;
end:
    if (dc != NULL)
        DefragContextDestroy(dc);
    if (p != NULL)
        SCFree(p);

    DefragDestroy();
    return ret;
}
