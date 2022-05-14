CopyKeyAliasesToKeymap(struct xkb_keymap *keymap, KeyNamesInfo *info)
{
    AliasInfo *alias;
    unsigned i, num_key_aliases;
    struct xkb_key_alias *key_aliases;

    /*
     * Do some sanity checking on the aliases. We can't do it before
     * because keys and their aliases may be added out-of-order.
     */
    num_key_aliases = 0;
    darray_foreach(alias, info->aliases) {
        /* Check that ->real is a key. */
        if (!XkbKeyByName(keymap, alias->real, false)) {
            log_vrb(info->ctx, 5,
                    "Attempt to alias %s to non-existent key %s; Ignored\n",
                    KeyNameText(info->ctx, alias->alias),
                    KeyNameText(info->ctx, alias->real));
            alias->real = XKB_ATOM_NONE;
            continue;
        }

        /* Check that ->alias is not a key. */
        if (XkbKeyByName(keymap, alias->alias, false)) {
            log_vrb(info->ctx, 5,
                    "Attempt to create alias with the name of a real key; "
                    "Alias \"%s = %s\" ignored\n",
                    KeyNameText(info->ctx, alias->alias),
                    KeyNameText(info->ctx, alias->real));
            alias->real = XKB_ATOM_NONE;
            continue;
        }

        num_key_aliases++;
    }

    /* Copy key aliases. */
    key_aliases = NULL;
    if (num_key_aliases > 0) {
         key_aliases = calloc(num_key_aliases, sizeof(*key_aliases));
         if (!key_aliases)
             return false;
 
        i = 0;
        darray_foreach(alias, info->aliases) {
            if (alias->real != XKB_ATOM_NONE) {
                key_aliases[i].alias = alias->alias;
                key_aliases[i].real = alias->real;
                i++;
            }
         }
     }
 
    keymap->num_key_aliases = num_key_aliases;
    keymap->key_aliases = key_aliases;
    return true;
}
