static entity_table_opt determine_entity_table(int all, int doctype)
{
 	entity_table_opt retval = {NULL};
 
 	assert(!(doctype == ENT_HTML_DOC_XML1 && all));
 	if (all) {
 		retval.ms_table = (doctype == ENT_HTML_DOC_HTML5) ?
 			entity_ms_table_html5 : entity_ms_table_html4;
	} else {
		retval.table = (doctype == ENT_HTML_DOC_HTML401) ?
			stage3_table_be_noapos_00000 : stage3_table_be_apos_00000;
	}
	return retval;
}
