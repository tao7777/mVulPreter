GF_Err hdlr_dump(GF_Box *a, FILE * trace)
 {
 	GF_HandlerBox *p = (GF_HandlerBox *)a;
 	gf_isom_box_dump_start(a, "HandlerBox", trace);
	if (p->nameUTF8 && (u32) p->nameUTF8[0] == strlen(p->nameUTF8)-1) {
 		fprintf(trace, "hdlrType=\"%s\" Name=\"%s\" ", gf_4cc_to_str(p->handlerType), p->nameUTF8+1);
 	} else {
 		fprintf(trace, "hdlrType=\"%s\" Name=\"%s\" ", gf_4cc_to_str(p->handlerType), p->nameUTF8);
	}
	fprintf(trace, "reserved1=\"%d\" reserved2=\"", p->reserved1);
	dump_data(trace, (char *) p->reserved2, 12);
	fprintf(trace, "\"");

	fprintf(trace, ">\n");
	gf_isom_box_dump_done("HandlerBox", a, trace);
	return GF_OK;
}
