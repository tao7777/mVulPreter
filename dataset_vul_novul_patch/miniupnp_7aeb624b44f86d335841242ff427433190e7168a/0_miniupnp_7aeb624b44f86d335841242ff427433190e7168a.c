ParseNameValue(const char * buffer, int bufsize,
                struct NameValueParserData * data)
 {
 	struct xmlparser parser;
	memset(data, 0, sizeof(struct NameValueParserData));
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
