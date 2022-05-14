void net_checksum_calculate(uint8_t *data, int length)
{
     int hlen, plen, proto, csum_offset;
     uint16_t csum;
 
    /* Ensure data has complete L2 & L3 headers. */
    if (length < 14 + 20) {
        return;
    }

     if ((data[14] & 0xf0) != 0x40)
        return; /* not IPv4 */
     hlen  = (data[14] & 0x0f) * 4;
	csum_offset = 16;
	break;
    case PROTO_UDP:
	csum_offset = 6;
	break;
    default:
	return;
    }
