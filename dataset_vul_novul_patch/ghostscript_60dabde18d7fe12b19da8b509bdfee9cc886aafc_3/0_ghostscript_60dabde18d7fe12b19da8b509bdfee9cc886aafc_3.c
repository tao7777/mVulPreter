xps_parse_path(xps_document *doc, const fz_matrix *ctm, char *base_uri, xps_resource *dict, fz_xml *root)
{
	fz_xml *node;

	char *fill_uri;
	char *stroke_uri;
	char *opacity_mask_uri;

	char *transform_att;
	char *clip_att;
	char *data_att;
	char *fill_att;
	char *stroke_att;
	char *opacity_att;
	char *opacity_mask_att;

	fz_xml *transform_tag = NULL;
	fz_xml *clip_tag = NULL;
	fz_xml *data_tag = NULL;
	fz_xml *fill_tag = NULL;
	fz_xml *stroke_tag = NULL;
	fz_xml *opacity_mask_tag = NULL;

	char *fill_opacity_att = NULL;
	char *stroke_opacity_att = NULL;

	char *stroke_dash_array_att;
	char *stroke_dash_cap_att;
	char *stroke_dash_offset_att;
	char *stroke_end_line_cap_att;
	char *stroke_start_line_cap_att;
	char *stroke_line_join_att;
	char *stroke_miter_limit_att;
	char *stroke_thickness_att;
	char *navigate_uri_att;
 
        fz_stroke_state *stroke = NULL;
        fz_matrix transform;
       float samples[FZ_MAX_COLORS];
        fz_colorspace *colorspace;
        fz_path *path = NULL;
        fz_path *stroke_path = NULL;
	fz_rect area;
	int fill_rule;
	int dash_len = 0;
	fz_matrix local_ctm;

	/*
	 * Extract attributes and extended attributes.
	 */

	transform_att = fz_xml_att(root, "RenderTransform");
	clip_att = fz_xml_att(root, "Clip");
	data_att = fz_xml_att(root, "Data");
	fill_att = fz_xml_att(root, "Fill");
	stroke_att = fz_xml_att(root, "Stroke");
	opacity_att = fz_xml_att(root, "Opacity");
	opacity_mask_att = fz_xml_att(root, "OpacityMask");

	stroke_dash_array_att = fz_xml_att(root, "StrokeDashArray");
	stroke_dash_cap_att = fz_xml_att(root, "StrokeDashCap");
	stroke_dash_offset_att = fz_xml_att(root, "StrokeDashOffset");
	stroke_end_line_cap_att = fz_xml_att(root, "StrokeEndLineCap");
	stroke_start_line_cap_att = fz_xml_att(root, "StrokeStartLineCap");
	stroke_line_join_att = fz_xml_att(root, "StrokeLineJoin");
	stroke_miter_limit_att = fz_xml_att(root, "StrokeMiterLimit");
	stroke_thickness_att = fz_xml_att(root, "StrokeThickness");
	navigate_uri_att = fz_xml_att(root, "FixedPage.NavigateUri");

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "Path.RenderTransform"))
			transform_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Path.OpacityMask"))
			opacity_mask_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Path.Clip"))
			clip_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Path.Fill"))
			fill_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Path.Stroke"))
			stroke_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "Path.Data"))
			data_tag = fz_xml_down(node);
	}

	fill_uri = base_uri;
	stroke_uri = base_uri;
	opacity_mask_uri = base_uri;

	xps_resolve_resource_reference(doc, dict, &data_att, &data_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &clip_att, &clip_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &transform_att, &transform_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &fill_att, &fill_tag, &fill_uri);
	xps_resolve_resource_reference(doc, dict, &stroke_att, &stroke_tag, &stroke_uri);
	xps_resolve_resource_reference(doc, dict, &opacity_mask_att, &opacity_mask_tag, &opacity_mask_uri);

	/*
	 * Act on the information we have gathered:
	 */

	if (!data_att && !data_tag)
		return;

	if (fill_tag && !strcmp(fz_xml_tag(fill_tag), "SolidColorBrush"))
	{
		fill_opacity_att = fz_xml_att(fill_tag, "Opacity");
		fill_att = fz_xml_att(fill_tag, "Color");
		fill_tag = NULL;
	}

	if (stroke_tag && !strcmp(fz_xml_tag(stroke_tag), "SolidColorBrush"))
	{
		stroke_opacity_att = fz_xml_att(stroke_tag, "Opacity");
		stroke_att = fz_xml_att(stroke_tag, "Color");
		stroke_tag = NULL;
	}

	if (stroke_att || stroke_tag)
	{
		if (stroke_dash_array_att)
		{
			char *s = stroke_dash_array_att;

			while (*s)
			{
				while (*s == ' ')
					s++;
				if (*s) /* needed in case of a space before the last quote */
					dash_len++;

				while (*s && *s != ' ')
					s++;
			}
		}
		stroke = fz_new_stroke_state_with_dash_len(doc->ctx, dash_len);
		stroke->start_cap = xps_parse_line_cap(stroke_start_line_cap_att);
		stroke->dash_cap = xps_parse_line_cap(stroke_dash_cap_att);
		stroke->end_cap = xps_parse_line_cap(stroke_end_line_cap_att);

		stroke->linejoin = FZ_LINEJOIN_MITER_XPS;
		if (stroke_line_join_att)
		{
			if (!strcmp(stroke_line_join_att, "Miter")) stroke->linejoin = FZ_LINEJOIN_MITER_XPS;
			if (!strcmp(stroke_line_join_att, "Round")) stroke->linejoin = FZ_LINEJOIN_ROUND;
			if (!strcmp(stroke_line_join_att, "Bevel")) stroke->linejoin = FZ_LINEJOIN_BEVEL;
		}

		stroke->miterlimit = 10;
		if (stroke_miter_limit_att)
			stroke->miterlimit = fz_atof(stroke_miter_limit_att);

		stroke->linewidth = 1;
		if (stroke_thickness_att)
			stroke->linewidth = fz_atof(stroke_thickness_att);

		stroke->dash_phase = 0;
		stroke->dash_len = 0;
		if (stroke_dash_array_att)
		{
			char *s = stroke_dash_array_att;

			if (stroke_dash_offset_att)
				stroke->dash_phase = fz_atof(stroke_dash_offset_att) * stroke->linewidth;

			while (*s)
			{
				while (*s == ' ')
					s++;
				if (*s) /* needed in case of a space before the last quote */
					stroke->dash_list[stroke->dash_len++] = fz_atof(s) * stroke->linewidth;
				while (*s && *s != ' ')
					s++;
			}
			stroke->dash_len = dash_len;
		}
	}

	transform = fz_identity;
	if (transform_att)
		xps_parse_render_transform(doc, transform_att, &transform);
	if (transform_tag)
		xps_parse_matrix_transform(doc, transform_tag, &transform);
	fz_concat(&local_ctm, &transform, ctm);

	if (clip_att || clip_tag)
		xps_clip(doc, &local_ctm, dict, clip_att, clip_tag);

	fill_rule = 0;
	if (data_att)
		path = xps_parse_abbreviated_geometry(doc, data_att, &fill_rule);
	else if (data_tag)
	{
		path = xps_parse_path_geometry(doc, dict, data_tag, 0, &fill_rule);
		if (stroke_att || stroke_tag)
			stroke_path = xps_parse_path_geometry(doc, dict, data_tag, 1, &fill_rule);
	}
	if (!stroke_path)
		stroke_path = path;

	if (stroke_att || stroke_tag)
	{
		fz_bound_path(doc->ctx, stroke_path, stroke, &local_ctm, &area);
		if (stroke_path != path && (fill_att || fill_tag)) {
			fz_rect bounds;
			fz_bound_path(doc->ctx, path, NULL, &local_ctm, &bounds);
			fz_union_rect(&area, &bounds);
		}
	}
	else
		fz_bound_path(doc->ctx, path, NULL, &local_ctm, &area);

	if (navigate_uri_att)
		xps_add_link(doc, &area, base_uri, navigate_uri_att);

	xps_begin_opacity(doc, &local_ctm, &area, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	if (fill_att)
	{
		xps_parse_color(doc, base_uri, fill_att, &colorspace, samples);
		if (fill_opacity_att)
			samples[0] *= fz_atof(fill_opacity_att);
		xps_set_color(doc, colorspace, samples);

		fz_fill_path(doc->dev, path, fill_rule == 0, &local_ctm,
			doc->colorspace, doc->color, doc->alpha);
	}

	if (fill_tag)
	{
		fz_clip_path(doc->dev, path, &area, fill_rule == 0, &local_ctm);
		xps_parse_brush(doc, &local_ctm, &area, fill_uri, dict, fill_tag);
		fz_pop_clip(doc->dev);
	}

	if (stroke_att)
	{
		xps_parse_color(doc, base_uri, stroke_att, &colorspace, samples);
		if (stroke_opacity_att)
			samples[0] *= fz_atof(stroke_opacity_att);
		xps_set_color(doc, colorspace, samples);

		fz_stroke_path(doc->dev, stroke_path, stroke, &local_ctm,
			doc->colorspace, doc->color, doc->alpha);
	}

	if (stroke_tag)
	{
		fz_clip_stroke_path(doc->dev, stroke_path, &area, stroke, &local_ctm);
		xps_parse_brush(doc, &local_ctm, &area, stroke_uri, dict, stroke_tag);
		fz_pop_clip(doc->dev);
	}

	xps_end_opacity(doc, opacity_mask_uri, dict, opacity_att, opacity_mask_tag);

	if (stroke_path != path)
		fz_free_path(doc->ctx, stroke_path);
	fz_free_path(doc->ctx, path);
	path = NULL;
	fz_drop_stroke_state(doc->ctx, stroke);

	if (clip_att || clip_tag)
		fz_pop_clip(doc->dev);
}
