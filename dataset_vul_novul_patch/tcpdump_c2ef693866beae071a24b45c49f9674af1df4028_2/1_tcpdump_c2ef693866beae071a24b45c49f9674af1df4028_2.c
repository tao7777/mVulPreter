ikev2_n_print(netdissect_options *ndo, u_char tpay _U_,
		const struct isakmp_gen *ext,
		u_int item_len, const u_char *ep,
		uint32_t phase _U_, uint32_t doi _U_,
		uint32_t proto _U_, int depth _U_)
{
 	const struct ikev2_n *p;
 	struct ikev2_n n;
 	const u_char *cp;
	u_char showspi, showdata, showsomedata;
 	const char *notify_name;
 	uint32_t type;
 
	p = (const struct ikev2_n *)ext;
	ND_TCHECK(*p);
	UNALIGNED_MEMCPY(&n, ext, sizeof(n));
 	ikev2_pay_print(ndo, NPSTR(ISAKMP_NPTYPE_N), n.h.critical);
 
 	showspi = 1;
	showdata = 0;
 	showsomedata=0;
 	notify_name=NULL;
 
	ND_PRINT((ndo," prot_id=%s", PROTOIDSTR(n.prot_id)));

	type = ntohs(n.type);

	/* notify space is annoying sparse */
	switch(type) {
	case IV2_NOTIFY_UNSUPPORTED_CRITICAL_PAYLOAD:
		notify_name = "unsupported_critical_payload";
		showspi = 0;
		break;

	case IV2_NOTIFY_INVALID_IKE_SPI:
		notify_name = "invalid_ike_spi";
		showspi = 1;
		break;

	case IV2_NOTIFY_INVALID_MAJOR_VERSION:
		notify_name = "invalid_major_version";
		showspi = 0;
		break;

	case IV2_NOTIFY_INVALID_SYNTAX:
		notify_name = "invalid_syntax";
		showspi = 1;
		break;

	case IV2_NOTIFY_INVALID_MESSAGE_ID:
		notify_name = "invalid_message_id";
		showspi = 1;
		break;

	case IV2_NOTIFY_INVALID_SPI:
		notify_name = "invalid_spi";
		showspi = 1;
		break;

	case IV2_NOTIFY_NO_PROPOSAL_CHOSEN:
		notify_name = "no_protocol_chosen";
		showspi = 1;
		break;

	case IV2_NOTIFY_INVALID_KE_PAYLOAD:
		notify_name = "invalid_ke_payload";
		showspi = 1;
		break;

	case IV2_NOTIFY_AUTHENTICATION_FAILED:
		notify_name = "authentication_failed";
		showspi = 1;
		break;

	case IV2_NOTIFY_SINGLE_PAIR_REQUIRED:
		notify_name = "single_pair_required";
		showspi = 1;
		break;

	case IV2_NOTIFY_NO_ADDITIONAL_SAS:
		notify_name = "no_additional_sas";
		showspi = 0;
		break;

	case IV2_NOTIFY_INTERNAL_ADDRESS_FAILURE:
		notify_name = "internal_address_failure";
		showspi = 0;
		break;

	case IV2_NOTIFY_FAILED_CP_REQUIRED:
		notify_name = "failed:cp_required";
		showspi = 0;
		break;

	case IV2_NOTIFY_INVALID_SELECTORS:
		notify_name = "invalid_selectors";
		showspi = 0;
		break;

	case IV2_NOTIFY_INITIAL_CONTACT:
		notify_name = "initial_contact";
		showspi = 0;
		break;

	case IV2_NOTIFY_SET_WINDOW_SIZE:
		notify_name = "set_window_size";
		showspi = 0;
		break;

	case IV2_NOTIFY_ADDITIONAL_TS_POSSIBLE:
		notify_name = "additional_ts_possible";
		showspi = 0;
		break;

	case IV2_NOTIFY_IPCOMP_SUPPORTED:
		notify_name = "ipcomp_supported";
		showspi = 0;
		break;

	case IV2_NOTIFY_NAT_DETECTION_SOURCE_IP:
		notify_name = "nat_detection_source_ip";
		showspi = 1;
		break;

	case IV2_NOTIFY_NAT_DETECTION_DESTINATION_IP:
		notify_name = "nat_detection_destination_ip";
		showspi = 1;
		break;

	case IV2_NOTIFY_COOKIE:
 		notify_name = "cookie";
 		showspi = 1;
 		showsomedata= 1;
		showdata= 0;
 		break;
 
 	case IV2_NOTIFY_USE_TRANSPORT_MODE:
		notify_name = "use_transport_mode";
		showspi = 0;
		break;

	case IV2_NOTIFY_HTTP_CERT_LOOKUP_SUPPORTED:
		notify_name = "http_cert_lookup_supported";
		showspi = 0;
		break;

	case IV2_NOTIFY_REKEY_SA:
		notify_name = "rekey_sa";
		showspi = 1;
		break;

	case IV2_NOTIFY_ESP_TFC_PADDING_NOT_SUPPORTED:
		notify_name = "tfc_padding_not_supported";
		showspi = 0;
		break;

	case IV2_NOTIFY_NON_FIRST_FRAGMENTS_ALSO:
		notify_name = "non_first_fragment_also";
		showspi = 0;
		break;

	default:
		if (type < 8192) {
			notify_name="error";
		} else if(type < 16384) {
			notify_name="private-error";
		} else if(type < 40960) {
			notify_name="status";
		} else {
			notify_name="private-status";
		}
	}

	if(notify_name) {
		ND_PRINT((ndo," type=%u(%s)", type, notify_name));
	}


	if (showspi && n.spi_size) {
		ND_PRINT((ndo," spi="));
		if (!rawprint(ndo, (const uint8_t *)(p + 1), n.spi_size))
			goto trunc;
	}
 
 	cp = (const u_char *)(p + 1) + n.spi_size;
 
	if(3 < ndo->ndo_vflag) {
		showdata = 1;
	}
	if ((showdata || (showsomedata && ep-cp < 30)) && cp < ep) {
		ND_PRINT((ndo," data=("));
		if (!rawprint(ndo, (const uint8_t *)(cp), ep - cp))
			goto trunc;
		ND_PRINT((ndo,")"));
 
	} else if(showsomedata && cp < ep) {
		if(!ike_show_somedata(ndo, cp, ep)) goto trunc;
 	}
 
 	return (const u_char *)ext + item_len;
trunc:
	ND_PRINT((ndo," [|%s]", NPSTR(ISAKMP_NPTYPE_N)));
	return NULL;
}
