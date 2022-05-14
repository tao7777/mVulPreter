jp2_box_t *jp2_box_create(int type)
jp2_box_t *jp2_box_create0()
 {
 	jp2_box_t *box;
 	if (!(box = jas_malloc(sizeof(jp2_box_t)))) {
 		return 0;
 	}
 	memset(box, 0, sizeof(jp2_box_t));
	box->type = 0;
	box->len = 0;
	// Mark the box data as never having been constructed
	// so that we will not errantly attempt to destroy it later.
	box->ops = &jp2_boxinfo_unk.ops;
	return box;
}

jp2_box_t *jp2_box_create(int type)
{
	jp2_box_t *box;
	jp2_boxinfo_t *boxinfo;
	if (!(box = jp2_box_create0())) {
		return 0;
	}
 	box->type = type;
 	box->len = 0;
 	if (!(boxinfo = jp2_boxinfolookup(type))) {
		return 0;
	}
	box->info = boxinfo;
	box->ops = &boxinfo->ops;
	return box;
}
