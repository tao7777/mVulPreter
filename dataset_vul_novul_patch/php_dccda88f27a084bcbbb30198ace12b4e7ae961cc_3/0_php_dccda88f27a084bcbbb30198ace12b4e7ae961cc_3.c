static void _xml_add_to_info(xml_parser *parser,char *name)
{
	zval **element, *values;

	if (! parser->info) {
		return;
	}
 
        if (zend_hash_find(Z_ARRVAL_P(parser->info),name,strlen(name) + 1,(void **) &element) == FAILURE) {
                MAKE_STD_ZVAL(values);

                array_init(values);

                zend_hash_update(Z_ARRVAL_P(parser->info), name, strlen(name)+1, (void *) &values, sizeof(zval*), (void **) &element);
       }

        add_next_index_long(*element,parser->curtag);

        parser->curtag++;
 }
