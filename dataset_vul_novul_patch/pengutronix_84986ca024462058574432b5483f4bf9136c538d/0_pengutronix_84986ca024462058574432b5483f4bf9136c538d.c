static int nfs_readlink_reply(unsigned char *pkt, unsigned len)
 {
 	uint32_t *data;
 	char *path;
	unsigned int rlen;
 	int ret;
 
 	ret = rpc_check_reply(pkt, 1);
	if (ret)
		return ret;

	data = (uint32_t *)(pkt + sizeof(struct rpc_reply));

	data++;
 
 	rlen = ntohl(net_read_uint32(data)); /* new path length */
 
	rlen = max_t(unsigned int, rlen,
		     len - sizeof(struct rpc_reply) - sizeof(uint32_t));

 	data++;
 	path = (char *)data;
	} else {
		memcpy(nfs_path, path, rlen);
		nfs_path[rlen] = 0;
	}
