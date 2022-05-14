static void dex_parse_debug_item(RBinFile *binfile, RBinDexObj *bin,
				  RBinDexClass *c, int MI, int MA, int paddr, int ins_size,
				  int insns_size, char *class_name, int regsz,
				  int debug_info_off) {
	struct r_bin_t *rbin = binfile->rbin;
	const ut8 *p4 = r_buf_get_at (binfile->buf, debug_info_off, NULL);
	const ut8 *p4_end = p4 + binfile->buf->length - debug_info_off;
	ut64 line_start;
	ut64 parameters_size;
	ut64 param_type_idx;
	ut16 argReg = regsz - ins_size;
	ut64 source_file_idx = c->source_file;
	RList *params, *debug_positions, *emitted_debug_locals = NULL; 
	bool keep = true;
	if (argReg > regsz) {
		return; // this return breaks tests
	}
	p4 = r_uleb128 (p4, p4_end - p4, &line_start);
	p4 = r_uleb128 (p4, p4_end - p4, &parameters_size);
	ut32 address = 0;
	ut32 line = line_start;
	if (!(debug_positions = r_list_newf ((RListFree)free))) {
		return;	
	}
	if (!(emitted_debug_locals = r_list_newf ((RListFree)free))) {
		r_list_free (debug_positions);
		return;
	}

	struct dex_debug_local_t debug_locals[regsz];
	memset (debug_locals, 0, sizeof (struct dex_debug_local_t) * regsz);
	if (!(MA & 0x0008)) {
		debug_locals[argReg].name = "this";
		debug_locals[argReg].descriptor = r_str_newf("%s;", class_name);
		debug_locals[argReg].startAddress = 0;
		debug_locals[argReg].signature = NULL;
		debug_locals[argReg].live = true;
		argReg++;
	}
	if (!(params = dex_method_signature2 (bin, MI))) {
		r_list_free (debug_positions);
		r_list_free (emitted_debug_locals);
		return;
	}

	RListIter *iter = r_list_iterator (params);
	char *name;
	char *type;
	int reg;

	r_list_foreach (params, iter, type) {
		if ((argReg >= regsz) || !type || parameters_size <= 0) {
			r_list_free (debug_positions);
			r_list_free (params);
			r_list_free (emitted_debug_locals);
			return;
		}
		p4 = r_uleb128 (p4, p4_end - p4, &param_type_idx); // read uleb128p1
		param_type_idx -= 1;
		name = getstr (bin, param_type_idx);
		reg = argReg;
		switch (type[0]) {
		case 'D':
		case 'J':
			argReg += 2;
			break;
		default:
			argReg += 1;
			break;
		}
		if (name) {
			debug_locals[reg].name = name;
			debug_locals[reg].descriptor = type;
			debug_locals[reg].signature = NULL;
			debug_locals[reg].startAddress = address;
			debug_locals[reg].live = true;
		}
 		--parameters_size;
 	}
 
	if (p4 <= 0) {
		return;
	}
 	ut8 opcode = *(p4++) & 0xff;
 	while (keep) {
 		switch (opcode) {
		case 0x0: // DBG_END_SEQUENCE
			keep = false;
			break;
		case 0x1: // DBG_ADVANCE_PC
			{
			ut64 addr_diff;
			p4 = r_uleb128 (p4, p4_end - p4, &addr_diff);
			address += addr_diff;
			}
			break;
		case 0x2: // DBG_ADVANCE_LINE
			{
			st64 line_diff = r_sleb128 (&p4, p4_end);
			line += line_diff;
			}
			break;	
		case 0x3: // DBG_START_LOCAL
			{
			ut64 register_num;
			ut64 name_idx;
			ut64 type_idx;
			p4 = r_uleb128 (p4, p4_end - p4, &register_num);
			p4 = r_uleb128 (p4, p4_end - p4, &name_idx); 
			name_idx -= 1;
			p4 = r_uleb128 (p4, p4_end - p4, &type_idx); 
			type_idx -= 1;
			if (register_num >= regsz) {
				r_list_free (debug_positions);
				r_list_free (params);
				return;
			}
			if (debug_locals[register_num].live) {
				struct dex_debug_local_t *local = malloc (
					sizeof (struct dex_debug_local_t));
				if (!local) {
					keep = false;
					break;
				}
				local->name = debug_locals[register_num].name;
				local->descriptor = debug_locals[register_num].descriptor;
				local->startAddress = debug_locals[register_num].startAddress;
				local->signature = debug_locals[register_num].signature;
				local->live = true;
				local->reg = register_num;
				local->endAddress = address;
				r_list_append (emitted_debug_locals, local);
			}
			debug_locals[register_num].name = getstr (bin, name_idx);
			debug_locals[register_num].descriptor = dex_type_descriptor (bin, type_idx);
			debug_locals[register_num].startAddress = address;
			debug_locals[register_num].signature = NULL;
			debug_locals[register_num].live = true;
			}
			break;
		case 0x4: //DBG_START_LOCAL_EXTENDED
			{
			ut64 register_num;
			ut64 name_idx;
			ut64 type_idx;
			ut64 sig_idx;
			p4 = r_uleb128 (p4, p4_end - p4, &register_num);
			p4 = r_uleb128 (p4, p4_end - p4, &name_idx);
			name_idx -= 1;
			p4 = r_uleb128 (p4, p4_end - p4, &type_idx);
			type_idx -= 1;
			p4 = r_uleb128 (p4, p4_end - p4, &sig_idx);
			sig_idx -= 1;
			if (register_num >= regsz) {
				r_list_free (debug_positions);
				r_list_free (params);
				return;
			}

			if (debug_locals[register_num].live) {
				struct dex_debug_local_t *local = malloc (
					sizeof (struct dex_debug_local_t));
				if (!local) {
					keep = false;
					break;
				}
				local->name = debug_locals[register_num].name;
				local->descriptor = debug_locals[register_num].descriptor;
				local->startAddress = debug_locals[register_num].startAddress;
				local->signature = debug_locals[register_num].signature;
				local->live = true;
				local->reg = register_num;
				local->endAddress = address;
				r_list_append (emitted_debug_locals, local);
			}

			debug_locals[register_num].name = getstr (bin, name_idx);
			debug_locals[register_num].descriptor = dex_type_descriptor (bin, type_idx);
			debug_locals[register_num].startAddress = address;
			debug_locals[register_num].signature = getstr (bin, sig_idx);
			debug_locals[register_num].live = true;
			}
			break;
		case 0x5: // DBG_END_LOCAL
			{
			ut64 register_num;
			p4 = r_uleb128 (p4, p4_end - p4, &register_num);
			if (debug_locals[register_num].live) {
				struct dex_debug_local_t *local = malloc (
					sizeof (struct dex_debug_local_t));
				if (!local) {
					keep = false;
					break;
				}
				local->name = debug_locals[register_num].name;
				local->descriptor = debug_locals[register_num].descriptor;
				local->startAddress = debug_locals[register_num].startAddress;
				local->signature = debug_locals[register_num].signature;
				local->live = true;
				local->reg = register_num;
				local->endAddress = address;
				r_list_append (emitted_debug_locals, local);
			}
			debug_locals[register_num].live = false;
			}
			break;
		case 0x6: // DBG_RESTART_LOCAL
			{
			ut64 register_num;
			p4 = r_uleb128 (p4, p4_end - p4, &register_num);
			if (!debug_locals[register_num].live) {
				debug_locals[register_num].startAddress = address;
				debug_locals[register_num].live = true;
			}
			}
			break;
		case 0x7: //DBG_SET_PROLOGUE_END
			break;
		case 0x8: //DBG_SET_PROLOGUE_BEGIN
			break;
		case 0x9:
			{
			p4 = r_uleb128 (p4, p4_end - p4, &source_file_idx);
			source_file_idx--;
			}
			break;
		default:
			{
			int adjusted_opcode = opcode - 0x0a;
			address += (adjusted_opcode / 15);
			line += -4 + (adjusted_opcode % 15);
			struct dex_debug_position_t *position =
				malloc (sizeof (struct dex_debug_position_t));
			if (!position) {
				keep = false;
				break;
			}
			position->source_file_idx = source_file_idx;
			position->address = address;
			position->line = line;
			r_list_append (debug_positions, position);
			}
			break;
		}
		opcode = *(p4++) & 0xff;
	}

	if (!binfile->sdb_addrinfo) {
		binfile->sdb_addrinfo = sdb_new0 ();
	}

	char *fileline;
	char offset[64];
	char *offset_ptr;

	RListIter *iter1;
	struct dex_debug_position_t *pos;
	r_list_foreach (debug_positions, iter1, pos) {
		fileline = r_str_newf ("%s|%"PFMT64d, getstr (bin, pos->source_file_idx), pos->line);
		offset_ptr = sdb_itoa (pos->address + paddr, offset, 16);
		sdb_set (binfile->sdb_addrinfo, offset_ptr, fileline, 0);
		sdb_set (binfile->sdb_addrinfo, fileline, offset_ptr, 0);
	}

	if (!dexdump) {
		r_list_free (debug_positions);
		r_list_free (emitted_debug_locals);
		r_list_free (params);
		return;
	}

	RListIter *iter2;
	struct dex_debug_position_t *position;

	rbin->cb_printf ("      positions     :\n");
	r_list_foreach (debug_positions, iter2, position) {
		rbin->cb_printf ("        0x%04llx line=%llu\n",
				 position->address, position->line);
	}

	rbin->cb_printf ("      locals        :\n");

	RListIter *iter3;
	struct dex_debug_local_t *local;
	r_list_foreach (emitted_debug_locals, iter3, local) {
		if (local->signature) {
			rbin->cb_printf (
				"        0x%04x - 0x%04x reg=%d %s %s %s\n",
				local->startAddress, local->endAddress,
				local->reg, local->name, local->descriptor,
				local->signature);
		} else {
			rbin->cb_printf (
				"        0x%04x - 0x%04x reg=%d %s %s\n",
				local->startAddress, local->endAddress,
				local->reg, local->name, local->descriptor);
		}
	}

	for (reg = 0; reg < regsz; reg++) {
		if (debug_locals[reg].live) {
			if (debug_locals[reg].signature) {
				rbin->cb_printf (
					"        0x%04x - 0x%04x reg=%d %s %s "
					"%s\n",
					debug_locals[reg].startAddress,
					insns_size, reg, debug_locals[reg].name,
					debug_locals[reg].descriptor,
					debug_locals[reg].signature);
			} else {
				rbin->cb_printf (
					"        0x%04x - 0x%04x reg=%d %s %s"
					"\n",
					debug_locals[reg].startAddress,
					insns_size, reg, debug_locals[reg].name,
					debug_locals[reg].descriptor);
			}
		}
	}
	r_list_free (debug_positions);
	r_list_free (emitted_debug_locals);
	r_list_free (params);
}
