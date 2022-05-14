static int dccp_print_option(netdissect_options *ndo, const u_char *option, u_int hlen)
static int
dccp_print_option(netdissect_options *ndo, const u_char *option, u_int hlen)
 {
 	uint8_t optlen, i;
 
	ND_TCHECK(*option);

	if (*option >= 32) {
		ND_TCHECK(*(option+1));
		optlen = *(option +1);
		if (optlen < 2) {
			if (*option >= 128)
				ND_PRINT((ndo, "CCID option %u optlen too short", *option));
			else
				ND_PRINT((ndo, "%s optlen too short",
					  tok2str(dccp_option_values, "Option %u", *option)));
			return 0;
		}
	} else
		optlen = 1;

	if (hlen < optlen) {
		if (*option >= 128)
			ND_PRINT((ndo, "CCID option %u optlen goes past header length",
				  *option));
		else
			ND_PRINT((ndo, "%s optlen goes past header length",
				  tok2str(dccp_option_values, "Option %u", *option)));
		return 0;
	}
	ND_TCHECK2(*option, optlen);

	if (*option >= 128) {
		ND_PRINT((ndo, "CCID option %d", *option));
		switch (optlen) {
			case 4:
				ND_PRINT((ndo, " %u", EXTRACT_16BITS(option + 2)));
				break;
			case 6:
				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
				break;
			default:
				break;
		}
	} else {
		ND_PRINT((ndo, "%s", tok2str(dccp_option_values, "Option %u", *option)));
		switch (*option) {
		case 32:
		case 33:
		case 34:
		case 35:
			if (optlen < 3) {
				ND_PRINT((ndo, " optlen too short"));
				return optlen;
			}
			if (*(option + 2) < 10){
				ND_PRINT((ndo, " %s", dccp_feature_nums[*(option + 2)]));
				for (i = 0; i < optlen - 3; i++)
					ND_PRINT((ndo, " %d", *(option + 3 + i)));
			}
			break;
		case 36:
			if (optlen > 2) {
				ND_PRINT((ndo, " 0x"));
				for (i = 0; i < optlen - 2; i++)
					ND_PRINT((ndo, "%02x", *(option + 2 + i)));
			}
			break;
		case 37:
			for (i = 0; i < optlen - 2; i++)
				ND_PRINT((ndo, " %d", *(option + 2 + i)));
			break;
		case 38:
			if (optlen > 2) {
				ND_PRINT((ndo, " 0x"));
				for (i = 0; i < optlen - 2; i++)
					ND_PRINT((ndo, "%02x", *(option + 2 + i)));
			}
			break;
		case 39:
			if (optlen > 2) {
				ND_PRINT((ndo, " 0x"));
				for (i = 0; i < optlen - 2; i++)
					ND_PRINT((ndo, "%02x", *(option + 2 + i)));
			}
			break;
		case 40:
			if (optlen > 2) {
				ND_PRINT((ndo, " 0x"));
				for (i = 0; i < optlen - 2; i++)
					ND_PRINT((ndo, "%02x", *(option + 2 + i)));
 			}
 			break;
 		case 41:
		/*
		 * 13.1.  Timestamp Option
		 *
		 *  +--------+--------+--------+--------+--------+--------+
		 *  |00101001|00000110|          Timestamp Value          |
		 *  +--------+--------+--------+--------+--------+--------+
		 *   Type=41  Length=6
		 */
			if (optlen == 6)
 				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
 			else
				ND_PRINT((ndo, " [optlen != 6]"));
 			break;
 		case 42:
		/*
		 * 13.3.  Timestamp Echo Option
		 *
		 *  +--------+--------+--------+--------+--------+--------+
		 *  |00101010|00000110|           Timestamp Echo          |
		 *  +--------+--------+--------+--------+--------+--------+
		 *   Type=42    Len=6
		 *
		 *  +--------+--------+------- ... -------+--------+--------+
		 *  |00101010|00001000|  Timestamp Echo   |   Elapsed Time  |
		 *  +--------+--------+------- ... -------+--------+--------+
		 *   Type=42    Len=8       (4 bytes)
		 *
		 *  +--------+--------+------- ... -------+------- ... -------+
		 *  |00101010|00001010|  Timestamp Echo   |    Elapsed Time   |
		 *  +--------+--------+------- ... -------+------- ... -------+
		 *   Type=42   Len=10       (4 bytes)           (4 bytes)
		 */
			switch (optlen) {
			case 6:
 				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
				break;
			case 8:
				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
				ND_PRINT((ndo, " (elapsed time %u)", EXTRACT_16BITS(option + 6)));
				break;
			case 10:
				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
				ND_PRINT((ndo, " (elapsed time %u)", EXTRACT_32BITS(option + 6)));
				break;
			default:
				ND_PRINT((ndo, " [optlen != 6 or 8 or 10]"));
				break;
			}
 			break;
 		case 43:
 			if (optlen == 6)
 				ND_PRINT((ndo, " %u", EXTRACT_32BITS(option + 2)));
 			else if (optlen == 4)
 				ND_PRINT((ndo, " %u", EXTRACT_16BITS(option + 2)));
 			else
				ND_PRINT((ndo, " [optlen != 4 or 6]"));
 			break;
 		case 44:
 			if (optlen > 2) {
				ND_PRINT((ndo, " "));
				for (i = 0; i < optlen - 2; i++)
					ND_PRINT((ndo, "%02x", *(option + 2 + i)));
			}
			break;
		}
	}

	return optlen;
trunc:
	ND_PRINT((ndo, "%s", tstr));
	return 0;
}
