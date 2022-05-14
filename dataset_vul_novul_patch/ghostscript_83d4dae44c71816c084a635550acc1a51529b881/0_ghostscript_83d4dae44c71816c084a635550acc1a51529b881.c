void fz_init_cached_color_converter(fz_context *ctx, fz_color_converter *cc, fz_colorspace *is, fz_colorspace *ds, fz_colorspace *ss, const fz_color_params *params)
{
	int n = ss->n;
	fz_cached_color_converter *cached = fz_malloc_struct(ctx, fz_cached_color_converter);

	cc->opaque = cached;
	cc->convert = fz_cached_color_convert;
	cc->ds = ds ? ds : fz_device_gray(ctx);
	cc->ss = ss;
	cc->is = is;

	fz_try(ctx)
	{
		fz_find_color_converter(ctx, &cached->base, is, cc->ds, ss, params);
		cached->hash = fz_new_hash_table(ctx, 256, n * sizeof(float), -1, fz_free);
	}
	fz_catch(ctx)
	{
                fz_drop_color_converter(ctx, &cached->base);
                fz_drop_hash_table(ctx, cached->hash);
                fz_free(ctx, cached);
               cc->opaque = NULL;
                fz_rethrow(ctx);
        }
 }
