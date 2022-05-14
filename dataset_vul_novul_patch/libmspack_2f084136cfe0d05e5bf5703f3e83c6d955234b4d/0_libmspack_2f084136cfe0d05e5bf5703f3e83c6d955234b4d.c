static int chmd_read_headers(struct mspack_system *sys, struct mspack_file *fh,
                             struct mschmd_header *chm, int entire)
{
  unsigned int section, name_len, x, errors, num_chunks;
  unsigned char buf[0x54], *chunk = NULL, *name, *p, *end;
  struct mschmd_file *fi, *link = NULL;
  off_t offset, length;
  int num_entries;

  /* initialise pointers */
  chm->files         = NULL;
  chm->sysfiles      = NULL;
  chm->chunk_cache   = NULL;
  chm->sec0.base.chm = chm;
  chm->sec0.base.id  = 0;
  chm->sec1.base.chm = chm;
  chm->sec1.base.id  = 1;
  chm->sec1.content  = NULL;
  chm->sec1.control  = NULL;
  chm->sec1.spaninfo = NULL;
  chm->sec1.rtable   = NULL;

  /* read the first header */
  if (sys->read(fh, &buf[0], chmhead_SIZEOF) != chmhead_SIZEOF) {
    return MSPACK_ERR_READ;
  }

  /* check ITSF signature */
  if (EndGetI32(&buf[chmhead_Signature]) != 0x46535449) {
    return MSPACK_ERR_SIGNATURE;
  }

  /* check both header GUIDs */
  if (memcmp(&buf[chmhead_GUID1], &guids[0], 32L) != 0) {
    D(("incorrect GUIDs"))
    return MSPACK_ERR_SIGNATURE;
  }

  chm->version   = EndGetI32(&buf[chmhead_Version]);
  chm->timestamp = EndGetM32(&buf[chmhead_Timestamp]);
  chm->language  = EndGetI32(&buf[chmhead_LanguageID]);
  if (chm->version > 3) {
    sys->message(fh, "WARNING; CHM version > 3");
  }

  /* read the header section table */
  if (sys->read(fh, &buf[0], chmhst3_SIZEOF) != chmhst3_SIZEOF) {
    return MSPACK_ERR_READ;
  }

  /* chmhst3_OffsetCS0 does not exist in version 1 or 2 CHM files.
   * The offset will be corrected later, once HS1 is read.
   */
  if (read_off64(&offset,           &buf[chmhst_OffsetHS0],  sys, fh) ||
      read_off64(&chm->dir_offset,  &buf[chmhst_OffsetHS1],  sys, fh) ||
      read_off64(&chm->sec0.offset, &buf[chmhst3_OffsetCS0], sys, fh))
  {
    return MSPACK_ERR_DATAFORMAT;
  }

  /* seek to header section 0 */
  if (sys->seek(fh, offset, MSPACK_SYS_SEEK_START)) {
    return MSPACK_ERR_SEEK;
  }

  /* read header section 0 */
  if (sys->read(fh, &buf[0], chmhs0_SIZEOF) != chmhs0_SIZEOF) {
    return MSPACK_ERR_READ;
  }
  if (read_off64(&chm->length, &buf[chmhs0_FileLen], sys, fh)) {
    return MSPACK_ERR_DATAFORMAT;
  }

  /* seek to header section 1 */
  if (sys->seek(fh, chm->dir_offset, MSPACK_SYS_SEEK_START)) {
    return MSPACK_ERR_SEEK;
  }

  /* read header section 1 */
  if (sys->read(fh, &buf[0], chmhs1_SIZEOF) != chmhs1_SIZEOF) {
    return MSPACK_ERR_READ;
  }

  chm->dir_offset = sys->tell(fh);
  chm->chunk_size = EndGetI32(&buf[chmhs1_ChunkSize]);
  chm->density    = EndGetI32(&buf[chmhs1_Density]);
  chm->depth      = EndGetI32(&buf[chmhs1_Depth]);
  chm->index_root = EndGetI32(&buf[chmhs1_IndexRoot]);
  chm->num_chunks = EndGetI32(&buf[chmhs1_NumChunks]);
  chm->first_pmgl = EndGetI32(&buf[chmhs1_FirstPMGL]);
  chm->last_pmgl  = EndGetI32(&buf[chmhs1_LastPMGL]);

  if (chm->version < 3) {
    /* versions before 3 don't have chmhst3_OffsetCS0 */
    chm->sec0.offset = chm->dir_offset + (chm->chunk_size * chm->num_chunks);
  }

  /* check if content offset or file size is wrong */
  if (chm->sec0.offset > chm->length) {
    D(("content section begins after file has ended"))
    return MSPACK_ERR_DATAFORMAT;
  }
  
  /* ensure there are chunks and that chunk size is
   * large enough for signature and num_entries */
  if (chm->chunk_size < (pmgl_Entries + 2)) {
    D(("chunk size not large enough"))
    return MSPACK_ERR_DATAFORMAT;
  }
  if (chm->num_chunks == 0) {
    D(("no chunks"))
    return MSPACK_ERR_DATAFORMAT;
  }

