decode_bundle(bool load, const struct nx_action_bundle *nab,
              const struct vl_mff_map *vl_mff_map, uint64_t *tlv_bitmap,
              struct ofpbuf *ofpacts)
{
    static struct vlog_rate_limit rl = VLOG_RATE_LIMIT_INIT(1, 5);
    struct ofpact_bundle *bundle;
    uint32_t slave_type;
    size_t slaves_size, i;
    enum ofperr error;

    bundle = ofpact_put_BUNDLE(ofpacts);

    bundle->n_slaves = ntohs(nab->n_slaves);
    bundle->basis = ntohs(nab->basis);
    bundle->fields = ntohs(nab->fields);
    bundle->algorithm = ntohs(nab->algorithm);
    slave_type = ntohl(nab->slave_type);
    slaves_size = ntohs(nab->len) - sizeof *nab;

    error = OFPERR_OFPBAC_BAD_ARGUMENT;
    if (!flow_hash_fields_valid(bundle->fields)) {
        VLOG_WARN_RL(&rl, "unsupported fields %d", (int) bundle->fields);
    } else if (bundle->n_slaves > BUNDLE_MAX_SLAVES) {
        VLOG_WARN_RL(&rl, "too many slaves");
    } else if (bundle->algorithm != NX_BD_ALG_HRW
               && bundle->algorithm != NX_BD_ALG_ACTIVE_BACKUP) {
        VLOG_WARN_RL(&rl, "unsupported algorithm %d", (int) bundle->algorithm);
    } else if (slave_type != mf_nxm_header(MFF_IN_PORT)) {
        VLOG_WARN_RL(&rl, "unsupported slave type %"PRIu16, slave_type);
    } else {
        error = 0;
    }

    if (!is_all_zeros(nab->zero, sizeof nab->zero)) {
        VLOG_WARN_RL(&rl, "reserved field is nonzero");
        error = OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    if (load) {
        bundle->dst.ofs = nxm_decode_ofs(nab->ofs_nbits);
        bundle->dst.n_bits = nxm_decode_n_bits(nab->ofs_nbits);
        error = mf_vl_mff_mf_from_nxm_header(ntohl(nab->dst), vl_mff_map,
                                             &bundle->dst.field, tlv_bitmap);
        if (error) {
            return error;
        }

        if (bundle->dst.n_bits < 16) {
            VLOG_WARN_RL(&rl, "bundle_load action requires at least 16 bit "
                         "destination.");
            error = OFPERR_OFPBAC_BAD_ARGUMENT;
        }
    } else {
        if (nab->ofs_nbits || nab->dst) {
            VLOG_WARN_RL(&rl, "bundle action has nonzero reserved fields");
            error = OFPERR_OFPBAC_BAD_ARGUMENT;
        }
    }

    if (slaves_size < bundle->n_slaves * sizeof(ovs_be16)) {
        VLOG_WARN_RL(&rl, "Nicira action %s only has %"PRIuSIZE" bytes "
                     "allocated for slaves.  %"PRIuSIZE" bytes are required "
                     "for %"PRIu16" slaves.",
                      load ? "bundle_load" : "bundle", slaves_size,
                      bundle->n_slaves * sizeof(ovs_be16), bundle->n_slaves);
         error = OFPERR_OFPBAC_BAD_LEN;
    } else {
        for (i = 0; i < bundle->n_slaves; i++) {
            ofp_port_t ofp_port
                = u16_to_ofp(ntohs(((ovs_be16 *)(nab + 1))[i]));
            ofpbuf_put(ofpacts, &ofp_port, sizeof ofp_port);
            bundle = ofpacts->header;
        }
     }
 
     ofpact_finish_BUNDLE(ofpacts, &bundle);
    if (!error) {
        error = bundle_check(bundle, OFPP_MAX, NULL);
    }
    return error;
}
