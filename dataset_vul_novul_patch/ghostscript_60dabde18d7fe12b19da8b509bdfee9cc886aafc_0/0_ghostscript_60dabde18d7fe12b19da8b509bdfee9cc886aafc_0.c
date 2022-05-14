 xps_parse_color(xps_document *doc, char *base_uri, char *string,
                fz_colorspace **csp, float *samples)
 {
       fz_context *ctx = doc->ctx;
        char *p;
        int i, n;
        char buf[1024];
        char *profile;
 
       *csp = fz_device_rgb(ctx);
 
        samples[0] = 1;
        samples[1] = 0;
	samples[3] = 0;

	if (string[0] == '#')
	{
		if (strlen(string) == 9)
		{
			samples[0] = unhex(string[1]) * 16 + unhex(string[2]);
			samples[1] = unhex(string[3]) * 16 + unhex(string[4]);
			samples[2] = unhex(string[5]) * 16 + unhex(string[6]);
			samples[3] = unhex(string[7]) * 16 + unhex(string[8]);
		}
		else
		{
			samples[0] = 255;
			samples[1] = unhex(string[1]) * 16 + unhex(string[2]);
			samples[2] = unhex(string[3]) * 16 + unhex(string[4]);
			samples[3] = unhex(string[5]) * 16 + unhex(string[6]);
		}

		samples[0] /= 255;
		samples[1] /= 255;
		samples[2] /= 255;
		samples[3] /= 255;
	}

	else if (string[0] == 's' && string[1] == 'c' && string[2] == '#')
	{
		if (count_commas(string) == 2)
			sscanf(string, "sc#%g,%g,%g", samples + 1, samples + 2, samples + 3);
		if (count_commas(string) == 3)
			sscanf(string, "sc#%g,%g,%g,%g", samples, samples + 1, samples + 2, samples + 3);
	}

	else if (strstr(string, "ContextColor ") == string)
	{
		/* Crack the string for profile name and sample values */
		fz_strlcpy(buf, string, sizeof buf);

		profile = strchr(buf, ' ');
                profile = strchr(buf, ' ');
                if (!profile)
                {
                       fz_warn(ctx, "cannot find icc profile uri in '%s'", string);
                        return;
                }
 
		p = strchr(profile, ' ');
                p = strchr(profile, ' ');
                if (!p)
                {
                       fz_warn(ctx, "cannot find component values in '%s'", profile);
                        return;
                }
 
                *p++ = 0;
                n = count_commas(p) + 1;
               if (n > FZ_MAX_COLORS)
               {
                       fz_warn(ctx, "ignoring %d color components (max %d allowed)", n - FZ_MAX_COLORS, FZ_MAX_COLORS);
                       n = FZ_MAX_COLORS;
               }
                i = 0;
                while (i < n)
                {
				p ++;
		}
		while (i < n)
		{
			samples[i++] = 0;
		}

		/* TODO: load ICC profile */
		switch (n)
		{
		case 2: *csp = fz_device_gray(doc->ctx); break;
		case 4: *csp = fz_device_rgb(doc->ctx); break;
		case 5: *csp = fz_device_cmyk(doc->ctx); break;
                /* TODO: load ICC profile */
                switch (n)
                {
               case 2: *csp = fz_device_gray(ctx); break;
               case 4: *csp = fz_device_rgb(ctx); break;
               case 5: *csp = fz_device_cmyk(ctx); break;
               default: *csp = fz_device_gray(ctx); break;
                }
        }
 }
	for (i = 0; i < colorspace->n; i++)
		doc->color[i] = samples[i + 1];
	doc->alpha = samples[0] * doc->opacity[doc->opacity_top];
}
