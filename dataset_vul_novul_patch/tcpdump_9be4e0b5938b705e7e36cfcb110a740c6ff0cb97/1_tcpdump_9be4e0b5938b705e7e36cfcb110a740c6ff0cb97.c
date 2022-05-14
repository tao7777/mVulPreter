extract_header_length(uint16_t fc)
{
	int len = 0;
	switch ((fc >> 10) & 0x3) {
	case 0x00:
		if (fc & (1 << 6)) /* intra-PAN with none dest addr */
			return -1;
		break;
	case 0x01:
		return -1;
	case 0x02:
		len += 4;
		break;
	case 0x03:
		len += 10;
		break;
	}
	switch ((fc >> 14) & 0x3) {
	case 0x00:
		break;
	case 0x01:
		return -1;
	case 0x02:
		len += 4;
		break;
	case 0x03:
		len += 10;
		break;
	}
	if (fc & (1 << 6)) {
		if (len < 2)
			return -1;
		len -= 2;
	}
	return len;
}
