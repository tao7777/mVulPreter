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
	int len;
 	/* allocate memory for maximum length filename */
	char *fn = (char *) sys->alloc(sys, (size_t) 13);
	if (!(hdr->filename = fn)) return MSPACK_ERR_NOMEMORY;
 
 	/* copy filename if present */
 	if (hdr->headers & MSKWAJ_HDR_HASFILENAME) {
	    /* read and copy up to 9 bytes of a null terminated string */
	    if ((len = sys->read(fh, &buf[0], 9)) < 2) return MSPACK_ERR_READ;
	    for (i = 0; i < len; i++) if (!(*fn++ = buf[i])) break;
	    /* if string was 9 bytes with no null terminator, reject it */
	    if (i == 9 && buf[8] != '\0') return MSPACK_ERR_DATAFORMAT;
	    /* seek to byte after string ended in file */
	    if (sys->seek(fh, (off_t)(i + 1 - len), MSPACK_SYS_SEEK_CUR))
 		return MSPACK_ERR_SEEK;
	    fn--; /* remove the null terminator */
 	}
 
 	/* copy extension if present */
 	if (hdr->headers & MSKWAJ_HDR_HASFILEEXT) {
 	    *fn++ = '.';
	    /* read and copy up to 4 bytes of a null terminated string */
	    if ((len = sys->read(fh, &buf[0], 4)) < 2) return MSPACK_ERR_READ;
	    for (i = 0; i < len; i++) if (!(*fn++ = buf[i])) break;
	    /* if string was 4 bytes with no null terminator, reject it */
	    if (i == 4 && buf[3] != '\0') return MSPACK_ERR_DATAFORMAT;
	    /* seek to byte after string ended in file */
	    if (sys->seek(fh, (off_t)(i + 1 - len), MSPACK_SYS_SEEK_CUR))
 		return MSPACK_ERR_SEEK;
	    fn--; /* remove the null terminator */
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