  /* The chunk_cache data structure is not great; large values for num_chunks
   * or num_chunks*chunk_size can exhaust all memory. Until a better chunk
   * cache is implemented, put arbitrary limits on num_chunks and chunk size.
   */
  if (chm->num_chunks > 100000) {
    D(("more than 100,000 chunks"))
    return MSPACK_ERR_DATAFORMAT;
  }   
  if ((off_t)chm->chunk_size * (off_t)chm->num_chunks > chm->length) {
    D(("chunks larger than entire file"))
    return MSPACK_ERR_DATAFORMAT;
  }

  /* common sense checks on header section 1 fields */
  if ((chm->chunk_size & (chm->chunk_size - 1)) != 0) {
    sys->message(fh, "WARNING; chunk size is not a power of two");
  }
  if (chm->first_pmgl != 0) {
    sys->message(fh, "WARNING; first PMGL chunk is not zero");
  }
  if (chm->first_pmgl > chm->last_pmgl) {
    D(("first pmgl chunk is after last pmgl chunk"))
    return MSPACK_ERR_DATAFORMAT;
  }
  if (chm->index_root != 0xFFFFFFFF && chm->index_root >= chm->num_chunks) {
    D(("index_root outside valid range"))
    return MSPACK_ERR_DATAFORMAT;
  }

  /* if we are doing a quick read, stop here! */
  if (!entire) {
    return MSPACK_ERR_OK;
  }

  /* seek to the first PMGL chunk, and reduce the number of chunks to read */
  if ((x = chm->first_pmgl) != 0) {
    if (sys->seek(fh,(off_t) (x * chm->chunk_size), MSPACK_SYS_SEEK_CUR)) {
      return MSPACK_ERR_SEEK;
    }
  }
  num_chunks = chm->last_pmgl - x + 1;

  if (!(chunk = (unsigned char *) sys->alloc(sys, (size_t)chm->chunk_size))) {
    return MSPACK_ERR_NOMEMORY;
  }

  /* read and process all chunks from FirstPMGL to LastPMGL */
  errors = 0;
  while (num_chunks--) {
    /* read next chunk */
    if (sys->read(fh, chunk, (int)chm->chunk_size) != (int)chm->chunk_size) {
      sys->free(chunk);
      return MSPACK_ERR_READ;
    }

    /* process only directory (PMGL) chunks */
    if (EndGetI32(&chunk[pmgl_Signature]) != 0x4C474D50) continue;

    if (EndGetI32(&chunk[pmgl_QuickRefSize]) < 2) {
      sys->message(fh, "WARNING; PMGL quickref area is too small");
    }
    if (EndGetI32(&chunk[pmgl_QuickRefSize]) > 
        ((int)chm->chunk_size - pmgl_Entries))
    {
      sys->message(fh, "WARNING; PMGL quickref area is too large");
    }

    p = &chunk[pmgl_Entries];
    end = &chunk[chm->chunk_size - 2];
    num_entries = EndGetI16(end);

    while (num_entries--) {
      READ_ENCINT(name_len);
      if (name_len > (unsigned int) (end - p)) goto chunk_end;
      name = p; p += name_len;
      READ_ENCINT(section);
      READ_ENCINT(offset);
      READ_ENCINT(length);

      /* ignore blank or one-char (e.g. "/") filenames we'd return as blank */
      if (name_len < 2 || !name[0] || !name[1]) continue;

      /* empty files and directory names are stored as a file entry at
       * offset 0 with length 0. We want to keep empty files, but not
       * directory names, which end with a "/" */
      if ((offset == 0) && (length == 0)) {
        if ((name_len > 0) && (name[name_len-1] == '/')) continue;
      }

      if (section > 1) {
        sys->message(fh, "invalid section number '%u'.", section);
        continue;
      }

      if (!(fi = (struct mschmd_file *) sys->alloc(sys, sizeof(struct mschmd_file) + name_len + 1))) {
        sys->free(chunk);
        return MSPACK_ERR_NOMEMORY;
      }

      fi->next     = NULL;
      fi->filename = (char *) &fi[1];
      fi->section  = ((section == 0) ? (struct mschmd_section *) (&chm->sec0)
                                     : (struct mschmd_section *) (&chm->sec1));
      fi->offset   = offset;
      fi->length   = length;
      sys->copy(name, fi->filename, (size_t) name_len);
      fi->filename[name_len] = '\0';
 
       if (name[0] == ':' && name[1] == ':') {
         /* system file */
        if (name_len == 40 && memcmp(name, content_name, 40) == 0) {
          chm->sec1.content = fi;
        }
        else if (name_len == 44 && memcmp(name, control_name, 44) == 0) {
          chm->sec1.control = fi;
        }
        else if (name_len == 41 && memcmp(name, spaninfo_name, 41) == 0) {
          chm->sec1.spaninfo = fi;
        }
        else if (name_len == 105 && memcmp(name, rtable_name, 105) == 0) {
          chm->sec1.rtable = fi;
         }
         fi->next = chm->sysfiles;
         chm->sysfiles = fi;
      }
      else {
        /* normal file */
        if (link) link->next = fi; else chm->files = fi;
        link = fi;
      }
    }

    /* this is reached either when num_entries runs out, or if
     * reading data from the chunk reached a premature end of chunk */
  chunk_end:
    if (num_entries >= 0) {
      D(("chunk ended before all entries could be read"))
      errors++;
    }

  }
  sys->free(chunk);
  return (errors > 0) ? MSPACK_ERR_DATAFORMAT : MSPACK_ERR_OK;
}
