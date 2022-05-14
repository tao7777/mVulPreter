read_header(struct archive_read *a, struct archive_entry *entry,
            char head_type)
{
  const void *h;
  const char *p, *endp;
  struct rar *rar;
  struct rar_header rar_header;
  struct rar_file_header file_header;
  int64_t header_size;
  unsigned filename_size, end;
  char *filename;
  char *strp;
  char packed_size[8];
  char unp_size[8];
  int ttime;
  struct archive_string_conv *sconv, *fn_sconv;
  unsigned long crc32_val;
  int ret = (ARCHIVE_OK), ret2;

  rar = (struct rar *)(a->format->data);

  /* Setup a string conversion object for non-rar-unicode filenames. */
  sconv = rar->opt_sconv;
  if (sconv == NULL) {
    if (!rar->init_default_conversion) {
      rar->sconv_default =
          archive_string_default_conversion_for_read(
            &(a->archive));
      rar->init_default_conversion = 1;
    }
    sconv = rar->sconv_default;
  }


  if ((h = __archive_read_ahead(a, 7, NULL)) == NULL)
    return (ARCHIVE_FATAL);
  p = h;
  memcpy(&rar_header, p, sizeof(rar_header));
  rar->file_flags = archive_le16dec(rar_header.flags);
  header_size = archive_le16dec(rar_header.size);
  if (header_size < (int64_t)sizeof(file_header) + 7) {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
      "Invalid header size");
    return (ARCHIVE_FATAL);
  }
  crc32_val = crc32(0, (const unsigned char *)p + 2, 7 - 2);
  __archive_read_consume(a, 7);

  if (!(rar->file_flags & FHD_SOLID))
  {
    rar->compression_method = 0;
    rar->packed_size = 0;
    rar->unp_size = 0;
    rar->mtime = 0;
    rar->ctime = 0;
    rar->atime = 0;
    rar->arctime = 0;
    rar->mode = 0;
    memset(&rar->salt, 0, sizeof(rar->salt));
    rar->atime = 0;
    rar->ansec = 0;
    rar->ctime = 0;
    rar->cnsec = 0;
    rar->mtime = 0;
    rar->mnsec = 0;
    rar->arctime = 0;
    rar->arcnsec = 0;
  }
  else
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "RAR solid archive support unavailable.");
    return (ARCHIVE_FATAL);
  }

  if ((h = __archive_read_ahead(a, (size_t)header_size - 7, NULL)) == NULL)
    return (ARCHIVE_FATAL);

  /* File Header CRC check. */
  crc32_val = crc32(crc32_val, h, (unsigned)(header_size - 7));
  if ((crc32_val & 0xffff) != archive_le16dec(rar_header.crc)) {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
      "Header CRC error");
    return (ARCHIVE_FATAL);
  }
  /* If no CRC error, Go on parsing File Header. */
  p = h;
  endp = p + header_size - 7;
  memcpy(&file_header, p, sizeof(file_header));
  p += sizeof(file_header);

  rar->compression_method = file_header.method;

  ttime = archive_le32dec(file_header.file_time);
  rar->mtime = get_time(ttime);

  rar->file_crc = archive_le32dec(file_header.file_crc);

  if (rar->file_flags & FHD_PASSWORD)
  {
	archive_entry_set_is_data_encrypted(entry, 1);
	rar->has_encrypted_entries = 1;
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "RAR encryption support unavailable.");
    /* Since it is only the data part itself that is encrypted we can at least
       extract information about the currently processed entry and don't need
       to return ARCHIVE_FATAL here. */
    /*return (ARCHIVE_FATAL);*/
  }

  if (rar->file_flags & FHD_LARGE)
  {
    memcpy(packed_size, file_header.pack_size, 4);
    memcpy(packed_size + 4, p, 4); /* High pack size */
    p += 4;
    memcpy(unp_size, file_header.unp_size, 4);
    memcpy(unp_size + 4, p, 4); /* High unpack size */
    p += 4;
    rar->packed_size = archive_le64dec(&packed_size);
    rar->unp_size = archive_le64dec(&unp_size);
  }
  else
  {
    rar->packed_size = archive_le32dec(file_header.pack_size);
    rar->unp_size = archive_le32dec(file_header.unp_size);
  }

  if (rar->packed_size < 0 || rar->unp_size < 0)
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Invalid sizes specified.");
    return (ARCHIVE_FATAL);
  }

  rar->bytes_remaining = rar->packed_size;

  /* TODO: RARv3 subblocks contain comments. For now the complete block is
   * consumed at the end.
   */
  if (head_type == NEWSUB_HEAD) {
    size_t distance = p - (const char *)h;
    header_size += rar->packed_size;
    /* Make sure we have the extended data. */
    if ((h = __archive_read_ahead(a, (size_t)header_size - 7, NULL)) == NULL)
        return (ARCHIVE_FATAL);
    p = h;
    endp = p + header_size - 7;
    p += distance;
  }

  filename_size = archive_le16dec(file_header.name_size);
  if (p + filename_size > endp) {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
      "Invalid filename size");
    return (ARCHIVE_FATAL);
  }
  if (rar->filename_allocated < filename_size * 2 + 2) {
    char *newptr;
    size_t newsize = filename_size * 2 + 2;
    newptr = realloc(rar->filename, newsize);
    if (newptr == NULL) {
      archive_set_error(&a->archive, ENOMEM,
                        "Couldn't allocate memory.");
      return (ARCHIVE_FATAL);
    }
    rar->filename = newptr;
    rar->filename_allocated = newsize;
  }
  filename = rar->filename;
  memcpy(filename, p, filename_size);
  filename[filename_size] = '\0';
  if (rar->file_flags & FHD_UNICODE)
  {
    if (filename_size != strlen(filename))
    {
      unsigned char highbyte, flagbits, flagbyte;
      unsigned fn_end, offset;

      end = filename_size;
      fn_end = filename_size * 2;
      filename_size = 0;
      offset = (unsigned)strlen(filename) + 1;
      highbyte = *(p + offset++);
      flagbits = 0;
      flagbyte = 0;
      while (offset < end && filename_size < fn_end)
      {
        if (!flagbits)
        {
          flagbyte = *(p + offset++);
          flagbits = 8;
        }

        flagbits -= 2;
        switch((flagbyte >> flagbits) & 3)
        {
          case 0:
            filename[filename_size++] = '\0';
            filename[filename_size++] = *(p + offset++);
            break;
          case 1:
            filename[filename_size++] = highbyte;
            filename[filename_size++] = *(p + offset++);
            break;
          case 2:
            filename[filename_size++] = *(p + offset + 1);
            filename[filename_size++] = *(p + offset);
            offset += 2;
            break;
          case 3:
          {
            char extra, high;
            uint8_t length = *(p + offset++);

            if (length & 0x80) {
              extra = *(p + offset++);
              high = (char)highbyte;
            } else
              extra = high = 0;
            length = (length & 0x7f) + 2;
            while (length && filename_size < fn_end) {
              unsigned cp = filename_size >> 1;
              filename[filename_size++] = high;
              filename[filename_size++] = p[cp] + extra;
              length--;
            }
          }
          break;
        }
      }
      if (filename_size > fn_end) {
        archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
          "Invalid filename");
         return (ARCHIVE_FATAL);
       }
       filename[filename_size++] = '\0';
      /*
       * Do not increment filename_size here as the computations below
       * add the space for the terminating NUL explicitly.
       */
      filename[filename_size] = '\0';
 
       /* Decoded unicode form is UTF-16BE, so we have to update a string
        * conversion object for it. */
      if (rar->sconv_utf16be == NULL) {
        rar->sconv_utf16be = archive_string_conversion_from_charset(
           &a->archive, "UTF-16BE", 1);
        if (rar->sconv_utf16be == NULL)
          return (ARCHIVE_FATAL);
      }
      fn_sconv = rar->sconv_utf16be;

      strp = filename;
      while (memcmp(strp, "\x00\x00", 2))
      {
        if (!memcmp(strp, "\x00\\", 2))
          *(strp + 1) = '/';
        strp += 2;
      }
      p += offset;
    } else {
      /*
       * If FHD_UNICODE is set but no unicode data, this file name form
       * is UTF-8, so we have to update a string conversion object for
       * it accordingly.
       */
      if (rar->sconv_utf8 == NULL) {
        rar->sconv_utf8 = archive_string_conversion_from_charset(
           &a->archive, "UTF-8", 1);
        if (rar->sconv_utf8 == NULL)
          return (ARCHIVE_FATAL);
      }
      fn_sconv = rar->sconv_utf8;
      while ((strp = strchr(filename, '\\')) != NULL)
        *strp = '/';
      p += filename_size;
    }
  }
  else
  {
    fn_sconv = sconv;
    while ((strp = strchr(filename, '\\')) != NULL)
      *strp = '/';
    p += filename_size;
  }

  /* Split file in multivolume RAR. No more need to process header. */
  if (rar->filename_save &&
    filename_size == rar->filename_save_size &&
    !memcmp(rar->filename, rar->filename_save, filename_size + 1))
  {
    __archive_read_consume(a, header_size - 7);
    rar->cursor++;
    if (rar->cursor >= rar->nodes)
    {
      rar->nodes++;
      if ((rar->dbo =
        realloc(rar->dbo, sizeof(*rar->dbo) * rar->nodes)) == NULL)
      {
        archive_set_error(&a->archive, ENOMEM, "Couldn't allocate memory.");
        return (ARCHIVE_FATAL);
      }
      rar->dbo[rar->cursor].header_size = header_size;
      rar->dbo[rar->cursor].start_offset = -1;
      rar->dbo[rar->cursor].end_offset = -1;
    }
    if (rar->dbo[rar->cursor].start_offset < 0)
    {
      rar->dbo[rar->cursor].start_offset = a->filter->position;
      rar->dbo[rar->cursor].end_offset = rar->dbo[rar->cursor].start_offset +
        rar->packed_size;
    }
    return ret;
  }

  rar->filename_save = (char*)realloc(rar->filename_save,
                                      filename_size + 1);
  memcpy(rar->filename_save, rar->filename, filename_size + 1);
  rar->filename_save_size = filename_size;

  /* Set info for seeking */
  free(rar->dbo);
  if ((rar->dbo = calloc(1, sizeof(*rar->dbo))) == NULL)
  {
    archive_set_error(&a->archive, ENOMEM, "Couldn't allocate memory.");
    return (ARCHIVE_FATAL);
  }
  rar->dbo[0].header_size = header_size;
  rar->dbo[0].start_offset = -1;
  rar->dbo[0].end_offset = -1;
  rar->cursor = 0;
  rar->nodes = 1;

  if (rar->file_flags & FHD_SALT)
  {
    if (p + 8 > endp) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
        "Invalid header size");
      return (ARCHIVE_FATAL);
    }
    memcpy(rar->salt, p, 8);
    p += 8;
  }

  if (rar->file_flags & FHD_EXTTIME) {
    if (read_exttime(p, rar, endp) < 0) {
      archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
        "Invalid header size");
      return (ARCHIVE_FATAL);
    }
  }

  __archive_read_consume(a, header_size - 7);
  rar->dbo[0].start_offset = a->filter->position;
  rar->dbo[0].end_offset = rar->dbo[0].start_offset + rar->packed_size;

  switch(file_header.host_os)
  {
  case OS_MSDOS:
  case OS_OS2:
  case OS_WIN32:
    rar->mode = archive_le32dec(file_header.file_attr);
    if (rar->mode & FILE_ATTRIBUTE_DIRECTORY)
      rar->mode = AE_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    else
      rar->mode = AE_IFREG;
    rar->mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    break;

  case OS_UNIX:
  case OS_MAC_OS:
  case OS_BEOS:
    rar->mode = archive_le32dec(file_header.file_attr);
    break;

  default:
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Unknown file attributes from RAR file's host OS");
    return (ARCHIVE_FATAL);
  }

  rar->bytes_uncopied = rar->bytes_unconsumed = 0;
  rar->lzss.position = rar->offset = 0;
  rar->offset_seek = 0;
  rar->dictionary_size = 0;
  rar->offset_outgoing = 0;
  rar->br.cache_avail = 0;
  rar->br.avail_in = 0;
  rar->crc_calculated = 0;
  rar->entry_eof = 0;
  rar->valid = 1;
  rar->is_ppmd_block = 0;
  rar->start_new_table = 1;
  free(rar->unp_buffer);
  rar->unp_buffer = NULL;
  rar->unp_offset = 0;
  rar->unp_buffer_size = UNP_BUFFER_SIZE;
  memset(rar->lengthtable, 0, sizeof(rar->lengthtable));
  __archive_ppmd7_functions.Ppmd7_Free(&rar->ppmd7_context, &g_szalloc);
  rar->ppmd_valid = rar->ppmd_eod = 0;

  /* Don't set any archive entries for non-file header types */
  if (head_type == NEWSUB_HEAD)
    return ret;

  archive_entry_set_mtime(entry, rar->mtime, rar->mnsec);
  archive_entry_set_ctime(entry, rar->ctime, rar->cnsec);
  archive_entry_set_atime(entry, rar->atime, rar->ansec);
  archive_entry_set_size(entry, rar->unp_size);
  archive_entry_set_mode(entry, rar->mode);

  if (archive_entry_copy_pathname_l(entry, filename, filename_size, fn_sconv))
  {
    if (errno == ENOMEM)
    {
      archive_set_error(&a->archive, ENOMEM,
                        "Can't allocate memory for Pathname");
      return (ARCHIVE_FATAL);
    }
    archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
                      "Pathname cannot be converted from %s to current locale.",
                      archive_string_conversion_charset_name(fn_sconv));
    ret = (ARCHIVE_WARN);
  }

  if (((rar->mode) & AE_IFMT) == AE_IFLNK)
  {
    /* Make sure a symbolic-link file does not have its body. */
    rar->bytes_remaining = 0;
    archive_entry_set_size(entry, 0);

    /* Read a symbolic-link name. */
    if ((ret2 = read_symlink_stored(a, entry, sconv)) < (ARCHIVE_WARN))
      return ret2;
    if (ret > ret2)
      ret = ret2;
  }

  if (rar->bytes_remaining == 0)
    rar->entry_eof = 1;

  return ret;
}
