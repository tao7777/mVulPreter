xps_init_truetype_font(xps_context_t *ctx, xps_font_t *font)
{
    int code = 0;

    font->font = (void*) gs_alloc_struct(ctx->memory, gs_font_type42, &st_gs_font_type42, "xps_font type42");
    if (!font->font)
        return gs_throw(gs_error_VMerror, "out of memory");

    /* no shortage of things to initialize */
    {
        gs_font_type42 *p42 = (gs_font_type42*) font->font;

        /* Common to all fonts: */

        p42->next = 0;
        p42->prev = 0;
        p42->memory = ctx->memory;

        p42->dir = ctx->fontdir; /* NB also set by gs_definefont later */
        p42->base = font->font; /* NB also set by gs_definefont later */
        p42->is_resource = false;
        gs_notify_init(&p42->notify_list, gs_memory_stable(ctx->memory));
        p42->id = gs_next_ids(ctx->memory, 1);

        p42->client_data = font; /* that's us */

        /* this is overwritten in grid_fit() */
        gs_make_identity(&p42->FontMatrix);
        gs_make_identity(&p42->orig_FontMatrix); /* NB ... original or zeroes? */

        p42->FontType = ft_TrueType;
        p42->BitmapWidths = false;
        p42->ExactSize = fbit_use_outlines;
        p42->InBetweenSize = fbit_use_outlines;
        p42->TransformedChar = fbit_use_outlines;
        p42->WMode = 0;
        p42->PaintType = 0;
        p42->StrokeWidth = 0;
        p42->is_cached = 0;

        p42->procs.define_font = gs_no_define_font;
        p42->procs.make_font = gs_no_make_font;
        p42->procs.font_info = gs_type42_font_info;
        p42->procs.same_font = gs_default_same_font;
        p42->procs.encode_char = xps_true_callback_encode_char;
        p42->procs.decode_glyph = xps_true_callback_decode_glyph;
        p42->procs.enumerate_glyph = gs_type42_enumerate_glyph;
        p42->procs.glyph_info = gs_type42_glyph_info;
        p42->procs.glyph_outline = gs_type42_glyph_outline;
        p42->procs.glyph_name = xps_true_callback_glyph_name;
        p42->procs.init_fstack = gs_default_init_fstack;
        p42->procs.next_char_glyph = gs_default_next_char_glyph;
         p42->procs.build_char = xps_true_callback_build_char;
 
         memset(p42->font_name.chars, 0, sizeof(p42->font_name.chars));
        xps_load_sfnt_name(font, (char*)p42->font_name.chars, sizeof(p42->font_name.chars));
         p42->font_name.size = strlen((char*)p42->font_name.chars);
 
         memset(p42->key_name.chars, 0, sizeof(p42->key_name.chars));
        strcpy((char*)p42->key_name.chars, (char*)p42->font_name.chars);
        p42->key_name.size = strlen((char*)p42->key_name.chars);

        /* Base font specific: */

        p42->FontBBox.p.x = 0;
        p42->FontBBox.p.y = 0;
        p42->FontBBox.q.x = 0;
        p42->FontBBox.q.y = 0;

        uid_set_UniqueID(&p42->UID, p42->id);

        p42->encoding_index = ENCODING_INDEX_UNKNOWN;
        p42->nearest_encoding_index = ENCODING_INDEX_ISOLATIN1;

        p42->FAPI = 0;
        p42->FAPI_font_data = 0;

        /* Type 42 specific: */

        p42->data.string_proc = xps_true_callback_string_proc;
        p42->data.proc_data = font;

        gs_type42_font_init(p42, font->subfontid);
        p42->data.get_glyph_index = xps_true_get_glyph_index;
    }

    if ((code = gs_definefont(ctx->fontdir, font->font)) < 0) {
        return(code);
    }

    code = xps_fapi_passfont (font->font, NULL, NULL, font->data, font->length);
    return code;
}
