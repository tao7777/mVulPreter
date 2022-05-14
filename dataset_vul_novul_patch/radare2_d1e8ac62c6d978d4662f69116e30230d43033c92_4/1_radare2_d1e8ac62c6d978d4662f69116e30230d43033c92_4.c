static int init_items(struct MACH0_(obj_t)* bin) {
	struct load_command lc = {0, 0};
	ut8 loadc[sizeof (struct load_command)] = {0};
	bool is_first_thread = true;
	ut64 off = 0LL;
	int i, len;

	bin->uuidn = 0;
	bin->os = 0;
	bin->has_crypto = 0;
	if (bin->hdr.sizeofcmds > bin->size) {
		bprintf ("Warning: chopping hdr.sizeofcmds\n");
		bin->hdr.sizeofcmds = bin->size - 128;
	}
	for (i = 0, off = sizeof (struct MACH0_(mach_header)); \
			i < bin->hdr.ncmds; i++, off += lc.cmdsize) {
		if (off > bin->size || off + sizeof (struct load_command) > bin->size){
			bprintf ("mach0: out of bounds command\n");
			return false;
		}
		len = r_buf_read_at (bin->b, off, loadc, sizeof (struct load_command));
		if (len < 1) {
			bprintf ("Error: read (lc) at 0x%08"PFMT64x"\n", off);
			return false;
		}
		lc.cmd = r_read_ble32 (&loadc[0], bin->big_endian);
		lc.cmdsize = r_read_ble32 (&loadc[4], bin->big_endian);

		if (lc.cmdsize < 1 || off + lc.cmdsize > bin->size) {
			bprintf ("Warning: mach0_header %d = cmdsize<1.\n", i);
			break;
		}

		sdb_num_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.offset", i), off, 0);
		sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.format", i), "xd cmd size", 0);

		switch (lc.cmd) {
		case LC_DATA_IN_CODE:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "data_in_code", 0);
			break;
		case LC_RPATH:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "rpath", 0);
			break;
		case LC_SEGMENT_64:
		case LC_SEGMENT:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "segment", 0);
			bin->nsegs++;
			if (!parse_segments (bin, off)) {
				bprintf ("error parsing segment\n");
				bin->nsegs--;
				return false;
			}
			break;
		case LC_SYMTAB:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "symtab", 0);
			if (!parse_symtab (bin, off)) {
				bprintf ("error parsing symtab\n");
				return false;
			}
			break;
		case LC_DYSYMTAB:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "dysymtab", 0);
			if (!parse_dysymtab(bin, off)) {
				bprintf ("error parsing dysymtab\n");
				return false;
			}
			break;
		case LC_DYLIB_CODE_SIGN_DRS:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "dylib_code_sign_drs", 0);
			break;
		case LC_VERSION_MIN_MACOSX:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "version_min_macosx", 0);
			bin->os = 1;
			break;
		case LC_VERSION_MIN_IPHONEOS:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "version_min_iphoneos", 0);
			bin->os = 2;
			break;
		case LC_VERSION_MIN_TVOS:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "version_min_tvos", 0);
			bin->os = 4;
			break;
		case LC_VERSION_MIN_WATCHOS:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "version_min_watchos", 0);
			bin->os = 3;
			break;
		case LC_UUID:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "uuid", 0);
			{
			struct uuid_command uc = {0};
			if (off + sizeof (struct uuid_command) > bin->size) {
				bprintf ("UUID out of obunds\n");
				return false;
			}
			if (r_buf_fread_at (bin->b, off, (ut8*)&uc, "24c", 1) != -1) {
				char key[128];
				char val[128];
				snprintf (key, sizeof (key)-1, "uuid.%d", bin->uuidn++);
				r_hex_bin2str ((ut8*)&uc.uuid, 16, val);
				sdb_set (bin->kv, key, val, 0);
			}
			}
			break;
		case LC_ENCRYPTION_INFO_64:
			/* TODO: the struct is probably different here */
		case LC_ENCRYPTION_INFO:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "encryption_info", 0);
			{
			struct MACH0_(encryption_info_command) eic = {0};
			ut8 seic[sizeof (struct MACH0_(encryption_info_command))] = {0};
			if (off + sizeof (struct MACH0_(encryption_info_command)) > bin->size) {
				bprintf ("encryption info out of bounds\n");
				return false;
			}
			if (r_buf_read_at (bin->b, off, seic, sizeof (struct MACH0_(encryption_info_command))) != -1) {
				eic.cmd = r_read_ble32 (&seic[0], bin->big_endian);
				eic.cmdsize = r_read_ble32 (&seic[4], bin->big_endian);
				eic.cryptoff = r_read_ble32 (&seic[8], bin->big_endian);
				eic.cryptsize = r_read_ble32 (&seic[12], bin->big_endian);
				eic.cryptid = r_read_ble32 (&seic[16], bin->big_endian);

				bin->has_crypto = eic.cryptid;
				sdb_set (bin->kv, "crypto", "true", 0);
				sdb_num_set (bin->kv, "cryptid", eic.cryptid, 0);
				sdb_num_set (bin->kv, "cryptoff", eic.cryptoff, 0);
				sdb_num_set (bin->kv, "cryptsize", eic.cryptsize, 0);
				sdb_num_set (bin->kv, "cryptheader", off, 0);
			} }
			break;
		case LC_LOAD_DYLINKER:
			{
				sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "dylinker", 0);
				free (bin->intrp);
				bin->intrp = NULL;
				struct dylinker_command dy = {0};
				ut8 sdy[sizeof (struct dylinker_command)] = {0};
				if (off + sizeof (struct dylinker_command) > bin->size){
					bprintf ("Warning: Cannot parse dylinker command\n");
					return false;
				}
				if (r_buf_read_at (bin->b, off, sdy, sizeof (struct dylinker_command)) == -1) {
					bprintf ("Warning: read (LC_DYLD_INFO) at 0x%08"PFMT64x"\n", off);
				} else {
					dy.cmd = r_read_ble32 (&sdy[0], bin->big_endian);
					dy.cmdsize = r_read_ble32 (&sdy[4], bin->big_endian);
					dy.name = r_read_ble32 (&sdy[8], bin->big_endian);

					int len = dy.cmdsize;
					char *buf = malloc (len+1);
					if (buf) {
						r_buf_read_at (bin->b, off + 0xc, (ut8*)buf, len);
						buf[len] = 0;
						free (bin->intrp);
						bin->intrp = buf;
					}
				}
			}
			break;
		case LC_MAIN:
			{
			struct {
				ut64 eo;
				ut64 ss;
			} ep = {0};
			ut8 sep[2 * sizeof (ut64)] = {0};
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "main", 0);

			if (!is_first_thread) {
				bprintf("Error: LC_MAIN with other threads\n");
				return false;
			}
			if (off + 8 > bin->size || off + sizeof (ep) > bin->size) {
				bprintf ("invalid command size for main\n");
				return false;
			}
			r_buf_read_at (bin->b, off + 8, sep, 2 * sizeof (ut64));
			ep.eo = r_read_ble64 (&sep[0], bin->big_endian);
			ep.ss = r_read_ble64 (&sep[8], bin->big_endian);

			bin->entry = ep.eo;
			bin->main_cmd = lc;

			sdb_num_set (bin->kv, "mach0.entry.offset", off + 8, 0);
			sdb_num_set (bin->kv, "stacksize", ep.ss, 0);

			is_first_thread = false;
			}
			break;
		case LC_UNIXTHREAD:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "unixthread", 0);
			if (!is_first_thread) {
				bprintf("Error: LC_UNIXTHREAD with other threads\n");
				return false;
			}
		case LC_THREAD:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "thread", 0);
			if (!parse_thread (bin, &lc, off, is_first_thread)) {
				bprintf ("Cannot parse thread\n");
				return false;
			}
			is_first_thread = false;
			break;
		case LC_LOAD_DYLIB:
 		case LC_LOAD_WEAK_DYLIB:
 			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "load_dylib", 0);
 			bin->nlibs++;
			if (!parse_dylib(bin, off)){
 				bprintf ("Cannot parse dylib\n");
 				bin->nlibs--;
 				return false;
			}
			break;
		case LC_DYLD_INFO:
		case LC_DYLD_INFO_ONLY:
 			{
 			ut8 dyldi[sizeof (struct dyld_info_command)] = {0};
 			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "dyld_info", 0);
			bin->dyld_info = malloc (sizeof(struct dyld_info_command));
			if (off + sizeof (struct dyld_info_command) > bin->size){
				bprintf ("Cannot parse dyldinfo\n");
				free (bin->dyld_info);
				return false;
			}
			if (r_buf_read_at (bin->b, off, dyldi, sizeof (struct dyld_info_command)) == -1) {
				free (bin->dyld_info);
				bin->dyld_info = NULL;
				bprintf ("Error: read (LC_DYLD_INFO) at 0x%08"PFMT64x"\n", off);
			} else {
				bin->dyld_info->cmd = r_read_ble32 (&dyldi[0], bin->big_endian);
				bin->dyld_info->cmdsize = r_read_ble32 (&dyldi[4], bin->big_endian);
				bin->dyld_info->rebase_off = r_read_ble32 (&dyldi[8], bin->big_endian);
				bin->dyld_info->rebase_size = r_read_ble32 (&dyldi[12], bin->big_endian);
				bin->dyld_info->bind_off = r_read_ble32 (&dyldi[16], bin->big_endian);
				bin->dyld_info->bind_size = r_read_ble32 (&dyldi[20], bin->big_endian);
				bin->dyld_info->weak_bind_off = r_read_ble32 (&dyldi[24], bin->big_endian);
				bin->dyld_info->weak_bind_size = r_read_ble32 (&dyldi[28], bin->big_endian);
				bin->dyld_info->lazy_bind_off = r_read_ble32 (&dyldi[32], bin->big_endian);
				bin->dyld_info->lazy_bind_size = r_read_ble32 (&dyldi[36], bin->big_endian);
				bin->dyld_info->export_off = r_read_ble32 (&dyldi[40], bin->big_endian);
				bin->dyld_info->export_size = r_read_ble32 (&dyldi[44], bin->big_endian);
 			}
 			}
 			break;
		case LC_CODE_SIGNATURE:
			parse_signature (bin, off);
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "signature", 0);
			/* ut32 dataoff
			break;
		case LC_SOURCE_VERSION:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "version", 0);
			/* uint64_t  version;  */
			/* A.B.C.D.E packed as a24.b10.c10.d10.e10 */
			break;
		case LC_SEGMENT_SPLIT_INFO:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "split_info", 0);
			/* TODO */
			break;
		case LC_FUNCTION_STARTS:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "function_starts", 0);
			if (!parse_function_starts (bin, off)) {
				bprintf ("Cannot parse LC_FUNCTION_STARTS\n");
			}
			break;
		case LC_REEXPORT_DYLIB:
			sdb_set (bin->kv, sdb_fmt (0, "mach0_cmd_%d.cmd", i), "dylib", 0);
			/* TODO */
			break;
		default:
			break;
		}
	}
	return true;
}
