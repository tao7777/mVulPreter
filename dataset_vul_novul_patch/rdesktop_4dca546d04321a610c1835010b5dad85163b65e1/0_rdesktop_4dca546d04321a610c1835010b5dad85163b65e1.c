ber_parse_header(STREAM s, int tagval, int *length)
ber_parse_header(STREAM s, int tagval, uint32 *length)
 {
 	int tag, len;
 
	if (tagval > 0xff)
	{
		in_uint16_be(s, tag);
	}
	else
	{
		in_uint8(s, tag);
	}

	if (tag != tagval)
	{
		logger(Core, Error, "ber_parse_header(), expected tag %d, got %d", tagval, tag);
		return False;
	}

	in_uint8(s, len);

	if (len & 0x80)
	{
		len &= ~0x80;
		*length = 0;
		while (len--)
			next_be(s, *length);
	}
	else
		*length = len;

	return s_check(s);
}
