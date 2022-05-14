static void bgp_packet_mpattr_tea(struct bgp *bgp, struct peer *peer,
				  struct stream *s, struct attr *attr,
				  uint8_t attrtype)
{
	unsigned int attrlenfield = 0;
	unsigned int attrhdrlen = 0;
	struct bgp_attr_encap_subtlv *subtlvs;
	struct bgp_attr_encap_subtlv *st;
	const char *attrname;

	if (!attr || (attrtype == BGP_ATTR_ENCAP
		      && (!attr->encap_tunneltype
			  || attr->encap_tunneltype == BGP_ENCAP_TYPE_MPLS)))
		return;

	switch (attrtype) {
	case BGP_ATTR_ENCAP:
		attrname = "Tunnel Encap";
		subtlvs = attr->encap_subtlvs;
		if (subtlvs == NULL) /* nothing to do */
			return;
		/*
		 * The tunnel encap attr has an "outer" tlv.
		 * T = tunneltype,
		 * L = total length of subtlvs,
		 * V = concatenated subtlvs.
		 */
		attrlenfield = 2 + 2; /* T + L */
 		attrhdrlen = 1 + 1;   /* subTLV T + L */
 		break;
 
#if ENABLE_BGP_VNC
 	case BGP_ATTR_VNC:
 		attrname = "VNC";
 		subtlvs = attr->vnc_subtlvs;
		if (subtlvs == NULL) /* nothing to do */
			return;
		attrlenfield = 0;   /* no outer T + L */
		attrhdrlen = 2 + 2; /* subTLV T + L */
		break;
#endif

	default:
		assert(0);
	}

	/* compute attr length */
	for (st = subtlvs; st; st = st->next) {
		attrlenfield += (attrhdrlen + st->length);
	}

	if (attrlenfield > 0xffff) {
		zlog_info("%s attribute is too long (length=%d), can't send it",
			  attrname, attrlenfield);
		return;
	}

	if (attrlenfield > 0xff) {
		/* 2-octet length field */
		stream_putc(s,
			    BGP_ATTR_FLAG_TRANS | BGP_ATTR_FLAG_OPTIONAL
				    | BGP_ATTR_FLAG_EXTLEN);
		stream_putc(s, attrtype);
		stream_putw(s, attrlenfield & 0xffff);
	} else {
		/* 1-octet length field */
		stream_putc(s, BGP_ATTR_FLAG_TRANS | BGP_ATTR_FLAG_OPTIONAL);
		stream_putc(s, attrtype);
		stream_putc(s, attrlenfield & 0xff);
	}

	if (attrtype == BGP_ATTR_ENCAP) {
		/* write outer T+L */
		stream_putw(s, attr->encap_tunneltype);
		stream_putw(s, attrlenfield - 4);
	}

	/* write each sub-tlv */
	for (st = subtlvs; st; st = st->next) {
		if (attrtype == BGP_ATTR_ENCAP) {
			stream_putc(s, st->type);
			stream_putc(s, st->length);
#if ENABLE_BGP_VNC
		} else {
			stream_putw(s, st->type);
			stream_putw(s, st->length);
#endif
		}
		stream_put(s, st->value, st->length);
	}
}
