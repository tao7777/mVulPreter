 pdf_repair_xref(fz_context *ctx, pdf_document *doc)
 {
	int c;
	pdf_lexbuf *buf = &doc->lexbuf.base;
	int num_roots = 0;
	int max_roots = 0;

	fz_var(encrypt);
	fz_var(id);
	fz_var(roots);
	fz_var(num_roots);
	fz_var(max_roots);
	fz_var(info);
	fz_var(list);
	fz_var(obj);

	if (doc->repair_attempted)
		fz_throw(ctx, FZ_ERROR_GENERIC, "Repair failed already - not trying again");
	doc->repair_attempted = 1;

	doc->dirty = 1;
	/* Can't support incremental update after repair */
	doc->freeze_updates = 1;

	fz_seek(ctx, doc->file, 0, 0);

	fz_try(ctx)
	{
		pdf_xref_entry *entry;
		listlen = 0;
		listcap = 1024;
		list = fz_malloc_array(ctx, listcap, sizeof(struct entry));

		/* look for '%PDF' version marker within first kilobyte of file */
		n = fz_read(ctx, doc->file, (unsigned char *)buf->scratch, fz_mini(buf->size, 1024));

		fz_seek(ctx, doc->file, 0, 0);
		if (n >= 4)
		{
			for (j = 0; j < n - 4; j++)
			{
				if (memcmp(&buf->scratch[j], "%PDF", 4) == 0)
				{
					fz_seek(ctx, doc->file, j + 8, 0); /* skip "%PDF-X.Y" */
					break;
				}
			}
		}

		/* skip comment line after version marker since some generators
		 * forget to terminate the comment with a newline */
		c = fz_read_byte(ctx, doc->file);
		while (c >= 0 && (c == ' ' || c == '%'))
			c = fz_read_byte(ctx, doc->file);
		fz_unread_byte(ctx, doc->file);

		while (1)
		{
			tmpofs = fz_tell(ctx, doc->file);
			if (tmpofs < 0)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot tell in file");

			fz_try(ctx)
			{
				tok = pdf_lex_no_string(ctx, doc->file, buf);
			}
			fz_catch(ctx)
			{
				fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
				fz_warn(ctx, "ignoring the rest of the file");
				break;
			}

			/* If we have the next token already, then we'll jump
			 * back here, rather than going through the top of
			 * the loop. */
		have_next_token:

			if (tok == PDF_TOK_INT)
			{
				if (buf->i < 0)
				{
					num = 0;
					gen = 0;
					continue;
				}
				numofs = genofs;
				num = gen;
				genofs = tmpofs;
				gen = buf->i;
			}

			else if (tok == PDF_TOK_OBJ)
			{
				pdf_obj *root = NULL;

				fz_try(ctx)
				{
					stm_len = 0;
					stm_ofs = 0;
					tok = pdf_repair_obj(ctx, doc, buf, &stm_ofs, &stm_len, &encrypt, &id, NULL, &tmpofs, &root);
					if (root)
						add_root(ctx, root, &roots, &num_roots, &max_roots);
				}
				fz_always(ctx)
				{
					pdf_drop_obj(ctx, root);
				}
				fz_catch(ctx)
				{
					fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
					/* If we haven't seen a root yet, there is nothing
					 * we can do, but give up. Otherwise, we'll make
					 * do. */
					if (!roots)
						fz_rethrow(ctx);
					fz_warn(ctx, "cannot parse object (%d %d R) - ignoring rest of file", num, gen);
					break;
				}

				if (num <= 0 || num > MAX_OBJECT_NUMBER)
				{
					fz_warn(ctx, "ignoring object with invalid object number (%d %d R)", num, gen);
					goto have_next_token;
				}

				gen = fz_clampi(gen, 0, 65535);

				if (listlen + 1 == listcap)
				{
					listcap = (listcap * 3) / 2;
					list = fz_resize_array(ctx, list, listcap, sizeof(struct entry));
				}

				list[listlen].num = num;
				list[listlen].gen = gen;
				list[listlen].ofs = numofs;
				list[listlen].stm_ofs = stm_ofs;
				list[listlen].stm_len = stm_len;
				listlen ++;

				if (num > maxnum)
					maxnum = num;

				goto have_next_token;
			}

			/* If we find a dictionary it is probably the trailer,
			 * but could be a stream (or bogus) dictionary caused
			 * by a corrupt file. */
			else if (tok == PDF_TOK_OPEN_DICT)
			{
				fz_try(ctx)
				{
					dict = pdf_parse_dict(ctx, doc, doc->file, buf);
				}
				fz_catch(ctx)
				{
					fz_rethrow_if(ctx, FZ_ERROR_TRYLATER);
					/* If this was the real trailer dict
					 * it was broken, in which case we are
					 * in trouble. Keep going though in
					 * case this was just a bogus dict. */
					continue;
				}

				obj = pdf_dict_get(ctx, dict, PDF_NAME_Encrypt);
				if (obj)
				{
					pdf_drop_obj(ctx, encrypt);
					encrypt = pdf_keep_obj(ctx, obj);
				}

				obj = pdf_dict_get(ctx, dict, PDF_NAME_ID);
				if (obj && (!id || !encrypt || pdf_dict_get(ctx, dict, PDF_NAME_Encrypt)))
				{
					pdf_drop_obj(ctx, id);
					id = pdf_keep_obj(ctx, obj);
				}

				obj = pdf_dict_get(ctx, dict, PDF_NAME_Root);
				if (obj)
					add_root(ctx, obj, &roots, &num_roots, &max_roots);

				obj = pdf_dict_get(ctx, dict, PDF_NAME_Info);
				if (obj)
				{
					pdf_drop_obj(ctx, info);
					info = pdf_keep_obj(ctx, obj);
				}

				pdf_drop_obj(ctx, dict);
				obj = NULL;
			}

			else if (tok == PDF_TOK_EOF)
				break;
			else
			{
				if (tok == PDF_TOK_ERROR)
					fz_read_byte(ctx, doc->file);
				num = 0;
				gen = 0;
			}

		}

		if (listlen == 0)
			fz_throw(ctx, FZ_ERROR_GENERIC, "no objects found");

		/* make xref reasonable */

		/*
			Dummy access to entry to assure sufficient space in the xref table
			and avoid repeated reallocs in the loop
		*/
		/* Ensure that the first xref table is a 'solid' one from
		 * 0 to maxnum. */
		pdf_ensure_solid_xref(ctx, doc, maxnum);

		for (i = 1; i < maxnum; i++)
		{
			entry = pdf_get_populating_xref_entry(ctx, doc, i);
			if (entry->obj != NULL)
				continue;
			entry->type = 'f';
			entry->ofs = 0;
			entry->gen = 0;
			entry->num = 0;

			entry->stm_ofs = 0;
		}

		for (i = 0; i < listlen; i++)
		{
			entry = pdf_get_populating_xref_entry(ctx, doc, list[i].num);
			entry->type = 'n';
			entry->ofs = list[i].ofs;
			entry->gen = list[i].gen;
			entry->num = list[i].num;

			entry->stm_ofs = list[i].stm_ofs;

			/* correct stream length for unencrypted documents */
			if (!encrypt && list[i].stm_len >= 0)
			{
				dict = pdf_load_object(ctx, doc, list[i].num);

				length = pdf_new_int(ctx, doc, list[i].stm_len);
				pdf_dict_put(ctx, dict, PDF_NAME_Length, length);
				pdf_drop_obj(ctx, length);

				pdf_drop_obj(ctx, dict);
			}
		}

		entry = pdf_get_populating_xref_entry(ctx, doc, 0);
		entry->type = 'f';
		entry->ofs = 0;
		entry->gen = 65535;
		entry->num = 0;
		entry->stm_ofs = 0;

		next = 0;
                        /* correct stream length for unencrypted documents */
                        if (!encrypt && list[i].stm_len >= 0)
                        {
                                dict = pdf_load_object(ctx, doc, list[i].num);
 
                                length = pdf_new_int(ctx, doc, list[i].stm_len);
                               pdf_dict_put(ctx, dict, PDF_NAME_Length, length);
                               pdf_drop_obj(ctx, length);
                                pdf_drop_obj(ctx, dict);
                        }
                }

		obj = pdf_new_dict(ctx, doc, 5);
		/* During repair there is only a single xref section */
		pdf_set_populating_xref_trailer(ctx, doc, obj);
		pdf_drop_obj(ctx, obj);
		obj = NULL;

		obj = pdf_new_int(ctx, doc, maxnum + 1);
		pdf_dict_put(ctx, pdf_trailer(ctx, doc), PDF_NAME_Size, obj);
		pdf_drop_obj(ctx, obj);
		obj = NULL;

		if (roots)
		{
			int i;
			for (i = num_roots-1; i > 0; i--)
			{
				if (pdf_is_dict(ctx, roots[i]))
					break;
			}
			if (i >= 0)
			{
				pdf_dict_put(ctx, pdf_trailer(ctx, doc), PDF_NAME_Root, roots[i]);
			}
		}
		if (info)
		{
			pdf_dict_put(ctx, pdf_trailer(ctx, doc), PDF_NAME_Info, info);
			pdf_drop_obj(ctx, info);
			info = NULL;
		}

		if (encrypt)
		{
			if (pdf_is_indirect(ctx, encrypt))
			{
				/* create new reference with non-NULL xref pointer */
				obj = pdf_new_indirect(ctx, doc, pdf_to_num(ctx, encrypt), pdf_to_gen(ctx, encrypt));
				pdf_drop_obj(ctx, encrypt);
				encrypt = obj;
				obj = NULL;
			}
			pdf_dict_put(ctx, pdf_trailer(ctx, doc), PDF_NAME_Encrypt, encrypt);
			pdf_drop_obj(ctx, encrypt);
			encrypt = NULL;
		}

		if (id)
		{
			if (pdf_is_indirect(ctx, id))
			{
				/* create new reference with non-NULL xref pointer */
				obj = pdf_new_indirect(ctx, doc, pdf_to_num(ctx, id), pdf_to_gen(ctx, id));
				pdf_drop_obj(ctx, id);
				id = obj;
				obj = NULL;
			}
			pdf_dict_put(ctx, pdf_trailer(ctx, doc), PDF_NAME_ID, id);
			pdf_drop_obj(ctx, id);
			id = NULL;
		}

		fz_free(ctx, list);
	}
