static rsRetVal createSocket(instanceConf_t* info, void** sock) {
    int rv;
    sublist* sub;

    *sock = zsocket_new(s_context, info->type);
    if (!sock) {
        errmsg.LogError(0,
                        RS_RET_INVALID_PARAMS,
                        "zsocket_new failed: %s, for type %d",
                        zmq_strerror(errno),info->type);
        /* DK: invalid params seems right here */
        return RS_RET_INVALID_PARAMS;
    }
    DBGPRINTF("imzmq3: socket of type %d created successfully\n", info->type)
    /* Set options *before* the connect/bind. */
    if (info->identity)             zsocket_set_identity(*sock, info->identity);
    if (info->sndBuf > -1)          zsocket_set_sndbuf(*sock, info->sndBuf);
    if (info->rcvBuf > -1)          zsocket_set_rcvbuf(*sock, info->rcvBuf);
    if (info->linger > -1)          zsocket_set_linger(*sock, info->linger);
    if (info->backlog > -1)         zsocket_set_backlog(*sock, info->backlog);
    if (info->sndTimeout > -1)      zsocket_set_sndtimeo(*sock, info->sndTimeout);
    if (info->rcvTimeout > -1)      zsocket_set_rcvtimeo(*sock, info->rcvTimeout);
    if (info->maxMsgSize > -1)      zsocket_set_maxmsgsize(*sock, info->maxMsgSize);
    if (info->rate > -1)            zsocket_set_rate(*sock, info->rate);
    if (info->recoveryIVL > -1)     zsocket_set_recovery_ivl(*sock, info->recoveryIVL);
    if (info->multicastHops > -1)   zsocket_set_multicast_hops(*sock, info->multicastHops);
    if (info->reconnectIVL > -1)    zsocket_set_reconnect_ivl(*sock, info->reconnectIVL);
    if (info->reconnectIVLMax > -1) zsocket_set_reconnect_ivl_max(*sock, info->reconnectIVLMax);
    if (info->ipv4Only > -1)        zsocket_set_ipv4only(*sock, info->ipv4Only);
    if (info->affinity > -1)        zsocket_set_affinity(*sock, info->affinity);
    if (info->sndHWM > -1 )         zsocket_set_sndhwm(*sock, info->sndHWM);
    if (info->rcvHWM > -1 )         zsocket_set_rcvhwm(*sock, info->rcvHWM);
    /* Set subscriptions.*/
    if (info->type == ZMQ_SUB) {
        for(sub = info->subscriptions; sub!=NULL; sub=sub->next) {
            zsocket_set_subscribe(*sock, sub->subscribe);
        }
    }
 
     /* Do the bind/connect... */
     if (info->action==ACTION_CONNECT) {
        rv = zsocket_connect(*sock, "%s", info->description);
         if (rv == -1) {
             errmsg.LogError(0,
                             RS_RET_INVALID_PARAMS,
                            "zmq_connect using %s failed: %s",
                            info->description, zmq_strerror(errno));
            return RS_RET_INVALID_PARAMS;
         }
         DBGPRINTF("imzmq3: connect for %s successful\n",info->description);
     } else {
        rv = zsocket_bind(*sock, "%s", info->description);
         if (rv == -1) {
             errmsg.LogError(0,
                             RS_RET_INVALID_PARAMS,
                            "zmq_bind using %s failed: %s",
                            info->description, zmq_strerror(errno));
            return RS_RET_INVALID_PARAMS;
        }
        DBGPRINTF("imzmq3: bind for %s successful\n",info->description);
    }
    return RS_RET_OK;
}
