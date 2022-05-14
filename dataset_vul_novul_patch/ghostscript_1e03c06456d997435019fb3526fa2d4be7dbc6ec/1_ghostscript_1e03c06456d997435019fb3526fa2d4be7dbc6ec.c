pdf_dict_put(fz_context *ctx, pdf_obj *obj, pdf_obj *key, pdf_obj *val)
 {
        int i;
 
        RESOLVE(obj);
        if (!OBJ_IS_DICT(obj))
                fz_throw(ctx, FZ_ERROR_GENERIC, "not a dict (%s)", pdf_objkindstr(obj));
	if (!val)
		val = PDF_OBJ_NULL;

	if (DICT(obj)->len > 100 && !(obj->flags & PDF_FLAGS_SORTED))
		pdf_sort_dict(ctx, obj);

	if (key < PDF_OBJ_NAME__LIMIT)
		i = pdf_dict_find(ctx, obj, key);
	else
		i = pdf_dict_finds(ctx, obj, pdf_to_name(ctx, key));

	prepare_object_for_alteration(ctx, obj, val);

	if (i >= 0 && i < DICT(obj)->len)
	{
		if (DICT(obj)->items[i].v != val)
		{
			pdf_obj *d = DICT(obj)->items[i].v;
			DICT(obj)->items[i].v = pdf_keep_obj(ctx, val);
                {
                        pdf_obj *d = DICT(obj)->items[i].v;
                        DICT(obj)->items[i].v = pdf_keep_obj(ctx, val);
                       pdf_drop_obj(ctx, d);
                }
        }
        else
			memmove(&DICT(obj)->items[i + 1],
					&DICT(obj)->items[i],
					(DICT(obj)->len - i) * sizeof(struct keyval));

		DICT(obj)->items[i].k = pdf_keep_obj(ctx, key);
		DICT(obj)->items[i].v = pdf_keep_obj(ctx, val);
		DICT(obj)->len ++;
	}
}
