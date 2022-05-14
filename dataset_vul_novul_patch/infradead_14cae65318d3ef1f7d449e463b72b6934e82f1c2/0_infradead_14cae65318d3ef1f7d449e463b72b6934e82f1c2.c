static void set_banner(struct openconnect_info *vpninfo)
{
        char *banner, *q;
        const char *p;
 
       if (!vpninfo->banner || !(banner = malloc(strlen(vpninfo->banner)+1))) {
                unsetenv("CISCO_BANNER");
                return;
        }
	p = vpninfo->banner;
	q = banner;
	
	while (*p) {
		if (*p == '%' && isxdigit((int)(unsigned char)p[1]) &&
		    isxdigit((int)(unsigned char)p[2])) {
			*(q++) = unhex(p + 1);
			p += 3;
		} else 
			*(q++) = *(p++);
	}
	*q = 0;
	setenv("CISCO_BANNER", banner, 1);

	free(banner);
}	
