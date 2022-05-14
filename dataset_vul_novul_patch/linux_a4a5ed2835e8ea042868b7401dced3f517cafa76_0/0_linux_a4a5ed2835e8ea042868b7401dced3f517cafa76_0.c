void __init acpi_initrd_override(void *data, size_t size)
{
	int sig, no, table_nr = 0, total_offset = 0;
	long offset = 0;
	struct acpi_table_header *table;
	char cpio_path[32] = "kernel/firmware/acpi/";
	struct cpio_data file;

	if (data == NULL || size == 0)
		return;

	for (no = 0; no < ACPI_OVERRIDE_TABLES; no++) {
		file = find_cpio_data(cpio_path, data, size, &offset);
		if (!file.data)
			break;

		data += offset;
		size -= offset;

		if (file.size < sizeof(struct acpi_table_header)) {
			pr_err("ACPI OVERRIDE: Table smaller than ACPI header [%s%s]\n",
				cpio_path, file.name);
			continue;
		}

		table = file.data;

		for (sig = 0; table_sigs[sig]; sig++)
			if (!memcmp(table->signature, table_sigs[sig], 4))
				break;

		if (!table_sigs[sig]) {
			pr_err("ACPI OVERRIDE: Unknown signature [%s%s]\n",
				cpio_path, file.name);
			continue;
		}
		if (file.size != table->length) {
			pr_err("ACPI OVERRIDE: File length does not match table length [%s%s]\n",
				cpio_path, file.name);
			continue;
		}
		if (acpi_table_checksum(file.data, table->length)) {
			pr_err("ACPI OVERRIDE: Bad table checksum [%s%s]\n",
				cpio_path, file.name);
			continue;
		}

		pr_info("%4.4s ACPI table found in initrd [%s%s][0x%x]\n",
			table->signature, cpio_path, file.name, table->length);

		all_tables_size += table->length;
		acpi_initrd_files[table_nr].data = file.data;
		acpi_initrd_files[table_nr].size = file.size;
		table_nr++;
	}
 	if (table_nr == 0)
 		return;
 
	if (get_securelevel() > 0) {
		pr_notice(PREFIX
			"securelevel enabled, ignoring table override\n");
		return;
	}

 	acpi_tables_addr =
 		memblock_find_in_range(0, max_low_pfn_mapped << PAGE_SHIFT,
 				       all_tables_size, PAGE_SIZE);
	if (!acpi_tables_addr) {
		WARN_ON(1);
		return;
	}
	/*
	 * Only calling e820_add_reserve does not work and the
	 * tables are invalid (memory got used) later.
	 * memblock_reserve works as expected and the tables won't get modified.
	 * But it's not enough on X86 because ioremap will
	 * complain later (used by acpi_os_map_memory) that the pages
	 * that should get mapped are not marked "reserved".
	 * Both memblock_reserve and e820_add_region (via arch_reserve_mem_area)
	 * works fine.
	 */
	memblock_reserve(acpi_tables_addr, all_tables_size);
	arch_reserve_mem_area(acpi_tables_addr, all_tables_size);

	/*
	 * early_ioremap only can remap 256k one time. If we map all
	 * tables one time, we will hit the limit. Need to map chunks
	 * one by one during copying the same as that in relocate_initrd().
	 */
	for (no = 0; no < table_nr; no++) {
		unsigned char *src_p = acpi_initrd_files[no].data;
		phys_addr_t size = acpi_initrd_files[no].size;
		phys_addr_t dest_addr = acpi_tables_addr + total_offset;
		phys_addr_t slop, clen;
		char *dest_p;

		total_offset += size;

		while (size) {
			slop = dest_addr & ~PAGE_MASK;
			clen = size;
			if (clen > MAP_CHUNK_SIZE - slop)
				clen = MAP_CHUNK_SIZE - slop;
			dest_p = early_ioremap(dest_addr & PAGE_MASK,
						 clen + slop);
			memcpy(dest_p + slop, src_p, clen);
			early_iounmap(dest_p, clen + slop);
			src_p += clen;
			dest_addr += clen;
			size -= clen;
		}
	}
}
