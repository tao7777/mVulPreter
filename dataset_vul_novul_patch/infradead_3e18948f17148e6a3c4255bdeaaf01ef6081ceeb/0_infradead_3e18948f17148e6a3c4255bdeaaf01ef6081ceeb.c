void *nlmsg_reserve(struct nl_msg *n, size_t len, int pad)
{
	void *buf = n->nm_nlh;
        size_t nlmsg_len = n->nm_nlh->nlmsg_len;
        size_t tlen;
 
       if (len > n->nm_size)
               return NULL;

        tlen = pad ? ((len + (pad - 1)) & ~(pad - 1)) : len;
 
        if ((tlen + nlmsg_len) > n->nm_size)
	n->nm_nlh->nlmsg_len += tlen;

	if (tlen > len)
		memset(buf + len, 0, tlen - len);

	NL_DBG(2, "msg %p: Reserved %zu (%zu) bytes, pad=%d, nlmsg_len=%d\n",
		  n, tlen, len, pad, n->nm_nlh->nlmsg_len);

	return buf;
}
