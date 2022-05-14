 ut64 MACH0_(get_baddr)(struct MACH0_(obj_t)* bin) {
 	int i;
 
	if (bin->hdr.filetype != MH_EXECUTE && bin->hdr.filetype != MH_DYLINKER)
 		return 0;
	for (i = 0; i < bin->nsegs; ++i)
		if (bin->segs[i].fileoff == 0 && bin->segs[i].filesize != 0)
 			return bin->segs[i].vmaddr;
 	return 0;
 }
