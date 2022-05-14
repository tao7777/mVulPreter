struct addr_t* MACH0_(get_entrypoint)(struct MACH0_(obj_t)* bin) {
	struct addr_t *entry;
	int i;

	if (!bin->entry && !bin->sects) {
		return NULL;
	}
	if (!(entry = calloc (1, sizeof (struct addr_t)))) {
		return NULL;
	}

	if (bin->entry) {
		entry->addr = entry_to_vaddr (bin);
		entry->offset = addr_to_offset (bin, entry->addr);
		entry->haddr = sdb_num_get (bin->kv, "mach0.entry.offset", 0);
	}

	if (!bin->entry || entry->offset == 0) {
		for (i = 0; i < bin->nsects; i++) {
			if (!strncmp (bin->sects[i].sectname, "__text", 6)) {
				entry->offset = (ut64)bin->sects[i].offset;
				sdb_num_set (bin->kv, "mach0.entry", entry->offset, 0);
				entry->addr = (ut64)bin->sects[i].addr;
				if (!entry->addr) { // workaround for object files
					entry->addr = entry->offset;
				}
				break;
			}
 		}
 		bin->entry = entry->addr;
 	}
 	return entry;
 }
