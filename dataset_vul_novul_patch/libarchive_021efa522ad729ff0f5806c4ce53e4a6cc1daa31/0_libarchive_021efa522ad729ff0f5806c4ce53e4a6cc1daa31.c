parse_codes(struct archive_read *a)
{
  int i, j, val, n, r;
  unsigned char bitlengths[MAX_SYMBOLS], zerocount, ppmd_flags;
  unsigned int maxorder;
  struct huffman_code precode;
  struct rar *rar = (struct rar *)(a->format->data);
  struct rar_br *br = &(rar->br);

  free_codes(a);

  /* Skip to the next byte */
  rar_br_consume_unalined_bits(br);

  /* PPMd block flag */
  if (!rar_br_read_ahead(a, br, 1))
    goto truncated_data;
  if ((rar->is_ppmd_block = rar_br_bits(br, 1)) != 0)
  {
    rar_br_consume(br, 1);
    if (!rar_br_read_ahead(a, br, 7))
      goto truncated_data;
    ppmd_flags = rar_br_bits(br, 7);
    rar_br_consume(br, 7);

    /* Memory is allocated in MB */
    if (ppmd_flags & 0x20)
    {
      if (!rar_br_read_ahead(a, br, 8))
        goto truncated_data;
      rar->dictionary_size = (rar_br_bits(br, 8) + 1) << 20;
      rar_br_consume(br, 8);
    }

    if (ppmd_flags & 0x40)
    {
      if (!rar_br_read_ahead(a, br, 8))
        goto truncated_data;
      rar->ppmd_escape = rar->ppmd7_context.InitEsc = rar_br_bits(br, 8);
      rar_br_consume(br, 8);
    }
    else
      rar->ppmd_escape = 2;

    if (ppmd_flags & 0x20)
    {
      maxorder = (ppmd_flags & 0x1F) + 1;
      if(maxorder > 16)
        maxorder = 16 + (maxorder - 16) * 3;

      if (maxorder == 1)
      {
        archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                          "Truncated RAR file data");
        return (ARCHIVE_FATAL);
      }

      /* Make sure ppmd7_contest is freed before Ppmd7_Construct
       * because reading a broken file cause this abnormal sequence. */
      __archive_ppmd7_functions.Ppmd7_Free(&rar->ppmd7_context);

      rar->bytein.a = a;
      rar->bytein.Read = &ppmd_read;
      __archive_ppmd7_functions.PpmdRAR_RangeDec_CreateVTable(&rar->range_dec);
      rar->range_dec.Stream = &rar->bytein;
      __archive_ppmd7_functions.Ppmd7_Construct(&rar->ppmd7_context);

      if (rar->dictionary_size == 0) {
	      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                          "Invalid zero dictionary size");
	      return (ARCHIVE_FATAL);
      }

      if (!__archive_ppmd7_functions.Ppmd7_Alloc(&rar->ppmd7_context,
        rar->dictionary_size))
      {
        archive_set_error(&a->archive, ENOMEM,
                          "Out of memory");
        return (ARCHIVE_FATAL);
      }
      if (!__archive_ppmd7_functions.PpmdRAR_RangeDec_Init(&rar->range_dec))
      {
        archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                          "Unable to initialize PPMd range decoder");
        return (ARCHIVE_FATAL);
      }
      __archive_ppmd7_functions.Ppmd7_Init(&rar->ppmd7_context, maxorder);
      rar->ppmd_valid = 1;
    }
    else
    {
      if (!rar->ppmd_valid) {
        archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                          "Invalid PPMd sequence");
        return (ARCHIVE_FATAL);
      }
      if (!__archive_ppmd7_functions.PpmdRAR_RangeDec_Init(&rar->range_dec))
      {
        archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                          "Unable to initialize PPMd range decoder");
        return (ARCHIVE_FATAL);
      }
    }
  }
  else
  {
    rar_br_consume(br, 1);

    /* Keep existing table flag */
    if (!rar_br_read_ahead(a, br, 1))
      goto truncated_data;
    if (!rar_br_bits(br, 1))
      memset(rar->lengthtable, 0, sizeof(rar->lengthtable));
    rar_br_consume(br, 1);

    memset(&bitlengths, 0, sizeof(bitlengths));
    for (i = 0; i < MAX_SYMBOLS;)
    {
      if (!rar_br_read_ahead(a, br, 4))
        goto truncated_data;
      bitlengths[i++] = rar_br_bits(br, 4);
      rar_br_consume(br, 4);
      if (bitlengths[i-1] == 0xF)
      {
        if (!rar_br_read_ahead(a, br, 4))
          goto truncated_data;
        zerocount = rar_br_bits(br, 4);
        rar_br_consume(br, 4);
        if (zerocount)
        {
          i--;
          for (j = 0; j < zerocount + 2 && i < MAX_SYMBOLS; j++)
            bitlengths[i++] = 0;
        }
      }
    }

    memset(&precode, 0, sizeof(precode));
    r = create_code(a, &precode, bitlengths, MAX_SYMBOLS, MAX_SYMBOL_LENGTH);
    if (r != ARCHIVE_OK) {
      free(precode.tree);
      free(precode.table);
      return (r);
    }

    for (i = 0; i < HUFFMAN_TABLE_SIZE;)
    {
      if ((val = read_next_symbol(a, &precode)) < 0) {
        free(precode.tree);
        free(precode.table);
        return (ARCHIVE_FATAL);
      }
      if (val < 16)
      {
        rar->lengthtable[i] = (rar->lengthtable[i] + val) & 0xF;
        i++;
      }
      else if (val < 18)
      {
        if (i == 0)
        {
          free(precode.tree);
          free(precode.table);
          archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                            "Internal error extracting RAR file.");
          return (ARCHIVE_FATAL);
        }

        if(val == 16) {
          if (!rar_br_read_ahead(a, br, 3)) {
            free(precode.tree);
            free(precode.table);
            goto truncated_data;
          }
          n = rar_br_bits(br, 3) + 3;
          rar_br_consume(br, 3);
        } else {
          if (!rar_br_read_ahead(a, br, 7)) {
            free(precode.tree);
            free(precode.table);
            goto truncated_data;
          }
          n = rar_br_bits(br, 7) + 11;
          rar_br_consume(br, 7);
        }

        for (j = 0; j < n && i < HUFFMAN_TABLE_SIZE; j++)
        {
          rar->lengthtable[i] = rar->lengthtable[i-1];
          i++;
        }
      }
      else
      {
        if(val == 18) {
          if (!rar_br_read_ahead(a, br, 3)) {
            free(precode.tree);
            free(precode.table);
            goto truncated_data;
          }
          n = rar_br_bits(br, 3) + 3;
          rar_br_consume(br, 3);
        } else {
          if (!rar_br_read_ahead(a, br, 7)) {
            free(precode.tree);
            free(precode.table);
            goto truncated_data;
          }
          n = rar_br_bits(br, 7) + 11;
          rar_br_consume(br, 7);
        }

        for(j = 0; j < n && i < HUFFMAN_TABLE_SIZE; j++)
          rar->lengthtable[i++] = 0;
      }
    }
    free(precode.tree);
    free(precode.table);

    r = create_code(a, &rar->maincode, &rar->lengthtable[0], MAINCODE_SIZE,
                MAX_SYMBOL_LENGTH);
    if (r != ARCHIVE_OK)
      return (r);
    r = create_code(a, &rar->offsetcode, &rar->lengthtable[MAINCODE_SIZE],
                OFFSETCODE_SIZE, MAX_SYMBOL_LENGTH);
    if (r != ARCHIVE_OK)
      return (r);
    r = create_code(a, &rar->lowoffsetcode,
                &rar->lengthtable[MAINCODE_SIZE + OFFSETCODE_SIZE],
                LOWOFFSETCODE_SIZE, MAX_SYMBOL_LENGTH);
    if (r != ARCHIVE_OK)
      return (r);
    r = create_code(a, &rar->lengthcode,
                &rar->lengthtable[MAINCODE_SIZE + OFFSETCODE_SIZE +
                LOWOFFSETCODE_SIZE], LENGTHCODE_SIZE, MAX_SYMBOL_LENGTH);
    if (r != ARCHIVE_OK)
      return (r);
  }

  if (!rar->dictionary_size || !rar->lzss.window)
  {
    /* Seems as though dictionary sizes are not used. Even so, minimize
     * memory usage as much as possible.
     */
    void *new_window;
    unsigned int new_size;

    if (rar->unp_size >= DICTIONARY_MAX_SIZE)
       new_size = DICTIONARY_MAX_SIZE;
     else
       new_size = rar_fls((unsigned int)rar->unp_size) << 1;
    if (new_size == 0) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                        "Zero window size is invalid.");
      return (ARCHIVE_FATAL);
    }
     new_window = realloc(rar->lzss.window, new_size);
     if (new_window == NULL) {
       archive_set_error(&a->archive, ENOMEM,
                        "Unable to allocate memory for uncompressed data.");
      return (ARCHIVE_FATAL);
    }
    rar->lzss.window = (unsigned char *)new_window;
    rar->dictionary_size = new_size;
    memset(rar->lzss.window, 0, rar->dictionary_size);
    rar->lzss.mask = rar->dictionary_size - 1;
  }

  rar->start_new_table = 0;
  return (ARCHIVE_OK);
truncated_data:
  archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                    "Truncated RAR file data");
  rar->valid = 0;
  return (ARCHIVE_FATAL);
}
