void jp2_box_dump(jp2_box_t *box, FILE *out)
{
	jp2_boxinfo_t *boxinfo;
	boxinfo = jp2_boxinfolookup(box->type);
 	assert(boxinfo);
 
 	fprintf(out, "JP2 box: ");
	fprintf(out, "type=%c%s%c (0x%08"PRIxFAST32"); length=%"PRIuFAST32"\n", '"', boxinfo->name,
	  '"', box->type, box->len);
 	if (box->ops->dumpdata) {
 		(*box->ops->dumpdata)(box, out);
 	}
}
