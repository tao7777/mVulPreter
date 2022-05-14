do_bid_note(struct magic_set *ms, unsigned char *nbuf, uint32_t type,
    int swap __attribute__((__unused__)), uint32_t namesz, uint32_t descsz,
     size_t noff, size_t doff, int *flags)
 {
 	if (namesz == 4 && strcmp((char *)&nbuf[noff], "GNU") == 0 &&
	    type == NT_GNU_BUILD_ID && (descsz >= 4 && descsz <= 20)) {
 		uint8_t desc[20];
 		const char *btype;
 		uint32_t i;
		*flags |= FLAGS_DID_BUILD_ID;
		switch (descsz) {
		case 8:
		    btype = "xxHash";
		    break;
		case 16:
		    btype = "md5/uuid";
		    break;
		case 20:
		    btype = "sha1";
		    break;
		default:
		    btype = "unknown";
		    break;
		}
		if (file_printf(ms, ", BuildID[%s]=", btype) == -1)
			return 1;
		(void)memcpy(desc, &nbuf[doff], descsz);
		for (i = 0; i < descsz; i++)
		    if (file_printf(ms, "%02x", desc[i]) == -1)
			return 1;
		return 1;
	}
	return 0;
}
