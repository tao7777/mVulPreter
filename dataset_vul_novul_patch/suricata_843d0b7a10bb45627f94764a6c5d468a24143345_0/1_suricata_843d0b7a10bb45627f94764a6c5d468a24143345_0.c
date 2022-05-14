static void StreamTcpPacketSetState(Packet *p, TcpSession *ssn,
                                           uint8_t state)
{
     if (state == ssn->state || PKT_IS_PSEUDOPKT(p))
         return;
 
     ssn->state = state;
 
     /* update the flow state */
    switch(ssn->state) {
        case TCP_ESTABLISHED:
        case TCP_FIN_WAIT1:
        case TCP_FIN_WAIT2:
        case TCP_CLOSING:
        case TCP_CLOSE_WAIT:
            FlowUpdateState(p->flow, FLOW_STATE_ESTABLISHED);
            break;
        case TCP_LAST_ACK:
        case TCP_TIME_WAIT:
        case TCP_CLOSED:
            FlowUpdateState(p->flow, FLOW_STATE_CLOSED);
            break;
    }
}
