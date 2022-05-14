static rsRetVal initZMQ(instanceData* pData) {
    DEFiRet;
    
    /* create the context if necessary. */
    if (NULL == s_context) {
        zsys_handler_set(NULL);
        s_context = zctx_new();
        if (s_workerThreads > 0) zctx_set_iothreads(s_context, s_workerThreads);
    }
    
    pData->socket = zsocket_new(s_context, pData->type);
    if (NULL == pData->socket) {
        errmsg.LogError(0, RS_RET_NO_ERRCODE,
                        "omzmq3: zsocket_new failed for %s: %s",
                        pData->description, zmq_strerror(errno));
        ABORT_FINALIZE(RS_RET_NO_ERRCODE);
    }
    /* use czmq defaults for these, unless set to non-default values */
    if(pData->identity)             zsocket_set_identity(pData->socket, (char*)pData->identity);
    if(pData->sndBuf > -1)          zsocket_set_sndbuf(pData->socket, pData->sndBuf);
    if(pData->rcvBuf > -1)          zsocket_set_sndbuf(pData->socket, pData->rcvBuf);
    if(pData->linger > -1)          zsocket_set_linger(pData->socket, pData->linger);
    if(pData->backlog > -1)         zsocket_set_backlog(pData->socket, pData->backlog);
    if(pData->sndTimeout > -1)      zsocket_set_sndtimeo(pData->socket, pData->sndTimeout);
    if(pData->rcvTimeout > -1)      zsocket_set_rcvtimeo(pData->socket, pData->rcvTimeout);
    if(pData->maxMsgSize > -1)      zsocket_set_maxmsgsize(pData->socket, pData->maxMsgSize);
    if(pData->rate > -1)            zsocket_set_rate(pData->socket, pData->rate);
    if(pData->recoveryIVL > -1)     zsocket_set_recovery_ivl(pData->socket, pData->recoveryIVL);
    if(pData->multicastHops > -1)   zsocket_set_multicast_hops(pData->socket, pData->multicastHops);
    if(pData->reconnectIVL > -1)    zsocket_set_reconnect_ivl(pData->socket, pData->reconnectIVL);
    if(pData->reconnectIVLMax > -1) zsocket_set_reconnect_ivl_max(pData->socket, pData->reconnectIVLMax);
    if(pData->ipv4Only > -1)        zsocket_set_ipv4only(pData->socket, pData->ipv4Only);
    if(pData->affinity != 1)        zsocket_set_affinity(pData->socket, pData->affinity);
    if(pData->rcvHWM > -1)          zsocket_set_rcvhwm(pData->socket, pData->rcvHWM);
    if(pData->sndHWM > -1)          zsocket_set_sndhwm(pData->socket, pData->sndHWM);
    
    /* bind or connect to it */
     if (pData->action == ACTION_BIND) {
         /* bind asserts, so no need to test return val here
            which isn't the greatest api -- oh well */
        if(-1 == zsocket_bind(pData->socket, (char*)pData->description)) {
             errmsg.LogError(0, RS_RET_NO_ERRCODE, "omzmq3: bind failed for %s: %s",
                             pData->description, zmq_strerror(errno));
             ABORT_FINALIZE(RS_RET_NO_ERRCODE);
         }
         DBGPRINTF("omzmq3: bind to %s successful\n",pData->description);
     } else {
        if(-1 == zsocket_connect(pData->socket, (char*)pData->description)) {
             errmsg.LogError(0, RS_RET_NO_ERRCODE, "omzmq3: connect failed for %s: %s", 
                             pData->description, zmq_strerror(errno));
             ABORT_FINALIZE(RS_RET_NO_ERRCODE);
        }
        DBGPRINTF("omzmq3: connect to %s successful", pData->description);
    }
 finalize_it:
    RETiRet;
}
