static void DefragTrackerInit(DefragTracker *dt, Packet *p)
{
    /* copy address */
    COPY_ADDRESS(&p->src, &dt->src_addr);
    COPY_ADDRESS(&p->dst, &dt->dst_addr);

    if (PKT_IS_IPV4(p)) {
        dt->id = (int32_t)IPV4_GET_IPID(p);
        dt->af = AF_INET;
    } else {
         dt->id = (int32_t)IPV6_EXTHDR_GET_FH_ID(p);
         dt->af = AF_INET6;
     }
    dt->proto = IP_GET_IPPROTO(p);
     dt->vlan_id[0] = p->vlan_id[0];
     dt->vlan_id[1] = p->vlan_id[1];
     dt->policy = DefragGetOsPolicy(p);
    dt->host_timeout = DefragPolicyGetHostTimeout(p);
    dt->remove = 0;
    dt->seen_last = 0;

    TAILQ_INIT(&dt->frags);
    (void) DefragTrackerIncrUsecnt(dt);
}
