 void receive_tcppacket(connection_t *c, const char *buffer, int len) {
 	vpn_packet_t outpkt;
 
	if(len > sizeof outpkt.data)
		return;

 	outpkt.len = len;
 	if(c->options & OPTION_TCPONLY)
 		outpkt.priority = 0;
	else
		outpkt.priority = -1;
	memcpy(outpkt.data, buffer, len);

	receive_packet(c->node, &outpkt);
}
