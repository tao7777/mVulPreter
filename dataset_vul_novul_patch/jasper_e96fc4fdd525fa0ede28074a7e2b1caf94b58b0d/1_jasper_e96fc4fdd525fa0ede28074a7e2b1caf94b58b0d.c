jp2_box_t *jp2_box_create(int type)
 {
 	jp2_box_t *box;
	jp2_boxinfo_t *boxinfo;
 	if (!(box = jas_malloc(sizeof(jp2_box_t)))) {
 		return 0;
 	}
 	memset(box, 0, sizeof(jp2_box_t));
 	box->type = type;
 	box->len = 0;
 	if (!(boxinfo = jp2_boxinfolookup(type))) {
		return 0;
	}
	box->info = boxinfo;
	box->ops = &boxinfo->ops;
	return box;
}
