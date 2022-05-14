bgp_attr_parse_ret_t bgp_attr_parse(struct peer *peer, struct attr *attr,
				    bgp_size_t size, struct bgp_nlri *mp_update,
				    struct bgp_nlri *mp_withdraw)
{
	bgp_attr_parse_ret_t ret;
	uint8_t flag = 0;
	uint8_t type = 0;
	bgp_size_t length;
	uint8_t *startp, *endp;
	uint8_t *attr_endp;
	uint8_t seen[BGP_ATTR_BITMAP_SIZE];
	/* we need the as4_path only until we have synthesized the as_path with
	 * it */
	/* same goes for as4_aggregator */
	struct aspath *as4_path = NULL;
	as_t as4_aggregator = 0;
	struct in_addr as4_aggregator_addr = {.s_addr = 0};

	/* Initialize bitmap. */
	memset(seen, 0, BGP_ATTR_BITMAP_SIZE);

	/* End pointer of BGP attribute. */
	endp = BGP_INPUT_PNT(peer) + size;

	/* Get attributes to the end of attribute length. */
	while (BGP_INPUT_PNT(peer) < endp) {
		/* Check remaining length check.*/
		if (endp - BGP_INPUT_PNT(peer) < BGP_ATTR_MIN_LEN) {
			/* XXX warning: long int format, int arg (arg 5) */
			flog_warn(
				EC_BGP_ATTRIBUTE_TOO_SMALL,
				"%s: error BGP attribute length %lu is smaller than min len",
				peer->host,
				(unsigned long)(endp
						- stream_pnt(BGP_INPUT(peer))));

			bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
					BGP_NOTIFY_UPDATE_ATTR_LENG_ERR);
			return BGP_ATTR_PARSE_ERROR;
		}

		/* Fetch attribute flag and type. */
		startp = BGP_INPUT_PNT(peer);
		/* "The lower-order four bits of the Attribute Flags octet are
		   unused.  They MUST be zero when sent and MUST be ignored when
		   received." */
		flag = 0xF0 & stream_getc(BGP_INPUT(peer));
		type = stream_getc(BGP_INPUT(peer));

