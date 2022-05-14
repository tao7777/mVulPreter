LookupModMask(struct xkb_context *ctx, const void *priv, xkb_atom_t field,
              enum expr_value_type type, xkb_mod_mask_t *val_rtrn)
{
    const char *str;
    xkb_mod_index_t ndx;
    const LookupModMaskPriv *arg = priv;
    const struct xkb_mod_set *mods = arg->mods;
    enum mod_type mod_type = arg->mod_type;

    if (type != EXPR_TYPE_INT)
         return false;
 
     str = xkb_atom_text(ctx, field);
    if (!str)
        return false;
 
     if (istreq(str, "all")) {
         *val_rtrn  = MOD_REAL_MASK_ALL;
        return true;
    }

    if (istreq(str, "none")) {
        *val_rtrn = 0;
        return true;
    }

    ndx = XkbModNameToIndex(mods, field, mod_type);
    if (ndx == XKB_MOD_INVALID)
        return false;

    *val_rtrn = (1u << ndx);
    return true;
}
