ParseNameValue(const char * buffer, int bufsize,
                struct NameValueParserData * data)
 {
 	struct xmlparser parser;
	data->l_head = NULL;
	data->portListing = NULL;
	data->portListingLength = 0;
 	/* init xmlparser object */
 	parser.xmlstart = buffer;
 	parser.xmlsize = bufsize;
	parser.data = data;
	parser.starteltfunc = NameValueParserStartElt;
	parser.endeltfunc = NameValueParserEndElt;
	parser.datafunc = NameValueParserGetData;
	parser.attfunc = 0;
	parsexml(&parser);
}