		/* Check whether Extended-Length applies and is in bounds */
		if (CHECK_FLAG(flag, BGP_ATTR_FLAG_EXTLEN)
		    && ((endp - startp) < (BGP_ATTR_MIN_LEN + 1))) {
			flog_warn(
				EC_BGP_EXT_ATTRIBUTE_TOO_SMALL,
				"%s: Extended length set, but just %lu bytes of attr header",
				peer->host,
				(unsigned long)(endp
						- stream_pnt(BGP_INPUT(peer))));

			bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
					BGP_NOTIFY_UPDATE_ATTR_LENG_ERR);
			return BGP_ATTR_PARSE_ERROR;
		}

		/* Check extended attribue length bit. */
		if (CHECK_FLAG(flag, BGP_ATTR_FLAG_EXTLEN))
			length = stream_getw(BGP_INPUT(peer));
		else
			length = stream_getc(BGP_INPUT(peer));

		/* If any attribute appears more than once in the UPDATE
		   message, then the Error Subcode is set to Malformed Attribute
		   List. */

		if (CHECK_BITMAP(seen, type)) {
			flog_warn(
				EC_BGP_ATTRIBUTE_REPEATED,
				"%s: error BGP attribute type %d appears twice in a message",
				peer->host, type);

			bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
					BGP_NOTIFY_UPDATE_MAL_ATTR);
			return BGP_ATTR_PARSE_ERROR;
		}

		/* Set type to bitmap to check duplicate attribute.  `type' is
		   unsigned char so it never overflow bitmap range. */

		SET_BITMAP(seen, type);

		/* Overflow check. */
		attr_endp = BGP_INPUT_PNT(peer) + length;

		if (attr_endp > endp) {
			flog_warn(
				EC_BGP_ATTRIBUTE_TOO_LARGE,
				"%s: BGP type %d length %d is too large, attribute total length is %d.  attr_endp is %p.  endp is %p",
				peer->host, type, length, size, attr_endp,
				endp);
			/*
			 * RFC 4271 6.3
			 * If any recognized attribute has an Attribute
			 * Length that conflicts with the expected length
			 * (based on the attribute type code), then the
			 * Error Subcode MUST be set to Attribute Length
			 * Error.  The Data field MUST contain the erroneous
			 * attribute (type, length, and value).
			 * ----------
			 * We do not currently have a good way to determine the
			 * length of the attribute independent of the length
			 * received in the message. Instead we send the
			 * minimum between the amount of data we have and the
			 * amount specified by the attribute length field.
			 *
			 * Instead of directly passing in the packet buffer and
			 * offset we use the stream_get* functions to read into
			 * a stack buffer, since they perform bounds checking
			 * and we are working with untrusted data.
			 */
			unsigned char ndata[BGP_MAX_PACKET_SIZE];
			memset(ndata, 0x00, sizeof(ndata));
			size_t lfl =
				CHECK_FLAG(flag, BGP_ATTR_FLAG_EXTLEN) ? 2 : 1;
			/* Rewind to end of flag field */
			stream_forward_getp(BGP_INPUT(peer), -(1 + lfl));
			/* Type */
			stream_get(&ndata[0], BGP_INPUT(peer), 1);
			/* Length */
			stream_get(&ndata[1], BGP_INPUT(peer), lfl);
			/* Value */
			size_t atl = attr_endp - startp;
			size_t ndl = MIN(atl, STREAM_READABLE(BGP_INPUT(peer)));
			stream_get(&ndata[lfl + 1], BGP_INPUT(peer), ndl);

			bgp_notify_send_with_data(
				peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_ATTR_LENG_ERR, ndata,
				ndl + lfl + 1);

			return BGP_ATTR_PARSE_ERROR;
		}

		struct bgp_attr_parser_args attr_args = {
			.peer = peer,
			.length = length,
			.attr = attr,
			.type = type,
			.flags = flag,
			.startp = startp,
			.total = attr_endp - startp,
		};


		/* If any recognized attribute has Attribute Flags that conflict
		   with the Attribute Type Code, then the Error Subcode is set
		   to
		   Attribute Flags Error.  The Data field contains the erroneous
		   attribute (type, length and value). */
		if (bgp_attr_flag_invalid(&attr_args)) {
			ret = bgp_attr_malformed(
				&attr_args, BGP_NOTIFY_UPDATE_ATTR_FLAG_ERR,
				attr_args.total);
			if (ret == BGP_ATTR_PARSE_PROCEED)
				continue;
			return ret;
		}

		/* OK check attribute and store it's value. */
		switch (type) {
		case BGP_ATTR_ORIGIN:
			ret = bgp_attr_origin(&attr_args);
			break;
		case BGP_ATTR_AS_PATH:
			ret = bgp_attr_aspath(&attr_args);
			break;
		case BGP_ATTR_AS4_PATH:
			ret = bgp_attr_as4_path(&attr_args, &as4_path);
			break;
		case BGP_ATTR_NEXT_HOP:
			ret = bgp_attr_nexthop(&attr_args);
			break;
		case BGP_ATTR_MULTI_EXIT_DISC:
			ret = bgp_attr_med(&attr_args);
			break;
		case BGP_ATTR_LOCAL_PREF:
			ret = bgp_attr_local_pref(&attr_args);
			break;
		case BGP_ATTR_ATOMIC_AGGREGATE:
			ret = bgp_attr_atomic(&attr_args);
			break;
		case BGP_ATTR_AGGREGATOR:
			ret = bgp_attr_aggregator(&attr_args);
			break;
		case BGP_ATTR_AS4_AGGREGATOR:
			ret = bgp_attr_as4_aggregator(&attr_args,
						      &as4_aggregator,
						      &as4_aggregator_addr);
			break;
		case BGP_ATTR_COMMUNITIES:
			ret = bgp_attr_community(&attr_args);
			break;
		case BGP_ATTR_LARGE_COMMUNITIES:
			ret = bgp_attr_large_community(&attr_args);
			break;
		case BGP_ATTR_ORIGINATOR_ID:
			ret = bgp_attr_originator_id(&attr_args);
			break;
		case BGP_ATTR_CLUSTER_LIST:
			ret = bgp_attr_cluster_list(&attr_args);
			break;
		case BGP_ATTR_MP_REACH_NLRI:
			ret = bgp_mp_reach_parse(&attr_args, mp_update);
			break;
		case BGP_ATTR_MP_UNREACH_NLRI:
			ret = bgp_mp_unreach_parse(&attr_args, mp_withdraw);
			break;
 		case BGP_ATTR_EXT_COMMUNITIES:
 			ret = bgp_attr_ext_communities(&attr_args);
 			break;
#if ENABLE_BGP_VNC
 		case BGP_ATTR_VNC:
 #endif
 		case BGP_ATTR_ENCAP:
			ret = bgp_attr_encap(type, peer, length, attr, flag,
					     startp);
			break;
		case BGP_ATTR_PREFIX_SID:
			ret = bgp_attr_prefix_sid(length,
						  &attr_args, mp_update);
			break;
		case BGP_ATTR_PMSI_TUNNEL:
			ret = bgp_attr_pmsi_tunnel(&attr_args);
			break;
		default:
			ret = bgp_attr_unknown(&attr_args);
			break;
		}

		if (ret == BGP_ATTR_PARSE_ERROR_NOTIFYPLS) {
			bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
					BGP_NOTIFY_UPDATE_MAL_ATTR);
			ret = BGP_ATTR_PARSE_ERROR;
		}

		if (ret == BGP_ATTR_PARSE_EOR) {
			if (as4_path)
				aspath_unintern(&as4_path);
			return ret;
		}

		/* If hard error occurred immediately return to the caller. */
		if (ret == BGP_ATTR_PARSE_ERROR) {
			flog_warn(EC_BGP_ATTRIBUTE_PARSE_ERROR,
				  "%s: Attribute %s, parse error", peer->host,
				  lookup_msg(attr_str, type, NULL));
			if (as4_path)
				aspath_unintern(&as4_path);
			return ret;
		}
		if (ret == BGP_ATTR_PARSE_WITHDRAW) {

			flog_warn(
				EC_BGP_ATTRIBUTE_PARSE_WITHDRAW,
				"%s: Attribute %s, parse error - treating as withdrawal",
				peer->host, lookup_msg(attr_str, type, NULL));
			if (as4_path)
				aspath_unintern(&as4_path);
			return ret;
		}

		/* Check the fetched length. */
		if (BGP_INPUT_PNT(peer) != attr_endp) {
			flog_warn(EC_BGP_ATTRIBUTE_FETCH_ERROR,
				  "%s: BGP attribute %s, fetch error",
				  peer->host, lookup_msg(attr_str, type, NULL));
			bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
					BGP_NOTIFY_UPDATE_ATTR_LENG_ERR);
			if (as4_path)
				aspath_unintern(&as4_path);
			return BGP_ATTR_PARSE_ERROR;
		}
	}

	/* Check final read pointer is same as end pointer. */
	if (BGP_INPUT_PNT(peer) != endp) {
		flog_warn(EC_BGP_ATTRIBUTES_MISMATCH,
			  "%s: BGP attribute %s, length mismatch", peer->host,
			  lookup_msg(attr_str, type, NULL));
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_ATTR_LENG_ERR);
		if (as4_path)
			aspath_unintern(&as4_path);
		return BGP_ATTR_PARSE_ERROR;
	}

	/* Check all mandatory well-known attributes are present */
	if ((ret = bgp_attr_check(peer, attr)) < 0) {
		if (as4_path)
			aspath_unintern(&as4_path);
		return ret;
	}

	/*
	 * At this place we can see whether we got AS4_PATH and/or
	 * AS4_AGGREGATOR from a 16Bit peer and act accordingly.
	 * We can not do this before we've read all attributes because
	 * the as4 handling does not say whether AS4_PATH has to be sent
	 * after AS_PATH or not - and when AS4_AGGREGATOR will be send
	 * in relationship to AGGREGATOR.
	 * So, to be defensive, we are not relying on any order and read
	 * all attributes first, including these 32bit ones, and now,
	 * afterwards, we look what and if something is to be done for as4.
	 *
	 * It is possible to not have AS_PATH, e.g. GR EoR and sole
	 * MP_UNREACH_NLRI.
	 */
	/* actually... this doesn't ever return failure currently, but
	 * better safe than sorry */
	if (CHECK_FLAG(attr->flag, ATTR_FLAG_BIT(BGP_ATTR_AS_PATH))
	    && bgp_attr_munge_as4_attrs(peer, attr, as4_path, as4_aggregator,
					&as4_aggregator_addr)) {
		bgp_notify_send(peer, BGP_NOTIFY_UPDATE_ERR,
				BGP_NOTIFY_UPDATE_MAL_ATTR);
		if (as4_path)
			aspath_unintern(&as4_path);
		return BGP_ATTR_PARSE_ERROR;
	}

	/* At this stage, we have done all fiddling with as4, and the
	 * resulting info is in attr->aggregator resp. attr->aspath
	 * so we can chuck as4_aggregator and as4_path alltogether in
	 * order to save memory
	 */
	if (as4_path) {
		aspath_unintern(&as4_path); /* unintern - it is in the hash */
		/* The flag that we got this is still there, but that does not
		 * do any trouble
		 */
	}
	/*
	 * The "rest" of the code does nothing with as4_aggregator.
	 * there is no memory attached specifically which is not part
	 * of the attr.
	 * so ignoring just means do nothing.
	 */
	/*
	 * Finally do the checks on the aspath we did not do yet
	 * because we waited for a potentially synthesized aspath.
	 */
	if (attr->flag & (ATTR_FLAG_BIT(BGP_ATTR_AS_PATH))) {
		ret = bgp_attr_aspath_check(peer, attr);
		if (ret != BGP_ATTR_PARSE_PROCEED)
			return ret;
	}
	/* Finally intern unknown attribute. */
	if (attr->transit)
		attr->transit = transit_intern(attr->transit);
	if (attr->encap_subtlvs)
		attr->encap_subtlvs =
			encap_intern(attr->encap_subtlvs, ENCAP_SUBTLV_TYPE);
#if ENABLE_BGP_VNC
	if (attr->vnc_subtlvs)
		attr->vnc_subtlvs =
			encap_intern(attr->vnc_subtlvs, VNC_SUBTLV_TYPE);
#endif

	return BGP_ATTR_PARSE_PROCEED;
}
