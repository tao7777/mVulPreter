static int dns_parse_callback(void *c, int rr, const void *data, int len, const void *packet)
 {
 	char tmp[256];
 	struct dpc_ctx *ctx = c;
	if (ctx->cnt >= MAXADDRS) return -1;
 	switch (rr) {
 	case RR_A:
 		if (len != 4) return -1;
		ctx->addrs[ctx->cnt].scopeid = 0;
		memcpy(ctx->addrs[ctx->cnt++].addr, data, 4);
		break;
	case RR_AAAA:
		if (len != 16) return -1;
		ctx->addrs[ctx->cnt].family = AF_INET6;
		ctx->addrs[ctx->cnt].scopeid = 0;
		memcpy(ctx->addrs[ctx->cnt++].addr, data, 16);
		break;
	case RR_CNAME:
		if (__dn_expand(packet, (const unsigned char *)packet + 512,
		    data, tmp, sizeof tmp) > 0 && is_valid_hostname(tmp))
			strcpy(ctx->canon, tmp);
		break;
	}
	return 0;
}
