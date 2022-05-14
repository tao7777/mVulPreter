CompileKeymap(XkbFile *file, struct xkb_keymap *keymap, enum merge_mode merge)
{
    bool ok;
    XkbFile *files[LAST_KEYMAP_FILE_TYPE + 1] = { NULL };
    enum xkb_file_type type;
    struct xkb_context *ctx = keymap->ctx;

    /* Collect section files and check for duplicates. */
    for (file = (XkbFile *) file->defs; file;
          file = (XkbFile *) file->common.next) {
         if (file->file_type < FIRST_KEYMAP_FILE_TYPE ||
             file->file_type > LAST_KEYMAP_FILE_TYPE) {
            if (file->file_type == FILE_TYPE_GEOMETRY) {
                log_vrb(ctx, 1,
                        "Geometry sections are not supported; ignoring\n");
            } else {
                log_err(ctx, "Cannot define %s in a keymap file\n",
                        xkb_file_type_to_string(file->file_type));
            }
             continue;
         }
 
        if (files[file->file_type]) {
            log_err(ctx,
                    "More than one %s section in keymap file; "
                    "All sections after the first ignored\n",
                    xkb_file_type_to_string(file->file_type));
            continue;
        }

        files[file->file_type] = file;
    }

    /*
     * Check that all required section were provided.
     * Report everything before failing.
     */
    ok = true;
    for (type = FIRST_KEYMAP_FILE_TYPE;
         type <= LAST_KEYMAP_FILE_TYPE;
         type++) {
        if (files[type] == NULL) {
            log_err(ctx, "Required section %s missing from keymap\n",
                    xkb_file_type_to_string(type));
            ok = false;
        }
    }
    if (!ok)
        return false;

    /* Compile sections. */
    for (type = FIRST_KEYMAP_FILE_TYPE;
         type <= LAST_KEYMAP_FILE_TYPE;
         type++) {
        log_dbg(ctx, "Compiling %s \"%s\"\n",
                xkb_file_type_to_string(type), files[type]->name);

        ok = compile_file_fns[type](files[type], keymap, merge);
        if (!ok) {
            log_err(ctx, "Failed to compile %s\n",
                    xkb_file_type_to_string(type));
            return false;
        }
    }

    return UpdateDerivedKeymapFields(keymap);
}
