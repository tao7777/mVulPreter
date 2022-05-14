void __init setup_arch(char **cmdline_p)
{
	memblock_reserve(__pa_symbol(_text),
			 (unsigned long)__bss_stop - (unsigned long)_text);

	early_reserve_initrd();

	/*
	 * At this point everything still needed from the boot loader
	 * or BIOS or kernel text should be early reserved or marked not
	 * RAM in e820. All other memory is free game.
	 */

#ifdef CONFIG_X86_32
	memcpy(&boot_cpu_data, &new_cpu_data, sizeof(new_cpu_data));

	/*
	 * copy kernel address range established so far and switch
	 * to the proper swapper page table
	 */
	clone_pgd_range(swapper_pg_dir     + KERNEL_PGD_BOUNDARY,
			initial_page_table + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);

	load_cr3(swapper_pg_dir);
	/*
	 * Note: Quark X1000 CPUs advertise PGE incorrectly and require
	 * a cr3 based tlb flush, so the following __flush_tlb_all()
	 * will not flush anything because the cpu quirk which clears
	 * X86_FEATURE_PGE has not been invoked yet. Though due to the
	 * load_cr3() above the TLB has been flushed already. The
	 * quirk is invoked before subsequent calls to __flush_tlb_all()
	 * so proper operation is guaranteed.
	 */
	__flush_tlb_all();
#else
	printk(KERN_INFO "Command line: %s\n", boot_command_line);
#endif

	/*
	 * If we have OLPC OFW, we might end up relocating the fixmap due to
	 * reserve_top(), so do this before touching the ioremap area.
	 */
	olpc_ofw_detect();

	early_trap_init();
	early_cpu_init();
	early_ioremap_init();

	setup_olpc_ofw_pgd();

	ROOT_DEV = old_decode_dev(boot_params.hdr.root_dev);
	screen_info = boot_params.screen_info;
	edid_info = boot_params.edid_info;
#ifdef CONFIG_X86_32
	apm_info.bios = boot_params.apm_bios_info;
	ist_info = boot_params.ist_info;
#endif
	saved_video_mode = boot_params.hdr.vid_mode;
	bootloader_type = boot_params.hdr.type_of_loader;
	if ((bootloader_type >> 4) == 0xe) {
		bootloader_type &= 0xf;
		bootloader_type |= (boot_params.hdr.ext_loader_type+0x10) << 4;
	}
	bootloader_version  = bootloader_type & 0xf;
	bootloader_version |= boot_params.hdr.ext_loader_ver << 4;

#ifdef CONFIG_BLK_DEV_RAM
	rd_image_start = boot_params.hdr.ram_size & RAMDISK_IMAGE_START_MASK;
	rd_prompt = ((boot_params.hdr.ram_size & RAMDISK_PROMPT_FLAG) != 0);
	rd_doload = ((boot_params.hdr.ram_size & RAMDISK_LOAD_FLAG) != 0);
#endif
#ifdef CONFIG_EFI
	if (!strncmp((char *)&boot_params.efi_info.efi_loader_signature,
		     EFI32_LOADER_SIGNATURE, 4)) {
		set_bit(EFI_BOOT, &efi.flags);
	} else if (!strncmp((char *)&boot_params.efi_info.efi_loader_signature,
		     EFI64_LOADER_SIGNATURE, 4)) {
		set_bit(EFI_BOOT, &efi.flags);
		set_bit(EFI_64BIT, &efi.flags);
	}

	if (efi_enabled(EFI_BOOT))
		efi_memblock_x86_reserve_range();
#endif

	x86_init.oem.arch_setup();

	iomem_resource.end = (1ULL << boot_cpu_data.x86_phys_bits) - 1;
	setup_memory_map();
	parse_setup_data();

	copy_edd();

	if (!boot_params.hdr.root_flags)
		root_mountflags &= ~MS_RDONLY;
	init_mm.start_code = (unsigned long) _text;
	init_mm.end_code = (unsigned long) _etext;
	init_mm.end_data = (unsigned long) _edata;
	init_mm.brk = _brk_end;

	mpx_mm_init(&init_mm);

	code_resource.start = __pa_symbol(_text);
	code_resource.end = __pa_symbol(_etext)-1;
	data_resource.start = __pa_symbol(_etext);
	data_resource.end = __pa_symbol(_edata)-1;
	bss_resource.start = __pa_symbol(__bss_start);
	bss_resource.end = __pa_symbol(__bss_stop)-1;

#ifdef CONFIG_CMDLINE_BOOL
#ifdef CONFIG_CMDLINE_OVERRIDE
	strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
#else
	if (builtin_cmdline[0]) {
		/* append boot loader cmdline to builtin */
		strlcat(builtin_cmdline, " ", COMMAND_LINE_SIZE);
		strlcat(builtin_cmdline, boot_command_line, COMMAND_LINE_SIZE);
		strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
	}
#endif
#endif

	strlcpy(command_line, boot_command_line, COMMAND_LINE_SIZE);
	*cmdline_p = command_line;

	/*
	 * x86_configure_nx() is called before parse_early_param() to detect
	 * whether hardware doesn't support NX (so that the early EHCI debug
	 * console setup can safely call set_fixmap()). It may then be called
	 * again from within noexec_setup() during parsing early parameters
	 * to honor the respective command line option.
	 */
	x86_configure_nx();

	parse_early_param();

	x86_report_nx();

	/* after early param, so could get panic from serial */
	memblock_x86_reserve_range_setup_data();

	if (acpi_mps_check()) {
#ifdef CONFIG_X86_LOCAL_APIC
		disable_apic = 1;
#endif
		setup_clear_cpu_cap(X86_FEATURE_APIC);
	}

#ifdef CONFIG_PCI
	if (pci_early_dump_regs)
		early_dump_pci_devices();
#endif

	/* update the e820_saved too */
	e820_reserve_setup_data();
	finish_e820_parsing();

	if (efi_enabled(EFI_BOOT))
		efi_init();

	dmi_scan_machine();
	dmi_memdev_walk();
	dmi_set_dump_stack_arch_desc();

	/*
	 * VMware detection requires dmi to be available, so this
	 * needs to be done after dmi_scan_machine, for the BP.
	 */
	init_hypervisor_platform();

	x86_init.resources.probe_roms();

	/* after parse_early_param, so could debug it */
	insert_resource(&iomem_resource, &code_resource);
	insert_resource(&iomem_resource, &data_resource);
	insert_resource(&iomem_resource, &bss_resource);

	e820_add_kernel_range();
	trim_bios_range();
#ifdef CONFIG_X86_32
	if (ppro_with_ram_bug()) {
		e820_update_range(0x70000000ULL, 0x40000ULL, E820_RAM,
				  E820_RESERVED);
		sanitize_e820_map(e820.map, ARRAY_SIZE(e820.map), &e820.nr_map);
		printk(KERN_INFO "fixed physical RAM map:\n");
		e820_print_map("bad_ppro");
	}
#else
	early_gart_iommu_check();
#endif

	/*
	 * partially used pages are not usable - thus
	 * we are rounding upwards:
	 */
	max_pfn = e820_end_of_ram_pfn();

	/* update e820 for memory not covered by WB MTRRs */
	mtrr_bp_init();
	if (mtrr_trim_uncached_memory(max_pfn))
		max_pfn = e820_end_of_ram_pfn();

	max_possible_pfn = max_pfn;

#ifdef CONFIG_X86_32
	/* max_low_pfn get updated here */
	find_low_pfn_range();
#else
	check_x2apic();

	/* How many end-of-memory variables you have, grandma! */
	/* need this before calling reserve_initrd */
	if (max_pfn > (1UL<<(32 - PAGE_SHIFT)))
		max_low_pfn = e820_end_of_low_ram_pfn();
	else
		max_low_pfn = max_pfn;

	high_memory = (void *)__va(max_pfn * PAGE_SIZE - 1) + 1;
#endif

	/*
	 * Find and reserve possible boot-time SMP configuration:
	 */
	find_smp_config();

	reserve_ibft_region();

	early_alloc_pgt_buf();

	/*
	 * Need to conclude brk, before memblock_x86_fill()
	 *  it could use memblock_find_in_range, could overlap with
	 *  brk area.
	 */
	reserve_brk();

	cleanup_highmap();

	memblock_set_current_limit(ISA_END_ADDRESS);
	memblock_x86_fill();

	if (efi_enabled(EFI_BOOT)) {
		efi_fake_memmap();
		efi_find_mirror();
	}

	/*
	 * The EFI specification says that boot service code won't be called
	 * after ExitBootServices(). This is, in fact, a lie.
	 */
	if (efi_enabled(EFI_MEMMAP))
		efi_reserve_boot_services();

	/* preallocate 4k for mptable mpc */
	early_reserve_e820_mpc_new();

#ifdef CONFIG_X86_CHECK_BIOS_CORRUPTION
	setup_bios_corruption_check();
#endif

#ifdef CONFIG_X86_32
	printk(KERN_DEBUG "initial memory mapped: [mem 0x00000000-%#010lx]\n",
			(max_pfn_mapped<<PAGE_SHIFT) - 1);
#endif

	reserve_real_mode();

	trim_platform_memory_ranges();
	trim_low_memory_range();

	init_mem_mapping();

	early_trap_pf_init();

	setup_real_mode();

	memblock_set_current_limit(get_max_mapped());

	/*
	 * NOTE: On x86-32, only from this point on, fixmaps are ready for use.
	 */

#ifdef CONFIG_PROVIDE_OHCI1394_DMA_INIT
	if (init_ohci1394_dma_early)
		init_ohci1394_dma_on_all_controllers();
#endif
 	/* Allocate bigger log buffer */
 	setup_log_buf(1);
 
#ifdef CONFIG_EFI_SECURE_BOOT_SECURELEVEL
	if (boot_params.secure_boot) {
		set_securelevel(1);
	}
#endif

 	reserve_initrd();
 
 #if defined(CONFIG_ACPI) && defined(CONFIG_BLK_DEV_INITRD)
	acpi_initrd_override((void *)initrd_start, initrd_end - initrd_start);
#endif

	vsmp_init();
 
 	io_delay_init();
 
 	/*
 	 * Parse the ACPI tables for possible boot-time SMP configuration.
 	 */
	acpi_boot_table_init();

	early_acpi_boot_init();

	initmem_init();
	dma_contiguous_reserve(max_pfn_mapped << PAGE_SHIFT);

	/*
	 * Reserve memory for crash kernel after SRAT is parsed so that it
	 * won't consume hotpluggable memory.
	 */
	reserve_crashkernel();

	memblock_find_dma_reserve();

#ifdef CONFIG_KVM_GUEST
	kvmclock_init();
#endif

	x86_init.paging.pagetable_init();

	kasan_init();

	if (boot_cpu_data.cpuid_level >= 0) {
		/* A CPU has %cr4 if and only if it has CPUID */
		mmu_cr4_features = __read_cr4();
		if (trampoline_cr4_features)
			*trampoline_cr4_features = mmu_cr4_features;
	}

#ifdef CONFIG_X86_32
	/* sync back kernel address range */
	clone_pgd_range(initial_page_table + KERNEL_PGD_BOUNDARY,
			swapper_pg_dir     + KERNEL_PGD_BOUNDARY,
			KERNEL_PGD_PTRS);

	/*
	 * sync back low identity map too.  It is used for example
	 * in the 32-bit EFI stub.
	 */
	clone_pgd_range(initial_page_table,
			swapper_pg_dir     + KERNEL_PGD_BOUNDARY,
			min(KERNEL_PGD_PTRS, KERNEL_PGD_BOUNDARY));
#endif

	tboot_probe();

	map_vsyscall();

	generic_apic_probe();

	early_quirks();

	/*
	 * Read APIC and some other early information from ACPI tables.
	 */
	acpi_boot_init();
	sfi_init();
	x86_dtb_init();

	/*
	 * get boot-time SMP configuration:
	 */
	if (smp_found_config)
		get_smp_config();

	prefill_possible_map();

	init_cpu_to_node();

	init_apic_mappings();
	io_apic_init_mappings();

	kvm_guest_init();

	e820_reserve_resources();
	e820_mark_nosave_regions(max_low_pfn);

	x86_init.resources.reserve_resources();

	e820_setup_gap();

#ifdef CONFIG_VT
#if defined(CONFIG_VGA_CONSOLE)
	if (!efi_enabled(EFI_BOOT) || (efi_mem_type(0xa0000) != EFI_CONVENTIONAL_MEMORY))
		conswitchp = &vga_con;
#elif defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif
#endif
	x86_init.oem.banner();

	x86_init.timers.wallclock_init();

	mcheck_init();

	arch_init_ideal_nops();

	register_refined_jiffies(CLOCK_TICK_RATE);

#ifdef CONFIG_EFI
	if (efi_enabled(EFI_BOOT))
		efi_apply_memmap_quirks();
#endif
}
