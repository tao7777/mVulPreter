xps_begin_opacity(xps_document *doc, const fz_matrix *ctm, const fz_rect *area,
	char *base_uri, xps_resource *dict,
	char *opacity_att, fz_xml *opacity_mask_tag)
{
	float opacity;

	if (!opacity_att && !opacity_mask_tag)
		return;

	opacity = 1;
	if (opacity_att)
		opacity = fz_atof(opacity_att);

	if (opacity_mask_tag && !strcmp(fz_xml_tag(opacity_mask_tag), "SolidColorBrush"))
	{
		char *scb_opacity_att = fz_xml_att(opacity_mask_tag, "Opacity");
		char *scb_color_att = fz_xml_att(opacity_mask_tag, "Color");
		if (scb_opacity_att)
			opacity = opacity * fz_atof(scb_opacity_att);
                if (scb_color_att)
                {
                        fz_colorspace *colorspace;
                       float samples[FZ_MAX_COLORS];
                        xps_parse_color(doc, base_uri, scb_color_att, &colorspace, samples);
                        opacity = opacity * samples[0];
                }
		opacity_mask_tag = NULL;
	}

	if (doc->opacity_top + 1 < nelem(doc->opacity))
	{
		doc->opacity[doc->opacity_top + 1] = doc->opacity[doc->opacity_top] * opacity;
		doc->opacity_top++;
	}

	if (opacity_mask_tag)
	{
		fz_begin_mask(doc->dev, area, 0, NULL, NULL);
		xps_parse_brush(doc, ctm, area, base_uri, dict, opacity_mask_tag);
		fz_end_mask(doc->dev);
	}
}
