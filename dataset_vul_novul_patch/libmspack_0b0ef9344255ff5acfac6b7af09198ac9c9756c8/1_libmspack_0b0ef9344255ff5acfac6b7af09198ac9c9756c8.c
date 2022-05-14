static int kwajd_read_headers(struct mspack_system *sys,
			      struct mspack_file *fh,
			      struct mskwajd_header *hdr)
{
    unsigned char buf[16];
    int i;

    /* read in the header */
    if (sys->read(fh, &buf[0], kwajh_SIZEOF) != kwajh_SIZEOF) {
	return MSPACK_ERR_READ;
    }

    /* check for "KWAJ" signature */
    if (((unsigned int) EndGetI32(&buf[kwajh_Signature1]) != 0x4A41574B) ||
	((unsigned int) EndGetI32(&buf[kwajh_Signature2]) != 0xD127F088))
    {
	return MSPACK_ERR_SIGNATURE;
    }

    /* basic header fields */
    hdr->comp_type    = EndGetI16(&buf[kwajh_CompMethod]);
    hdr->data_offset  = EndGetI16(&buf[kwajh_DataOffset]);
    hdr->headers      = EndGetI16(&buf[kwajh_Flags]);
    hdr->length       = 0;
    hdr->filename     = NULL;
    hdr->extra        = NULL;
    hdr->extra_length = 0;

    /* optional headers */

    /* 4 bytes: length of unpacked file */
    if (hdr->headers & MSKWAJ_HDR_HASLENGTH) {
	if (sys->read(fh, &buf[0], 4) != 4) return MSPACK_ERR_READ;
	hdr->length = EndGetI32(&buf[0]);
    }

    /* 2 bytes: unknown purpose */
    if (hdr->headers & MSKWAJ_HDR_HASUNKNOWN1) {
	if (sys->read(fh, &buf[0], 2) != 2) return MSPACK_ERR_READ;
    }

    /* 2 bytes: length of section, then [length] bytes: unknown purpose */
    if (hdr->headers & MSKWAJ_HDR_HASUNKNOWN2) {
	if (sys->read(fh, &buf[0], 2) != 2) return MSPACK_ERR_READ;
	i = EndGetI16(&buf[0]);
	if (sys->seek(fh, (off_t)i, MSPACK_SYS_SEEK_CUR)) return MSPACK_ERR_SEEK;
    }
 
     /* filename and extension */
     if (hdr->headers & (MSKWAJ_HDR_HASFILENAME | MSKWAJ_HDR_HASFILEEXT)) {
	off_t pos = sys->tell(fh);
	char *fn = (char *) sys->alloc(sys, (size_t) 13);
 	/* allocate memory for maximum length filename */
	if (! fn) return MSPACK_ERR_NOMEMORY;
	hdr->filename = fn;
 
 	/* copy filename if present */
 	if (hdr->headers & MSKWAJ_HDR_HASFILENAME) {
	    if (sys->read(fh, &buf[0], 9) != 9) return MSPACK_ERR_READ;
	    for (i = 0; i < 9; i++, fn++) if (!(*fn = buf[i])) break;
	    pos += (i < 9) ? i+1 : 9;
	    if (sys->seek(fh, pos, MSPACK_SYS_SEEK_START))
 		return MSPACK_ERR_SEEK;
 	}
 
 	/* copy extension if present */
 	if (hdr->headers & MSKWAJ_HDR_HASFILEEXT) {
 	    *fn++ = '.';
	    if (sys->read(fh, &buf[0], 4) != 4) return MSPACK_ERR_READ;
	    for (i = 0; i < 4; i++, fn++) if (!(*fn = buf[i])) break;
	    pos += (i < 4) ? i+1 : 4;
	    if (sys->seek(fh, pos, MSPACK_SYS_SEEK_START))
 		return MSPACK_ERR_SEEK;
 	}
 	*fn = '\0';
     }

    /* 2 bytes: extra text length then [length] bytes of extra text data */
    if (hdr->headers & MSKWAJ_HDR_HASEXTRATEXT) {
	if (sys->read(fh, &buf[0], 2) != 2) return MSPACK_ERR_READ;
	i = EndGetI16(&buf[0]);
	hdr->extra = (char *) sys->alloc(sys, (size_t)i+1);
	if (! hdr->extra) return MSPACK_ERR_NOMEMORY;
	if (sys->read(fh, hdr->extra, i) != i) return MSPACK_ERR_READ;
	hdr->extra[i] = '\0';
	hdr->extra_length = i;
    }
    return MSPACK_ERR_OK;
}
