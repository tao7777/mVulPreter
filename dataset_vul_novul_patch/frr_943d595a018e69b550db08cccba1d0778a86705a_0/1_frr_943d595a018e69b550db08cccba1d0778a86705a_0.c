bgp_size_t bgp_packet_attribute(struct bgp *bgp, struct peer *peer,
				struct stream *s, struct attr *attr,
				struct bpacket_attr_vec_arr *vecarr,
				struct prefix *p, afi_t afi, safi_t safi,
				struct peer *from, struct prefix_rd *prd,
				mpls_label_t *label, uint32_t num_labels,
				int addpath_encode, uint32_t addpath_tx_id)
{
	size_t cp;
	size_t aspath_sizep;
	struct aspath *aspath;
	int send_as4_path = 0;
	int send_as4_aggregator = 0;
	int use32bit = (CHECK_FLAG(peer->cap, PEER_CAP_AS4_RCV)) ? 1 : 0;

	if (!bgp)
		bgp = peer->bgp;

	/* Remember current pointer. */
	cp = stream_get_endp(s);

	if (p
	    && !((afi == AFI_IP && safi == SAFI_UNICAST)
		 && !peer_cap_enhe(peer, afi, safi))) {
		size_t mpattrlen_pos = 0;

		mpattrlen_pos = bgp_packet_mpattr_start(s, peer, afi, safi,
							vecarr, attr);
		bgp_packet_mpattr_prefix(s, afi, safi, p, prd, label,
					 num_labels, addpath_encode,
					 addpath_tx_id, attr);
		bgp_packet_mpattr_end(s, mpattrlen_pos);
	}

	/* Origin attribute. */
	stream_putc(s, BGP_ATTR_FLAG_TRANS);
	stream_putc(s, BGP_ATTR_ORIGIN);
	stream_putc(s, 1);
	stream_putc(s, attr->origin);

	/* AS path attribute. */

	/* If remote-peer is EBGP */
	if (peer->sort == BGP_PEER_EBGP
	    && (!CHECK_FLAG(peer->af_flags[afi][safi],
			    PEER_FLAG_AS_PATH_UNCHANGED)
		|| attr->aspath->segments == NULL)
	    && (!CHECK_FLAG(peer->af_flags[afi][safi],
			    PEER_FLAG_RSERVER_CLIENT))) {
		aspath = aspath_dup(attr->aspath);

		/* Even though we may not be configured for confederations we
		 * may have
		 * RXed an AS_PATH with AS_CONFED_SEQUENCE or AS_CONFED_SET */
		aspath = aspath_delete_confed_seq(aspath);

		if (CHECK_FLAG(bgp->config, BGP_CONFIG_CONFEDERATION)) {
			/* Stuff our path CONFED_ID on the front */
			aspath = aspath_add_seq(aspath, bgp->confed_id);
		} else {
			if (peer->change_local_as) {
				/* If replace-as is specified, we only use the
				   change_local_as when
				   advertising routes. */
				if (!CHECK_FLAG(
					    peer->flags,
					    PEER_FLAG_LOCAL_AS_REPLACE_AS)) {
					aspath = aspath_add_seq(aspath,
								peer->local_as);
				}
				aspath = aspath_add_seq(aspath,
							peer->change_local_as);
			} else {
				aspath = aspath_add_seq(aspath, peer->local_as);
			}
		}
	} else if (peer->sort == BGP_PEER_CONFED) {
		/* A confed member, so we need to do the AS_CONFED_SEQUENCE
		 * thing */
		aspath = aspath_dup(attr->aspath);
		aspath = aspath_add_confed_seq(aspath, peer->local_as);
	} else
		aspath = attr->aspath;

	/* If peer is not AS4 capable, then:
	 * - send the created AS_PATH out as AS4_PATH (optional, transitive),
	 *   but ensure that no AS_CONFED_SEQUENCE and AS_CONFED_SET path
	 * segment
	 *   types are in it (i.e. exclude them if they are there)
	 *   AND do this only if there is at least one asnum > 65535 in the
	 * path!
	 * - send an AS_PATH out, but put 16Bit ASnums in it, not 32bit, and
	 * change
	 *   all ASnums > 65535 to BGP_AS_TRANS
	 */

	stream_putc(s, BGP_ATTR_FLAG_TRANS | BGP_ATTR_FLAG_EXTLEN);
	stream_putc(s, BGP_ATTR_AS_PATH);
	aspath_sizep = stream_get_endp(s);
	stream_putw(s, 0);
	stream_putw_at(s, aspath_sizep, aspath_put(s, aspath, use32bit));

	/* OLD session may need NEW_AS_PATH sent, if there are 4-byte ASNs
	 * in the path
	 */
	if (!use32bit && aspath_has_as4(aspath))
		send_as4_path =
			1; /* we'll do this later, at the correct place */

	/* Nexthop attribute. */
	if (afi == AFI_IP && safi == SAFI_UNICAST
	    && !peer_cap_enhe(peer, afi, safi)) {
		if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_NEXT_HOP)) {
			stream_putc(s, BGP_ATTR_FLAG_TRANS);
			stream_putc(s, BGP_ATTR_NEXT_HOP);
			bpacket_attr_vec_arr_set_vec(vecarr, BGP_ATTR_VEC_NH, s,
						     attr);
			stream_putc(s, 4);
			stream_put_ipv4(s, attr->nexthop.s_addr);
		} else if (peer_cap_enhe(from, afi, safi)) {
			/*
			 * Likely this is the case when an IPv4 prefix was
			 * received with
			 * Extended Next-hop capability and now being advertised
			 * to
			 * non-ENHE peers.
			 * Setting the mandatory (ipv4) next-hop attribute here
			 * to enable
			 * implicit next-hop self with correct (ipv4 address
			 * family).
			 */
			stream_putc(s, BGP_ATTR_FLAG_TRANS);
			stream_putc(s, BGP_ATTR_NEXT_HOP);
			bpacket_attr_vec_arr_set_vec(vecarr, BGP_ATTR_VEC_NH, s,
						     NULL);
			stream_putc(s, 4);
			stream_put_ipv4(s, 0);
		}
	}

	/* MED attribute. */
	if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_MULTI_EXIT_DISC)
	    || bgp->maxmed_active) {
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL);
		stream_putc(s, BGP_ATTR_MULTI_EXIT_DISC);
		stream_putc(s, 4);
		stream_putl(s, (bgp->maxmed_active ? bgp->maxmed_value
						   : attr->med));
	}

	/* Local preference. */
	if (peer->sort == BGP_PEER_IBGP || peer->sort == BGP_PEER_CONFED) {
		stream_putc(s, BGP_ATTR_FLAG_TRANS);
		stream_putc(s, BGP_ATTR_LOCAL_PREF);
		stream_putc(s, 4);
		stream_putl(s, attr->local_pref);
	}

	/* Atomic aggregate. */
	if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_ATOMIC_AGGREGATE)) {
		stream_putc(s, BGP_ATTR_FLAG_TRANS);
		stream_putc(s, BGP_ATTR_ATOMIC_AGGREGATE);
		stream_putc(s, 0);
	}

	/* Aggregator. */
	if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_AGGREGATOR)) {
		/* Common to BGP_ATTR_AGGREGATOR, regardless of ASN size */
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL | BGP_ATTR_FLAG_TRANS);
		stream_putc(s, BGP_ATTR_AGGREGATOR);

		if (use32bit) {
			/* AS4 capable peer */
			stream_putc(s, 8);
			stream_putl(s, attr->aggregator_as);
		} else {
			/* 2-byte AS peer */
			stream_putc(s, 6);

			/* Is ASN representable in 2-bytes? Or must AS_TRANS be
			 * used? */
			if (attr->aggregator_as > 65535) {
				stream_putw(s, BGP_AS_TRANS);

				/* we have to send AS4_AGGREGATOR, too.
				 * we'll do that later in order to send
				 * attributes in ascending
				 * order.
				 */
				send_as4_aggregator = 1;
			} else
				stream_putw(s, (uint16_t)attr->aggregator_as);
		}
		stream_put_ipv4(s, attr->aggregator_addr.s_addr);
	}

	/* Community attribute. */
	if (CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_SEND_COMMUNITY)
	    && (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_COMMUNITIES))) {
		if (attr->community->size * 4 > 255) {
			stream_putc(s,
				    BGP_ATTR_FLAG_OPTIONAL | BGP_ATTR_FLAG_TRANS
					    | BGP_ATTR_FLAG_EXTLEN);
			stream_putc(s, BGP_ATTR_COMMUNITIES);
			stream_putw(s, attr->community->size * 4);
		} else {
			stream_putc(s,
				    BGP_ATTR_FLAG_OPTIONAL
					    | BGP_ATTR_FLAG_TRANS);
			stream_putc(s, BGP_ATTR_COMMUNITIES);
			stream_putc(s, attr->community->size * 4);
		}
		stream_put(s, attr->community->val, attr->community->size * 4);
	}

	/*
	 * Large Community attribute.
	 */
	if (CHECK_FLAG(peer->af_flags[afi][safi],
		       PEER_FLAG_SEND_LARGE_COMMUNITY)
	    && (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_LARGE_COMMUNITIES))) {
		if (lcom_length(attr->lcommunity) > 255) {
			stream_putc(s,
				    BGP_ATTR_FLAG_OPTIONAL | BGP_ATTR_FLAG_TRANS
					    | BGP_ATTR_FLAG_EXTLEN);
			stream_putc(s, BGP_ATTR_LARGE_COMMUNITIES);
			stream_putw(s, lcom_length(attr->lcommunity));
		} else {
			stream_putc(s,
				    BGP_ATTR_FLAG_OPTIONAL
					    | BGP_ATTR_FLAG_TRANS);
			stream_putc(s, BGP_ATTR_LARGE_COMMUNITIES);
			stream_putc(s, lcom_length(attr->lcommunity));
		}
		stream_put(s, attr->lcommunity->val,
			   lcom_length(attr->lcommunity));
	}

	/* Route Reflector. */
	if (peer->sort == BGP_PEER_IBGP && from
	    && from->sort == BGP_PEER_IBGP) {
		/* Originator ID. */
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL);
		stream_putc(s, BGP_ATTR_ORIGINATOR_ID);
		stream_putc(s, 4);

		if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_ORIGINATOR_ID))
			stream_put_in_addr(s, &attr->originator_id);
		else
			stream_put_in_addr(s, &from->remote_id);

		/* Cluster list. */
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL);
		stream_putc(s, BGP_ATTR_CLUSTER_LIST);

		if (attr->cluster) {
			stream_putc(s, attr->cluster->length + 4);
			/* If this peer configuration's parent BGP has
			 * cluster_id. */
			if (bgp->config & BGP_CONFIG_CLUSTER_ID)
				stream_put_in_addr(s, &bgp->cluster_id);
			else
				stream_put_in_addr(s, &bgp->router_id);
			stream_put(s, attr->cluster->list,
				   attr->cluster->length);
		} else {
			stream_putc(s, 4);
			/* If this peer configuration's parent BGP has
			 * cluster_id. */
			if (bgp->config & BGP_CONFIG_CLUSTER_ID)
				stream_put_in_addr(s, &bgp->cluster_id);
			else
				stream_put_in_addr(s, &bgp->router_id);
		}
	}

	/* Extended Communities attribute. */
	if (CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_SEND_EXT_COMMUNITY)
	    && (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_EXT_COMMUNITIES))) {
		if (peer->sort == BGP_PEER_IBGP
		    || peer->sort == BGP_PEER_CONFED) {
			if (attr->ecommunity->size * 8 > 255) {
				stream_putc(s,
					    BGP_ATTR_FLAG_OPTIONAL
						    | BGP_ATTR_FLAG_TRANS
						    | BGP_ATTR_FLAG_EXTLEN);
				stream_putc(s, BGP_ATTR_EXT_COMMUNITIES);
				stream_putw(s, attr->ecommunity->size * 8);
			} else {
				stream_putc(s,
					    BGP_ATTR_FLAG_OPTIONAL
						    | BGP_ATTR_FLAG_TRANS);
				stream_putc(s, BGP_ATTR_EXT_COMMUNITIES);
				stream_putc(s, attr->ecommunity->size * 8);
			}
			stream_put(s, attr->ecommunity->val,
				   attr->ecommunity->size * 8);
		} else {
			uint8_t *pnt;
			int tbit;
			int ecom_tr_size = 0;
			int i;

			for (i = 0; i < attr->ecommunity->size; i++) {
				pnt = attr->ecommunity->val + (i * 8);
				tbit = *pnt;

				if (CHECK_FLAG(tbit,
					       ECOMMUNITY_FLAG_NON_TRANSITIVE))
					continue;

				ecom_tr_size++;
			}

			if (ecom_tr_size) {
				if (ecom_tr_size * 8 > 255) {
					stream_putc(
						s,
						BGP_ATTR_FLAG_OPTIONAL
							| BGP_ATTR_FLAG_TRANS
							| BGP_ATTR_FLAG_EXTLEN);
					stream_putc(s,
						    BGP_ATTR_EXT_COMMUNITIES);
					stream_putw(s, ecom_tr_size * 8);
				} else {
					stream_putc(
						s,
						BGP_ATTR_FLAG_OPTIONAL
							| BGP_ATTR_FLAG_TRANS);
					stream_putc(s,
						    BGP_ATTR_EXT_COMMUNITIES);
					stream_putc(s, ecom_tr_size * 8);
				}

				for (i = 0; i < attr->ecommunity->size; i++) {
					pnt = attr->ecommunity->val + (i * 8);
					tbit = *pnt;

					if (CHECK_FLAG(
						    tbit,
						    ECOMMUNITY_FLAG_NON_TRANSITIVE))
						continue;

					stream_put(s, pnt, 8);
				}
			}
		}
	}

	/* Label index attribute. */
	if (safi == SAFI_LABELED_UNICAST) {
		if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_PREFIX_SID)) {
			uint32_t label_index;

			label_index = attr->label_index;

			if (label_index != BGP_INVALID_LABEL_INDEX) {
				stream_putc(s,
					    BGP_ATTR_FLAG_OPTIONAL
						    | BGP_ATTR_FLAG_TRANS);
				stream_putc(s, BGP_ATTR_PREFIX_SID);
				stream_putc(s, 10);
				stream_putc(s, BGP_PREFIX_SID_LABEL_INDEX);
				stream_putw(s,
					    BGP_PREFIX_SID_LABEL_INDEX_LENGTH);
				stream_putc(s, 0); // reserved
				stream_putw(s, 0); // flags
				stream_putl(s, label_index);
			}
		}
	}

	if (send_as4_path) {
		/* If the peer is NOT As4 capable, AND */
		/* there are ASnums > 65535 in path  THEN
		 * give out AS4_PATH */

		/* Get rid of all AS_CONFED_SEQUENCE and AS_CONFED_SET
		 * path segments!
		 * Hm, I wonder...  confederation things *should* only be at
		 * the beginning of an aspath, right?  Then we should use
		 * aspath_delete_confed_seq for this, because it is already
		 * there! (JK)
		 * Folks, talk to me: what is reasonable here!?
		 */
		aspath = aspath_delete_confed_seq(aspath);

		stream_putc(s,
			    BGP_ATTR_FLAG_TRANS | BGP_ATTR_FLAG_OPTIONAL
				    | BGP_ATTR_FLAG_EXTLEN);
		stream_putc(s, BGP_ATTR_AS4_PATH);
		aspath_sizep = stream_get_endp(s);
		stream_putw(s, 0);
		stream_putw_at(s, aspath_sizep, aspath_put(s, aspath, 1));
	}

	if (aspath != attr->aspath)
		aspath_free(aspath);

	if (send_as4_aggregator) {
		/* send AS4_AGGREGATOR, at this place */
		/* this section of code moved here in order to ensure the
		 * correct
		 * *ascending* order of attributes
		 */
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL | BGP_ATTR_FLAG_TRANS);
		stream_putc(s, BGP_ATTR_AS4_AGGREGATOR);
		stream_putc(s, 8);
		stream_putl(s, attr->aggregator_as);
		stream_put_ipv4(s, attr->aggregator_addr.s_addr);
	}

	if (((afi == AFI_IP || afi == AFI_IP6)
	     && (safi == SAFI_ENCAP || safi == SAFI_MPLS_VPN))
	    || (afi == AFI_L2VPN && safi == SAFI_EVPN)) {
 		/* Tunnel Encap attribute */
 		bgp_packet_mpattr_tea(bgp, peer, s, attr, BGP_ATTR_ENCAP);
 
#if ENABLE_BGP_VNC
 		/* VNC attribute */
 		bgp_packet_mpattr_tea(bgp, peer, s, attr, BGP_ATTR_VNC);
 #endif
	}

	/* PMSI Tunnel */
	if (attr->flag & ATTR_FLAG_BIT(BGP_ATTR_PMSI_TUNNEL)) {
		stream_putc(s, BGP_ATTR_FLAG_OPTIONAL | BGP_ATTR_FLAG_TRANS);
		stream_putc(s, BGP_ATTR_PMSI_TUNNEL);
		stream_putc(s, 9); // Length
		stream_putc(s, 0); // Flags
		stream_putc(s, PMSI_TNLTYPE_INGR_REPL); // IR (6)
		stream_put(s, &(attr->label),
			   BGP_LABEL_BYTES); // MPLS Label / VXLAN VNI
		stream_put_ipv4(s, attr->nexthop.s_addr);
	}

	/* Unknown transit attribute. */
	if (attr->transit)
		stream_put(s, attr->transit->val, attr->transit->length);

	/* Return total size of attribute. */
	return stream_get_endp(s) - cp;
}
