process_plane(uint8 * in, int width, int height, uint8 * out, int size)
{
	UNUSED(size);
	int indexw;
	int indexh;
	int code;
	int collen;
	int replen;
	int color;
	int x;
	int revcode;
	uint8 * last_line;
	uint8 * this_line;
	uint8 * org_in;
	uint8 * org_out;

	org_in = in;
	org_out = out;
	last_line = 0;
	indexh = 0;
	while (indexh < height)
	{
		out = (org_out + width * height * 4) - ((indexh + 1) * width * 4);
		color = 0;
		this_line = out;
		indexw = 0;
		if (last_line == 0)
		{
			while (indexw < width)
			{
				code = CVAL(in);
				replen = code & 0xf;
				collen = (code >> 4) & 0xf;
				revcode = (replen << 4) | collen;
				if ((revcode <= 47) && (revcode >= 16))
				{
 					replen = revcode;
 					collen = 0;
 				}
				while (indexw < width && collen > 0)
 				{
 					color = CVAL(in);
 					*out = color;
 					out += 4;
 					indexw++;
 					collen--;
 				}
				while (indexw < width && replen > 0)
 				{
 					*out = color;
 					out += 4;
					indexw++;
					replen--;
				}
			}
		}
		else
		{
			while (indexw < width)
			{
				code = CVAL(in);
				replen = code & 0xf;
				collen = (code >> 4) & 0xf;
				revcode = (replen << 4) | collen;
				if ((revcode <= 47) && (revcode >= 16))
				{
 					replen = revcode;
 					collen = 0;
 				}
				while (indexw < width && collen > 0)
 				{
 					x = CVAL(in);
 					if (x & 1)
					{
						x = x >> 1;
						x = x + 1;
						color = -x;
					}
					else
					{
						x = x >> 1;
						color = x;
					}
					x = last_line[indexw * 4] + color;
					*out = x;
					out += 4;
 					indexw++;
 					collen--;
 				}
				while (indexw < width && replen > 0)
 				{
 					x = last_line[indexw * 4] + color;
 					*out = x;
					out += 4;
					indexw++;
					replen--;
				}
			}
		}
		indexh++;
		last_line = this_line;
	}
	return (int) (in - org_in);
}
