dophn_exec(struct magic_set *ms, int clazz, int swap, int fd, off_t off,
    int num, size_t size, off_t fsize, int *flags, int sh_num)
{
	Elf32_Phdr ph32;
	Elf64_Phdr ph64;
	const char *linking_style = "statically";
	const char *interp = "";
	unsigned char nbuf[BUFSIZ];
	char ibuf[BUFSIZ];
	ssize_t bufsize;
	size_t offset, align, len;
	
	if (size != xph_sizeof) {
		if (file_printf(ms, ", corrupted program header size") == -1)
			return -1;
		return 0;
 	}
 
   	for ( ; num; num--) {
		if (pread(fd, xph_addr, xph_sizeof, off) == -1) {
 			file_badread(ms);
 			return -1;
 		}

		off += size;
		bufsize = 0;
		align = 4;

		/* Things we can determine before we seek */
		switch (xph_type) {
		case PT_DYNAMIC:
			linking_style = "dynamically";
			break;
		case PT_NOTE:
			if (sh_num)	/* Did this through section headers */
				continue;
			if (((align = xph_align) & 0x80000000UL) != 0 ||
			    align < 4) {
				if (file_printf(ms, 
				    ", invalid note alignment 0x%lx",
				    (unsigned long)align) == -1)
					return -1;
				align = 4;
			}
			/*FALLTHROUGH*/
		case PT_INTERP:
			len = xph_filesz < sizeof(nbuf) ? xph_filesz
			    : sizeof(nbuf);
			bufsize = pread(fd, nbuf, len, xph_offset);
			if (bufsize == -1) {
				file_badread(ms);
				return -1;
			}
			break;
		default:
			if (fsize != SIZE_UNKNOWN && xph_offset > fsize) {
				/* Maybe warn here? */
				continue;
			}
			break;
		}

		/* Things we can determine when we seek */
		switch (xph_type) {
		case PT_INTERP:
			if (bufsize && nbuf[0]) {
				nbuf[bufsize - 1] = '\0';
				interp = (const char *)nbuf;
			} else
				interp = "*empty*";
			break;
		case PT_NOTE:
			/*
			 * This is a PT_NOTE section; loop through all the notes
			 * in the section.
			 */
			offset = 0;
			for (;;) {
				if (offset >= (size_t)bufsize)
					break;
				offset = donote(ms, nbuf, offset,
				    (size_t)bufsize, clazz, swap, align,
				    flags);
				if (offset == 0)
					break;
			}
			break;
		default:
			break;
		}
	}
	if (file_printf(ms, ", %s linked", linking_style)
	    == -1)
		return -1;
	if (interp[0])
		if (file_printf(ms, ", interpreter %s",
		    file_printable(ibuf, sizeof(ibuf), interp)) == -1)
			return -1;
	return 0;
}
