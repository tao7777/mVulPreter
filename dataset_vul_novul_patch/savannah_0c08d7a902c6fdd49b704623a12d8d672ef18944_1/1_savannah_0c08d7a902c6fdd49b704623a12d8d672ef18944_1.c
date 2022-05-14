 set_hunkmax (void)
 {
     if (!p_line)
	p_line = (char **) malloc (hunkmax * sizeof *p_line);
     if (!p_len)
	p_len = (size_t *) malloc (hunkmax * sizeof *p_len);
     if (!p_Char)
	p_Char = malloc (hunkmax * sizeof *p_Char);
 }
