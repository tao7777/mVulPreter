static unsigned char *read_chunk(struct mschm_decompressor_p *self,
				 struct mschmd_header *chm,
				 struct mspack_file *fh,
				 unsigned int chunk_num)
{
    struct mspack_system *sys = self->system;
     unsigned char *buf;
 
     /* check arguments - most are already checked by chmd_fast_find */
    if (chunk_num > chm->num_chunks) return NULL;
     
     /* ensure chunk cache is available */
     if (!chm->chunk_cache) {
	size_t size = sizeof(unsigned char *) * chm->num_chunks;
	if (!(chm->chunk_cache = (unsigned char **) sys->alloc(sys, size))) {
	    self->error = MSPACK_ERR_NOMEMORY;
	    return NULL;
	}
	memset(chm->chunk_cache, 0, size);
    }

    /* try to answer out of chunk cache */
    if (chm->chunk_cache[chunk_num]) return chm->chunk_cache[chunk_num];

    /* need to read chunk - allocate memory for it */
    if (!(buf = (unsigned char *) sys->alloc(sys, chm->chunk_size))) {
	self->error = MSPACK_ERR_NOMEMORY;
	return NULL;
    }

    /* seek to block and read it */
    if (sys->seek(fh, (off_t) (chm->dir_offset + (chunk_num * chm->chunk_size)),
		      MSPACK_SYS_SEEK_START))
    {
	self->error = MSPACK_ERR_SEEK;
	sys->free(buf);
	return NULL;
    }
    if (sys->read(fh, buf, (int)chm->chunk_size) != (int)chm->chunk_size) {
	self->error = MSPACK_ERR_READ;
	sys->free(buf);
	return NULL;
    }

    /* check the signature. Is is PMGL or PMGI? */
    if (!((buf[0] == 0x50) && (buf[1] == 0x4D) && (buf[2] == 0x47) &&
	  ((buf[3] == 0x4C) || (buf[3] == 0x49))))
    {
	self->error = MSPACK_ERR_SEEK;
	sys->free(buf);
	return NULL;
    }

    /* all OK. Store chunk in cache and return it */
    return chm->chunk_cache[chunk_num] = buf;
}
