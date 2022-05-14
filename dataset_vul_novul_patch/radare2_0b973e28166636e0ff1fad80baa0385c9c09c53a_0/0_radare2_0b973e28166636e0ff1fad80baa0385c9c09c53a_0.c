static Sdb *store_versioninfo_gnu_verneed(ELFOBJ *bin, Elf_(Shdr) *shdr, int sz) {
	ut8 *end, *need = NULL;
	const char *section_name = "";
	Elf_(Shdr) *link_shdr = NULL;
	const char *link_section_name = "";
	Sdb *sdb_vernaux = NULL;
	Sdb *sdb_version = NULL;
	Sdb *sdb = NULL;
	int i, cnt;

	if (!bin || !bin->dynstr) {
		return NULL;
	}
 	if (shdr->sh_link > bin->ehdr.e_shnum) {
 		return NULL;
 	}
	if (shdr->sh_size < 1 || shdr->sh_size > SIZE_MAX) {
 		return NULL;
 	}
 	sdb = sdb_new0 ();
	if (!sdb) {
		return NULL;
	}
	link_shdr = &bin->shdr[shdr->sh_link];
	if (bin->shstrtab && shdr->sh_name < bin->shstrtab_size) {
		section_name = &bin->shstrtab[shdr->sh_name];
	}
	if (bin->shstrtab && link_shdr->sh_name < bin->shstrtab_size) {
		link_section_name = &bin->shstrtab[link_shdr->sh_name];
	}
	if (!(need = (ut8*) calloc (R_MAX (1, shdr->sh_size), sizeof (ut8)))) {
		bprintf ("Warning: Cannot allocate memory for Elf_(Verneed)\n");
		goto beach;
	}
	end = need + shdr->sh_size;
	sdb_set (sdb, "section_name", section_name, 0);
	sdb_num_set (sdb, "num_entries", shdr->sh_info, 0);
	sdb_num_set (sdb, "addr", shdr->sh_addr, 0);
	sdb_num_set (sdb, "offset", shdr->sh_offset, 0);
	sdb_num_set (sdb, "link", shdr->sh_link, 0);
	sdb_set (sdb, "link_section_name", link_section_name, 0);

	if (shdr->sh_offset > bin->size || shdr->sh_offset + shdr->sh_size > bin->size) {
		goto beach;
	}
	if (shdr->sh_offset + shdr->sh_size < shdr->sh_size) {
		goto beach;
	}
	i = r_buf_read_at (bin->b, shdr->sh_offset, need, shdr->sh_size);
	if (i < 0)
		goto beach;
	for (i = 0, cnt = 0; cnt < shdr->sh_info; ++cnt) {
		int j, isum;
		ut8 *vstart = need + i;
		Elf_(Verneed) vvn = {0};
		if (vstart + sizeof (Elf_(Verneed)) > end) {
			goto beach;
		}
		Elf_(Verneed) *entry = &vvn;
		char key[32] = {0};
		sdb_version = sdb_new0 ();
		if (!sdb_version) {
			goto beach;
		}
		j = 0;
		vvn.vn_version = READ16 (vstart, j)
		vvn.vn_cnt = READ16 (vstart, j)
		vvn.vn_file = READ32 (vstart, j)
		vvn.vn_aux = READ32 (vstart, j)
		vvn.vn_next = READ32 (vstart, j)

		sdb_num_set (sdb_version, "vn_version", entry->vn_version, 0);
		sdb_num_set (sdb_version, "idx", i, 0);
		if (entry->vn_file > bin->dynstr_size) {
			goto beach;
		}
		{
			char *s = r_str_ndup (&bin->dynstr[entry->vn_file], 16);
			sdb_set (sdb_version, "file_name", s, 0);
			free (s);
		}
		sdb_num_set (sdb_version, "cnt", entry->vn_cnt, 0);
		st32 vnaux = entry->vn_aux;
		if (vnaux < 1) {
			goto beach;
		}
		vstart += vnaux;
		for (j = 0, isum = i + entry->vn_aux; j < entry->vn_cnt && vstart + sizeof (Elf_(Vernaux)) <= end; ++j) {
			int k;
			Elf_(Vernaux) * aux = NULL;
			Elf_(Vernaux) vaux = {0};
			sdb_vernaux = sdb_new0 ();
			if (!sdb_vernaux) {
				goto beach;
			}
			aux = (Elf_(Vernaux)*)&vaux;
			k = 0;
			vaux.vna_hash = READ32 (vstart, k)
			vaux.vna_flags = READ16 (vstart, k)
			vaux.vna_other = READ16 (vstart, k)
			vaux.vna_name = READ32 (vstart, k)
			vaux.vna_next = READ32 (vstart, k)
			if (aux->vna_name > bin->dynstr_size) {
				goto beach;
			}
			sdb_num_set (sdb_vernaux, "idx", isum, 0);
			if (aux->vna_name > 0 && aux->vna_name + 8 < bin->dynstr_size) {
				char name [16];
				strncpy (name, &bin->dynstr[aux->vna_name], sizeof (name)-1);
				name[sizeof(name)-1] = 0;
				sdb_set (sdb_vernaux, "name", name, 0);
			}
			sdb_set (sdb_vernaux, "flags", get_ver_flags (aux->vna_flags), 0);
			sdb_num_set (sdb_vernaux, "version", aux->vna_other, 0);
			isum += aux->vna_next;
			vstart += aux->vna_next;
			snprintf (key, sizeof (key), "vernaux%d", j);
			sdb_ns_set (sdb_version, key, sdb_vernaux);
		}
		if ((int)entry->vn_next < 0) {
			bprintf ("Invalid vn_next\n");
			break;
		}
		i += entry->vn_next;
		snprintf (key, sizeof (key), "version%d", cnt );
		sdb_ns_set (sdb, key, sdb_version);
		if (!entry->vn_next) {
			break;
		}
	}
	free (need);
	return sdb;
beach:
	free (need);
	sdb_free (sdb_vernaux);
	sdb_free (sdb_version);
	sdb_free (sdb);
	return NULL;
}
