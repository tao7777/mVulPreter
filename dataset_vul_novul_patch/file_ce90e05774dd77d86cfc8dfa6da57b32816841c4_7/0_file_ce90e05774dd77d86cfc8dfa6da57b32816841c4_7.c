 toomany(struct magic_set *ms, const char *name, uint16_t num)
 {
	if (file_printf(ms, ", too many %s (%u)", name, num
 	    ) == -1)
 		return -1;
 	return 0;
}
