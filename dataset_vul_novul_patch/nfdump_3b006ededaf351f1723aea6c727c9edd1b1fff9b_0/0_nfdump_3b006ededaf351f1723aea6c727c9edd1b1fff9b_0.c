static void Process_ipfix_template_withdraw(exporter_ipfix_domain_t *exporter, void *DataPtr, uint32_t size_left, FlowSource_t *fs) {
ipfix_template_record_t *ipfix_template_record;

 	while ( size_left ) {
 		uint32_t id;
 
		if ( size_left < 4 ) {
			LogError("Process_ipfix [%u] Template withdraw size error at %s line %u" , 
				exporter->info.id, __FILE__, __LINE__, strerror (errno));
			size_left = 0;
			continue;
		}


 		ipfix_template_record = (ipfix_template_record_t *)DataPtr;
 		size_left 		-= 4;

		id 	  = ntohs(ipfix_template_record->TemplateID);

		if ( id == IPFIX_TEMPLATE_FLOWSET_ID ) {
			remove_all_translation_tables(exporter);
			ReInitExtensionMapList(fs);
		} else {
			remove_translation_table(fs, exporter, id);
		}

		DataPtr = DataPtr + 4;
		if ( size_left < 4 ) {
			dbg_printf("Skip %u bytes padding\n", size_left);
			size_left = 0;
		}
	}
 
} // End of Process_ipfix_template_withdraw
