xps_parse_glyphs(xps_document *doc, const fz_matrix *ctm,
		char *base_uri, xps_resource *dict, fz_xml *root)
{
	fz_xml *node;

	char *fill_uri;
	char *opacity_mask_uri;

	char *bidi_level_att;
	char *fill_att;
	char *font_size_att;
	char *font_uri_att;
	char *origin_x_att;
	char *origin_y_att;
	char *is_sideways_att;
	char *indices_att;
	char *unicode_att;
	char *style_att;
	char *transform_att;
	char *clip_att;
	char *opacity_att;
	char *opacity_mask_att;
	char *navigate_uri_att;

	fz_xml *transform_tag = NULL;
	fz_xml *clip_tag = NULL;
	fz_xml *fill_tag = NULL;
	fz_xml *opacity_mask_tag = NULL;

	char *fill_opacity_att = NULL;

	xps_part *part;
	fz_font *font;

	char partname[1024];
	char fakename[1024];
	char *subfont;

	float font_size = 10;
	int subfontid = 0;
	int is_sideways = 0;
	int bidi_level = 0;

	fz_text *text;
	fz_rect area;

	fz_matrix local_ctm = *ctm;

	/*
	 * Extract attributes and extended attributes.
	 */

	bidi_level_att = fz_xml_att(root, "BidiLevel");
	fill_att = fz_xml_att(root, "Fill");
	font_size_att = fz_xml_att(root, "FontRenderingEmSize");
	font_uri_att = fz_xml_att(root, "FontUri");
	origin_x_att = fz_xml_att(root, "OriginX");
	origin_y_att = fz_xml_att(root, "OriginY");
	is_sideways_att = fz_xml_att(root, "IsSideways");
	indices_att = fz_xml_att(root, "Indices");
	unicode_att = fz_xml_att(root, "UnicodeString");
	style_att = fz_xml_att(root, "StyleSimulations");
	transform_att = fz_xml_att(root, "RenderTransform");
	clip_att = fz_xml_att(root, "Clip");
	opacity_att = fz_xml_att(root, "Opacity");
	opacity_mask_att = fz_xml_att(root, "OpacityMask");
	navigate_uri_att = fz_xml_att(root, "FixedPage.NavigateUri");

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "Glyphs.RenderTransform"))
			transform_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Glyphs.OpacityMask"))
			opacity_mask_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Glyphs.Clip"))
			clip_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Glyphs.Fill"))
			fill_tag = fz_xml_down(node);
	}

	fill_uri = base_uri;
	opacity_mask_uri = base_uri;

	xps_resolve_resource_reference(doc, dict, &transform_att, &transform_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &clip_att, &clip_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &fill_att, &fill_tag, &fill_uri);
	xps_resolve_resource_reference(doc, dict, &opacity_mask_att, &opacity_mask_tag, &opacity_mask_uri);

	/*
	 * Check that we have all the necessary information.
	 */

	if (!font_size_att || !font_uri_att || !origin_x_att || !origin_y_att) {
		fz_warn(doc->ctx, "missing attributes in glyphs element");
		return;
	}

	if (!indices_att && !unicode_att)
		return; /* nothing to draw */

	if (is_sideways_att)
		is_sideways = !strcmp(is_sideways_att, "true");

	if (bidi_level_att)
		bidi_level = atoi(bidi_level_att);

	/*
	 * Find and load the font resource
	 */

	xps_resolve_url(partname, base_uri, font_uri_att, sizeof partname);
	subfont = strrchr(partname, '#');
	if (subfont)
	{
		subfontid = atoi(subfont + 1);
		*subfont = 0;
	}

	/* Make a new part name for font with style simulation applied */
	fz_strlcpy(fakename, partname, sizeof fakename);
	if (style_att)
	{
		if (!strcmp(style_att, "BoldSimulation"))
			fz_strlcat(fakename, "#Bold", sizeof fakename);
		else if (!strcmp(style_att, "ItalicSimulation"))
			fz_strlcat(fakename, "#Italic", sizeof fakename);
		else if (!strcmp(style_att, "BoldItalicSimulation"))
			fz_strlcat(fakename, "#BoldItalic", sizeof fakename);
	}

	font = xps_lookup_font(doc, fakename);
	if (!font)
	{
		fz_try(doc->ctx)
		{
			part = xps_read_part(doc, partname);
		}
		fz_catch(doc->ctx)
		{
			fz_rethrow_if(doc->ctx, FZ_ERROR_TRYLATER);
			fz_warn(doc->ctx, "cannot find font resource part '%s'", partname);
			return;
		}

		/* deobfuscate if necessary */
		if (strstr(part->name, ".odttf"))
			xps_deobfuscate_font_resource(doc, part);
		if (strstr(part->name, ".ODTTF"))
			xps_deobfuscate_font_resource(doc, part);

		fz_try(doc->ctx)
		{
			fz_buffer *buf = fz_new_buffer_from_data(doc->ctx, part->data, part->size);
			font = fz_new_font_from_buffer(doc->ctx, NULL, buf, subfontid, 1);
			fz_drop_buffer(doc->ctx, buf);
		}
		fz_catch(doc->ctx)
		{
			fz_rethrow_if(doc->ctx, FZ_ERROR_TRYLATER);
			fz_warn(doc->ctx, "cannot load font resource '%s'", partname);
			xps_free_part(doc, part);
			return;
		}

		if (style_att)
		{
			font->ft_bold = !!strstr(style_att, "Bold");
			font->ft_italic = !!strstr(style_att, "Italic");
		}

		xps_select_best_font_encoding(doc, font);

		xps_insert_font(doc, fakename, font);

		/* NOTE: we already saved part->data in the buffer in the font */
		fz_free(doc->ctx, part->name);
		fz_free(doc->ctx, part);
	}

	/*
	 * Set up graphics state.
	 */

	if (transform_att || transform_tag)
	{
		fz_matrix transform;
		if (transform_att)
			xps_parse_render_transform(doc, transform_att, &transform);
		if (transform_tag)
			xps_parse_matrix_transform(doc, transform_tag, &transform);
		fz_concat(&local_ctm, &transform, &local_ctm);
	}

	if (clip_att || clip_tag)
		xps_clip(doc, &local_ctm, dict, clip_att, clip_tag);

	font_size = fz_atof(font_size_att);

	text = xps_parse_glyphs_imp(doc, &local_ctm, font, font_size,
			fz_atof(origin_x_att), fz_atof(origin_y_att),
			is_sideways, bidi_level, indices_att, unicode_att);

	fz_bound_text(doc->ctx, text, NULL, &local_ctm, &area);

	if (navigate_uri_att)
		xps_add_link(doc, &area, base_uri, navigate_uri_att);

	xps_begin_opacity(doc, &local_ctm, &area, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	/* If it's a solid color brush fill/stroke do a simple fill */

	if (fill_tag && !strcmp(fz_xml_tag(fill_tag), "SolidColorBrush"))
	{
		fill_opacity_att = fz_xml_att(fill_tag, "Opacity");
		fill_att = fz_xml_att(fill_tag, "Color");
		fill_tag = NULL;
	}
 
        if (fill_att)
        {
               float samples[FZ_MAX_COLORS];
                fz_colorspace *colorspace;
 
                xps_parse_color(doc, base_uri, fill_att, &colorspace, samples);
		if (fill_opacity_att)
			samples[0] *= fz_atof(fill_opacity_att);
		xps_set_color(doc, colorspace, samples);

		fz_fill_text(doc->dev, text, &local_ctm,
			doc->colorspace, doc->color, doc->alpha);
	}

	/* If it's a complex brush, use the charpath as a clip mask */

	if (fill_tag)
	{
		fz_clip_text(doc->dev, text, &local_ctm, 0);
		xps_parse_brush(doc, &local_ctm, &area, fill_uri, dict, fill_tag);
		fz_pop_clip(doc->dev);
	}

	xps_end_opacity(doc, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	fz_free_text(doc->ctx, text);

	if (clip_att || clip_tag)
		fz_pop_clip(doc->dev);

	fz_drop_font(doc->ctx, font);
}
