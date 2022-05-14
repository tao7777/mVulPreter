struct reloc_t* MACH0_(get_relocs)(struct MACH0_(obj_t)* bin) {
	struct reloc_t *relocs;
	int i = 0, len;
	ulebr ur = {NULL};
	int wordsize = MACH0_(get_bits)(bin) / 8;
	if (bin->dyld_info) {
		ut8 *opcodes,*end, type = 0, rel_type = 0;
		int lib_ord, seg_idx = -1, sym_ord = -1;
		size_t j, count, skip, bind_size, lazy_size;
		st64 addend = 0;
		ut64 segmentAddress = 0LL;
		ut64 addr = 0LL;
		ut8 done = 0;

#define CASE(T) case (T / 8): rel_type = R_BIN_RELOC_ ## T; break
		switch (wordsize) {
		CASE(8);
		CASE(16);
		CASE(32);
		CASE(64);
		default: return NULL;
		}
#undef CASE
		bind_size = bin->dyld_info->bind_size;
		lazy_size = bin->dyld_info->lazy_bind_size;

		if (!bind_size || !lazy_size) {
			return NULL;
		}

 		if ((bind_size + lazy_size)<1) {
 			return NULL;
 		}
		if (bin->dyld_info->bind_off > bin->size || bin->dyld_info->bind_off + bind_size > bin->size) {
 			return NULL;
		}
 		if (bin->dyld_info->lazy_bind_off > bin->size || \
			bin->dyld_info->lazy_bind_off + lazy_size > bin->size) {
 			return NULL;
		}
		if (bin->dyld_info->bind_off+bind_size+lazy_size > bin->size) {
 			return NULL;
		}
		if (!(relocs = calloc (1, (1 + bind_size + lazy_size) * sizeof (struct reloc_t)))) {
 			return NULL;
		}
 		opcodes = calloc (1, bind_size + lazy_size + 1);
 		if (!opcodes) {
 			free (relocs);
			return NULL;
		}
		len = r_buf_read_at (bin->b, bin->dyld_info->bind_off, opcodes, bind_size);
		i = r_buf_read_at (bin->b, bin->dyld_info->lazy_bind_off, opcodes + bind_size, lazy_size);
		if (len < 1 || i < 1) {
			bprintf ("Error: read (dyld_info bind) at 0x%08"PFMT64x"\n",
			(ut64)(size_t)bin->dyld_info->bind_off);
			free (opcodes);
			relocs[i].last = 1;
			return relocs;
		}
		i = 0;
		for (ur.p = opcodes, end = opcodes + bind_size + lazy_size ; (ur.p+2 < end) && !done; ) {
			ut8 imm = *ur.p & BIND_IMMEDIATE_MASK, op = *ur.p & BIND_OPCODE_MASK;
			++ur.p;
			switch (op) {
#define ULEB() read_uleb128 (&ur,end)
#define SLEB() read_sleb128 (&ur,end)
			case BIND_OPCODE_DONE:
				done = 1;
				break;
			case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
				lib_ord = imm;
				break;
			case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
				lib_ord = ULEB();
				break;
			case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
				lib_ord = imm? (st8)(BIND_OPCODE_MASK | imm) : 0;
				break;
			case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM: {
				char *sym_name = (char*)ur.p;
				while (*ur.p++ && ur.p<end) {
					/* empty loop */
				}
				sym_ord = -1;
				if (bin->symtab && bin->dysymtab.nundefsym < 0xffff)
				for (j = 0; j < bin->dysymtab.nundefsym; j++) {
					int stridx = 0;
					int iundefsym = bin->dysymtab.iundefsym;
					if (iundefsym>=0 && iundefsym < bin->nsymtab) {
						int sidx = iundefsym +j;
						if (sidx<0 || sidx>= bin->nsymtab)
							continue;
						stridx = bin->symtab[sidx].n_strx;
						if (stridx < 0 || stridx >= bin->symstrlen)
							continue;
					}
					if (!strcmp ((char *)bin->symstr + stridx, sym_name)) {
						sym_ord = j;
						break;
					}
				}
				break;
			}
			case BIND_OPCODE_SET_TYPE_IMM:
				type = imm;
				break;
			case BIND_OPCODE_SET_ADDEND_SLEB:
				addend = SLEB();
				break;
			case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
				seg_idx = imm;
				if (seg_idx < 0 || seg_idx >= bin->nsegs) {
					bprintf ("Error: BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB"
						" has unexistent segment %d\n", seg_idx);
					addr = 0LL;
					return 0; // early exit to avoid future mayhem
				} else {
					addr = bin->segs[seg_idx].vmaddr + ULEB();
					segmentAddress = bin->segs[seg_idx].vmaddr \
							+ bin->segs[seg_idx].vmsize;
				}
				break;
			case BIND_OPCODE_ADD_ADDR_ULEB:
				addr += ULEB();
				break;
#define DO_BIND() do {\
if (sym_ord < 0 || seg_idx < 0 ) break;\
if (i >= (bind_size + lazy_size)) break;\
relocs[i].addr = addr;\
relocs[i].offset = addr - bin->segs[seg_idx].vmaddr + bin->segs[seg_idx].fileoff;\
if (type == BIND_TYPE_TEXT_PCREL32)\
	relocs[i].addend = addend - (bin->baddr + addr);\
else relocs[i].addend = addend;\
/* library ordinal ??? */ \
relocs[i].ord = lib_ord;\
relocs[i].ord = sym_ord;\
relocs[i].type = rel_type;\
relocs[i++].last = 0;\
} while (0)
			case BIND_OPCODE_DO_BIND:
				if (addr >= segmentAddress) {
					bprintf ("Error: Malformed DO bind opcode\n");
					goto beach;
				}
				DO_BIND();
				addr += wordsize;
				break;
			case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
				if (addr >= segmentAddress) {
					bprintf ("Error: Malformed ADDR ULEB bind opcode\n");
					goto beach;
				}
				DO_BIND();
				addr += ULEB() + wordsize;
				break;
			case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
				if (addr >= segmentAddress) {
					bprintf ("Error: Malformed IMM SCALED bind opcode\n");
					goto beach;
				}
				DO_BIND();
				addr += (ut64)imm * (ut64)wordsize + wordsize;
				break;
			case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
				count = ULEB();
				skip = ULEB();
				for (j = 0; j < count; j++) {
					if (addr >= segmentAddress) {
						bprintf ("Error: Malformed ULEB TIMES bind opcode\n");
						goto beach;
					}
					DO_BIND();
					addr += skip + wordsize;
				}
				break;
#undef DO_BIND
#undef ULEB
#undef SLEB
			default:
				bprintf ("Error: unknown bind opcode 0x%02x in dyld_info\n", *ur.p);
				free (opcodes);
				relocs[i].last = 1;
				return relocs;
			}
		}
 		free (opcodes);
 	} else {
 		int j;
		if (!bin->symtab || !bin->symstr || !bin->sects || !bin->indirectsyms) {
 			return NULL;
		}
		if (!(relocs = malloc ((bin->dysymtab.nundefsym + 1) * sizeof(struct reloc_t)))) {
 			return NULL;
		}
 		for (j = 0; j < bin->dysymtab.nundefsym; j++) {
			if (parse_import_ptr (bin, &relocs[i], bin->dysymtab.iundefsym + j)) {
 				relocs[i].ord = j;
 				relocs[i++].last = 0;
 			}
		}
	}
beach:
	relocs[i].last = 1;

	return relocs;
}
