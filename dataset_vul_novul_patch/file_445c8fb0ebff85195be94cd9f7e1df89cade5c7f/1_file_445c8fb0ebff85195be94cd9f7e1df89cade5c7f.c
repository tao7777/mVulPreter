dophn_core(struct magic_set *ms, int clazz, int swap, int fd, off_t off,
    int num, size_t size, off_t fsize, int *flags)
{
	Elf32_Phdr ph32;
	Elf64_Phdr ph64;
	size_t offset, len;
	unsigned char nbuf[BUFSIZ];
	ssize_t bufsize;

	if (size != xph_sizeof) {
		if (file_printf(ms, ", corrupted program header size") == -1)
			return -1;
		return 0;
	}

	/*
 	 * Loop through all the program headers.
 	 */
 	for ( ; num; num--) {
		if (pread(fd, xph_addr, xph_sizeof, off) == -1) {
 			file_badread(ms);
 			return -1;
 		}
		off += size;

		if (fsize != SIZE_UNKNOWN && xph_offset > fsize) {
			/* Perhaps warn here */
			continue;
		}

		if (xph_type != PT_NOTE)
			continue;

		/*
		 * This is a PT_NOTE section; loop through all the notes
		 * in the section.
		 */
		len = xph_filesz < sizeof(nbuf) ? xph_filesz : sizeof(nbuf);
		if ((bufsize = pread(fd, nbuf, len, xph_offset)) == -1) {
			file_badread(ms);
			return -1;
		}
		offset = 0;
		for (;;) {
			if (offset >= (size_t)bufsize)
				break;
			offset = donote(ms, nbuf, offset, (size_t)bufsize,
			    clazz, swap, 4, flags);
			if (offset == 0)
				break;

		}
	}
	return 0;
}
