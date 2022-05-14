xps_parse_gradient_stops(xps_document *doc, char *base_uri, fz_xml *node,
        struct stop *stops, int maxcount)
 {
        fz_colorspace *colorspace;
       float sample[FZ_MAX_COLORS];
        float rgb[3];
        int before, after;
        int count;
	int i;

	/* We may have to insert 2 extra stops when postprocessing */
	maxcount -= 2;

	count = 0;
	while (node && count < maxcount)
	{
		if (!strcmp(fz_xml_tag(node), "GradientStop"))
		{
			char *offset = fz_xml_att(node, "Offset");
			char *color = fz_xml_att(node, "Color");
			if (offset && color)
			{
				stops[count].offset = fz_atof(offset);
				stops[count].index = count;

				xps_parse_color(doc, base_uri, color, &colorspace, sample);

				fz_convert_color(doc->ctx, fz_device_rgb(doc->ctx), rgb, colorspace, sample + 1);

				stops[count].r = rgb[0];
				stops[count].g = rgb[1];
				stops[count].b = rgb[2];
				stops[count].a = sample[0];

				count ++;
			}
		}
		node = fz_xml_next(node);
	}

	if (count == 0)
	{
		fz_warn(doc->ctx, "gradient brush has no gradient stops");
		stops[0].offset = 0;
		stops[0].r = 0;
		stops[0].g = 0;
		stops[0].b = 0;
		stops[0].a = 1;
		stops[1].offset = 1;
		stops[1].r = 1;
		stops[1].g = 1;
		stops[1].b = 1;
		stops[1].a = 1;
		return 2;
	}

	if (count == maxcount)
		fz_warn(doc->ctx, "gradient brush exceeded maximum number of gradient stops");

	/* Postprocess to make sure the range of offsets is 0.0 to 1.0 */

	qsort(stops, count, sizeof(struct stop), cmp_stop);

	before = -1;
	after = -1;

	for (i = 0; i < count; i++)
	{
		if (stops[i].offset < 0)
			before = i;
		if (stops[i].offset > 1)
		{
			after = i;
			break;
		}
	}

	/* Remove all stops < 0 except the largest one */
	if (before > 0)
	{
		memmove(stops, stops + before, (count - before) * sizeof(struct stop));
		count -= before;
	}

	/* Remove all stops > 1 except the smallest one */
	if (after >= 0)
		count = after + 1;

	/* Expand single stop to 0 .. 1 */
	if (count == 1)
	{
		stops[1] = stops[0];
		stops[0].offset = 0;
		stops[1].offset = 1;
		return 2;
	}

	/* First stop < 0 -- interpolate value to 0 */
	if (stops[0].offset < 0)
	{
		float d = -stops[0].offset / (stops[1].offset - stops[0].offset);
		stops[0].offset = 0;
		stops[0].r = lerp(stops[0].r, stops[1].r, d);
		stops[0].g = lerp(stops[0].g, stops[1].g, d);
		stops[0].b = lerp(stops[0].b, stops[1].b, d);
		stops[0].a = lerp(stops[0].a, stops[1].a, d);
	}

	/* Last stop > 1 -- interpolate value to 1 */
	if (stops[count-1].offset > 1)
	{
		float d = (1 - stops[count-2].offset) / (stops[count-1].offset - stops[count-2].offset);
		stops[count-1].offset = 1;
		stops[count-1].r = lerp(stops[count-2].r, stops[count-1].r, d);
		stops[count-1].g = lerp(stops[count-2].g, stops[count-1].g, d);
		stops[count-1].b = lerp(stops[count-2].b, stops[count-1].b, d);
		stops[count-1].a = lerp(stops[count-2].a, stops[count-1].a, d);
	}

	/* First stop > 0 -- insert a duplicate at 0 */
	if (stops[0].offset > 0)
	{
		memmove(stops + 1, stops, count * sizeof(struct stop));
		stops[0] = stops[1];
		stops[0].offset = 0;
		count++;
	}

	/* Last stop < 1 -- insert a duplicate at 1 */
	if (stops[count-1].offset < 1)
	{
		stops[count] = stops[count-1];
		stops[count].offset = 1;
		count++;
	}

	return count;
}
