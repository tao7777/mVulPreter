static Sdb *store_versioninfo_gnu_verdef(ELFOBJ *bin, Elf_(Shdr) *shdr, int sz) {
	const char *section_name = "";
	const char *link_section_name = "";
	char *end = NULL;
	Elf_(Shdr) *link_shdr = NULL;
	ut8 dfs[sizeof (Elf_(Verdef))] = {0};
	Sdb *sdb;
	int cnt, i;
	if (shdr->sh_link > bin->ehdr.e_shnum) {
		return false;
	}
	link_shdr = &bin->shdr[shdr->sh_link];
	if ((int)shdr->sh_size < 1) {
		return false;
	}
	Elf_(Verdef) *defs = calloc (shdr->sh_size, sizeof (char));
	if (!defs) {
		return false;
	}
	if (bin->shstrtab && shdr->sh_name < bin->shstrtab_size) {
		section_name = &bin->shstrtab[shdr->sh_name];
	}
	if (link_shdr && bin->shstrtab && link_shdr->sh_name < bin->shstrtab_size) {
		link_section_name = &bin->shstrtab[link_shdr->sh_name];
	}
	if (!defs) {
		bprintf ("Warning: Cannot allocate memory (Check Elf_(Verdef))\n");
		return NULL;
	}
	sdb = sdb_new0 ();
	end = (char *)defs + shdr->sh_size;
	sdb_set (sdb, "section_name", section_name, 0);
	sdb_num_set (sdb, "entries", shdr->sh_info, 0);
	sdb_num_set (sdb, "addr", shdr->sh_addr, 0);
	sdb_num_set (sdb, "offset", shdr->sh_offset, 0);
	sdb_num_set (sdb, "link", shdr->sh_link, 0);
	sdb_set (sdb, "link_section_name", link_section_name, 0);

 	for (cnt = 0, i = 0; i >= 0 && cnt < shdr->sh_info && ((char *)defs + i < end); ++cnt) {
 		Sdb *sdb_verdef = sdb_new0 ();
 		char *vstart = ((char*)defs) + i;
		size_t vstart_off = i;
 		char key[32] = {0};
 		Elf_(Verdef) *verdef = (Elf_(Verdef)*)vstart;
 		Elf_(Verdaux) aux = {0};
		int j = 0;
		int isum = 0;

		r_buf_read_at (bin->b, shdr->sh_offset + i, dfs, sizeof (Elf_(Verdef)));
		verdef->vd_version = READ16 (dfs, j)
		verdef->vd_flags = READ16 (dfs, j)
		verdef->vd_ndx = READ16 (dfs, j)
		verdef->vd_cnt = READ16 (dfs, j)
		verdef->vd_hash = READ32 (dfs, j)
 		verdef->vd_aux = READ32 (dfs, j)
 		verdef->vd_next = READ32 (dfs, j)
 		int vdaux = verdef->vd_aux;
		if (vdaux < 1 || shdr->sh_size - vstart_off < vdaux) {
 			sdb_free (sdb_verdef);
 			goto out_error;
 		}
 		vstart += vdaux;
		vstart_off += vdaux;
 		if (vstart > end || vstart + sizeof (Elf_(Verdaux)) > end) {
 			sdb_free (sdb_verdef);
 			goto out_error;
		}

		j = 0;
		aux.vda_name = READ32 (vstart, j)
		aux.vda_next = READ32 (vstart, j)

		isum = i + verdef->vd_aux;
		if (aux.vda_name > bin->dynstr_size) {
			sdb_free (sdb_verdef);
			goto out_error;
		}

		sdb_num_set (sdb_verdef, "idx", i, 0);
		sdb_num_set (sdb_verdef, "vd_version", verdef->vd_version, 0);
		sdb_num_set (sdb_verdef, "vd_ndx", verdef->vd_ndx, 0);
		sdb_num_set (sdb_verdef, "vd_cnt", verdef->vd_cnt, 0);
		sdb_set (sdb_verdef, "vda_name", &bin->dynstr[aux.vda_name], 0);
		sdb_set (sdb_verdef, "flags", get_ver_flags (verdef->vd_flags), 0);

		for (j = 1; j < verdef->vd_cnt; ++j) {
			int k;
 			Sdb *sdb_parent = sdb_new0 ();
 			isum += aux.vda_next;
 			vstart += aux.vda_next;
			vstart_off += aux.vda_next;
 			if (vstart > end || vstart + sizeof (Elf_(Verdaux)) > end) {
 				sdb_free (sdb_verdef);
 				sdb_free (sdb_parent);
				goto out_error;
			}
			k = 0;
			aux.vda_name = READ32 (vstart, k)
			aux.vda_next = READ32 (vstart, k)
			if (aux.vda_name > bin->dynstr_size) {
				sdb_free (sdb_verdef);
				sdb_free (sdb_parent);
				goto out_error;
			}
			sdb_num_set (sdb_parent, "idx", isum, 0);
			sdb_num_set (sdb_parent, "parent", j, 0);
			sdb_set (sdb_parent, "vda_name", &bin->dynstr[aux.vda_name], 0);
			snprintf (key, sizeof (key), "parent%d", j - 1);
			sdb_ns_set (sdb_verdef, key, sdb_parent);
		}

		snprintf (key, sizeof (key), "verdef%d", cnt);
		sdb_ns_set (sdb, key, sdb_verdef);
		if (!verdef->vd_next) {
			sdb_free (sdb_verdef);
			goto out_error;
		}
		if ((st32)verdef->vd_next < 1) {
			eprintf ("Warning: Invalid vd_next in the ELF version\n");
			break;
		}
		i += verdef->vd_next;
	}
	free (defs);
	return sdb;
out_error:
	free (defs);
	sdb_free (sdb);
	return NULL;
}
