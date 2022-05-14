setup_efi_state(struct boot_params *params, unsigned long params_load_addr,
		unsigned int efi_map_offset, unsigned int efi_map_sz,
		unsigned int efi_setup_data_offset)
{
	struct efi_info *current_ei = &boot_params.efi_info;
	struct efi_info *ei = &params->efi_info;

	if (!current_ei->efi_memmap_size)
		return 0;

	/*
	 * If 1:1 mapping is not enabled, second kernel can not setup EFI
	 * and use EFI run time services. User space will have to pass
	 * acpi_rsdp=<addr> on kernel command line to make second kernel boot
	 * without efi.
	 */
 	if (efi_enabled(EFI_OLD_MEMMAP))
 		return 0;
 
	params->secure_boot = boot_params.secure_boot;
 	ei->efi_loader_signature = current_ei->efi_loader_signature;
 	ei->efi_systab = current_ei->efi_systab;
 	ei->efi_systab_hi = current_ei->efi_systab_hi;

	ei->efi_memdesc_version = current_ei->efi_memdesc_version;
	ei->efi_memdesc_size = efi_get_runtime_map_desc_size();

	setup_efi_info_memmap(params, params_load_addr, efi_map_offset,
			      efi_map_sz);
	prepare_add_efi_setup_data(params, params_load_addr,
				   efi_setup_data_offset);
	return 0;
}
