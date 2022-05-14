static void Process_ipfix_template_add(exporter_ipfix_domain_t *exporter, void *DataPtr, uint32_t size_left, FlowSource_t *fs) {
input_translation_t *translation_table;
ipfix_template_record_t *ipfix_template_record;
ipfix_template_elements_std_t *NextElement;
int i;

	while ( size_left ) {
 		uint32_t table_id, count, size_required;
 		uint32_t num_extensions = 0;
 
		if ( size_left < 4 ) {
 			LogError("Process_ipfix [%u] Template size error at %s line %u" , 
 				exporter->info.id, __FILE__, __LINE__, strerror (errno));
 			size_left = 0;
			continue;
		}

		ipfix_template_record = (ipfix_template_record_t *)DataPtr;
		size_left 		-= 4;

		table_id = ntohs(ipfix_template_record->TemplateID);
		count	 = ntohs(ipfix_template_record->FieldCount);

		dbg_printf("\n[%u] Template ID: %u\n", exporter->info.id, table_id);
		dbg_printf("FieldCount: %u buffersize: %u\n", count, size_left);

		memset((void *)cache.common_extensions, 0,  (Max_num_extensions+1)*sizeof(uint32_t));
		memset((void *)cache.lookup_info, 0, 65536 * sizeof(struct element_param_s));
		for (i=1; ipfix_element_map[i].id != 0; i++ ) {
			uint32_t Type = ipfix_element_map[i].id;
			if ( ipfix_element_map[i].id == ipfix_element_map[i-1].id )
				continue;
			cache.lookup_info[Type].index   = i;
		}
		cache.input_order = calloc(count, sizeof(struct order_s));
		if ( !cache.input_order ) {
			LogError("Process_ipfix: Panic! malloc(): %s line %d: %s", __FILE__, __LINE__, strerror (errno));
			size_left = 0;
			continue;
		}

		cache.input_count = count;

		size_required   = 4*count;
		if ( size_left < size_required ) {
			LogError("Process_ipfix: [%u] Not enough data for template elements! required: %i, left: %u", 
					exporter->info.id, size_required, size_left);
			dbg_printf("ERROR: Not enough data for template elements! required: %i, left: %u", size_required, size_left);
			return;
		}

		NextElement 	 = (ipfix_template_elements_std_t *)ipfix_template_record->elements;
		for ( i=0; i<count; i++ ) {
			uint16_t Type, Length;
			uint32_t ext_id;
			int Enterprise;
	
			Type   = ntohs(NextElement->Type);
			Length = ntohs(NextElement->Length);
			Enterprise = Type & 0x8000 ? 1 : 0;
			Type = Type & 0x7FFF;

			ext_id = MapElement(Type, Length, i);

			if ( ext_id && extension_descriptor[ext_id].enabled ) {
				if ( cache.common_extensions[ext_id] == 0 ) {
					cache.common_extensions[ext_id] = 1;
					num_extensions++;
				}
			} 
	
			if ( Enterprise ) {
				ipfix_template_elements_e_t *e = (ipfix_template_elements_e_t *)NextElement;
				size_required += 4;	// ad 4 for enterprise value
				if ( size_left < size_required ) {
					LogError("Process_ipfix: [%u] Not enough data for template elements! required: %i, left: %u", 
							exporter->info.id, size_required, size_left);
					dbg_printf("ERROR: Not enough data for template elements! required: %i, left: %u", size_required, size_left);
					return;
				}
				if ( ntohl(e->EnterpriseNumber) == IPFIX_ReverseInformationElement ) {
					dbg_printf(" [%i] Enterprise: 1, Type: %u, Length %u Reverse Information Element: %u\n", i, Type, Length, ntohl(e->EnterpriseNumber));
				} else {
					dbg_printf(" [%i] Enterprise: 1, Type: %u, Length %u EnterpriseNumber: %u\n", i, Type, Length, ntohl(e->EnterpriseNumber));
				}
				e++;
				NextElement = (ipfix_template_elements_std_t *)e;
			} else {
				dbg_printf(" [%i] Enterprise: 0, Type: %u, Length %u\n", i, Type, Length);
				NextElement++;
			}
		}

		dbg_printf("Processed: %u\n", size_required);

		if ( compact_input_order() ) {

			if ( extension_descriptor[EX_ROUTER_IP_v4].enabled ) {
				if ( cache.common_extensions[EX_ROUTER_IP_v4] == 0 ) {
					cache.common_extensions[EX_ROUTER_IP_v4] = 1;
					num_extensions++;
				}
				dbg_printf("Add sending router IP address (%s) => Extension: %u\n", 
					fs->sa_family == PF_INET6 ? "ipv6" : "ipv4", EX_ROUTER_IP_v4);
			}
	
			extension_descriptor[EX_ROUTER_ID].enabled = 0;

/*	
		if ( extension_descriptor[EX_ROUTER_ID].enabled ) {
			if ( cache.common_extensions[EX_ROUTER_ID] == 0 ) {
				cache.common_extensions[EX_ROUTER_ID] = 1;
				num_extensions++;
			}
			dbg_printf("Force add router ID (engine type/ID), Extension: %u\n", EX_ROUTER_ID);
		}
*/
			if ( extension_descriptor[EX_RECEIVED].enabled ) {
				if ( cache.common_extensions[EX_RECEIVED] == 0 ) {
					cache.common_extensions[EX_RECEIVED] = 1;
					num_extensions++;
				}
				dbg_printf("Force add packet received time, Extension: %u\n", EX_RECEIVED);
			}

#ifdef DEVEL
		{
			int i;
			for (i=4; extension_descriptor[i].id; i++ ) {
				if ( cache.common_extensions[i] ) {
					printf("Enabled extension: %i\n", i);
				}
			}
		}
#endif
	
			translation_table = setup_translation_table(exporter, table_id);
			if (translation_table->extension_map_changed ) {
				dbg_printf("Translation Table changed! Add extension map ID: %i\n", translation_table->extension_info.map->map_id);
				AddExtensionMap(fs, translation_table->extension_info.map);
				translation_table->extension_map_changed = 0;
				dbg_printf("Translation Table added! map ID: %i\n", translation_table->extension_info.map->map_id);
			}
	
			if ( !reorder_sequencer(translation_table) ) {
				LogError("Process_ipfix: [%u] Failed to reorder sequencer. Remove table id: %u", 
							exporter->info.id, table_id);
				remove_translation_table(fs, exporter, table_id);
			}
		} else {
			dbg_printf("Template does not contain any common fields - skip\n");
		}
		size_left -= size_required;
		DataPtr = DataPtr + size_required+4;	// +4 for header
		if ( size_left < 4 ) {
			dbg_printf("Skip %u bytes padding\n", size_left);
			size_left = 0;
		}
		free(cache.input_order);
		cache.input_order = NULL;
	}

} // End of Process_ipfix_template_add
