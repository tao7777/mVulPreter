int net_get(int s, void *arg, int *len)
{
	struct net_hdr nh;
	int plen;

	if (net_read_exact(s, &nh, sizeof(nh)) == -1)
        {
		return -1;
        }

	plen = ntohl(nh.nh_len);
 	if (!(plen <= *len))
 		printf("PLEN %d type %d len %d\n",
 			plen, nh.nh_type, *len);
	assert(plen <= *len && plen > 0); /* XXX */
 
 	*len = plen;
 	if ((*len) && (net_read_exact(s, arg, *len) == -1))
        {
            return -1;
        }

	return nh.nh_type;
}
