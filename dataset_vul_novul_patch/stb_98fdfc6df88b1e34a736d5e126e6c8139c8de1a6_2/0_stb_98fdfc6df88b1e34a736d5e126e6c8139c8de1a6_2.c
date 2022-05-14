static int start_decoder(vorb *f)
{
   uint8 header[6], x,y;
   int len,i,j,k, max_submaps = 0;
   int longest_floorlist=0;


   if (!start_page(f))                              return FALSE;
   if (!(f->page_flag & PAGEFLAG_first_page))       return error(f, VORBIS_invalid_first_page);
   if (f->page_flag & PAGEFLAG_last_page)           return error(f, VORBIS_invalid_first_page);
   if (f->page_flag & PAGEFLAG_continued_packet)    return error(f, VORBIS_invalid_first_page);
   if (f->segment_count != 1)                       return error(f, VORBIS_invalid_first_page);
   if (f->segments[0] != 30) {
      if (f->segments[0] == 64 &&
          getn(f, header, 6) &&
          header[0] == 'f' &&
          header[1] == 'i' &&
          header[2] == 's' &&
          header[3] == 'h' &&
          header[4] == 'e' &&
          header[5] == 'a' &&
          get8(f)   == 'd' &&
          get8(f)   == '\0')                        return error(f, VORBIS_ogg_skeleton_not_supported);
      else
                                                    return error(f, VORBIS_invalid_first_page);
   }

   if (get8(f) != VORBIS_packet_id)                 return error(f, VORBIS_invalid_first_page);
   if (!getn(f, header, 6))                         return error(f, VORBIS_unexpected_eof);
   if (!vorbis_validate(header))                    return error(f, VORBIS_invalid_first_page);
   if (get32(f) != 0)                               return error(f, VORBIS_invalid_first_page);
   f->channels = get8(f); if (!f->channels)         return error(f, VORBIS_invalid_first_page);
   if (f->channels > STB_VORBIS_MAX_CHANNELS)       return error(f, VORBIS_too_many_channels);
   f->sample_rate = get32(f); if (!f->sample_rate)  return error(f, VORBIS_invalid_first_page);
   get32(f); // bitrate_maximum
   get32(f); // bitrate_nominal
   get32(f); // bitrate_minimum
   x = get8(f);
   {
      int log0,log1;
      log0 = x & 15;
      log1 = x >> 4;
      f->blocksize_0 = 1 << log0;
      f->blocksize_1 = 1 << log1;
      if (log0 < 6 || log0 > 13)                       return error(f, VORBIS_invalid_setup);
      if (log1 < 6 || log1 > 13)                       return error(f, VORBIS_invalid_setup);
      if (log0 > log1)                                 return error(f, VORBIS_invalid_setup);
   }

   x = get8(f);
   if (!(x & 1))                                    return error(f, VORBIS_invalid_first_page);

   if (!start_page(f))                              return FALSE;

   if (!start_packet(f))                            return FALSE;
   do {
      len = next_segment(f);
      skip(f, len);
      f->bytes_in_seg = 0;
   } while (len);

   if (!start_packet(f))                            return FALSE;

   #ifndef STB_VORBIS_NO_PUSHDATA_API
   if (IS_PUSH_MODE(f)) {
      if (!is_whole_packet_present(f, TRUE)) {
         if (f->error == VORBIS_invalid_stream)
            f->error = VORBIS_invalid_setup;
         return FALSE;
      }
   }
   #endif

   crc32_init(); // always init it, to avoid multithread race conditions

   if (get8_packet(f) != VORBIS_packet_setup)       return error(f, VORBIS_invalid_setup);
   for (i=0; i < 6; ++i) header[i] = get8_packet(f);
   if (!vorbis_validate(header))                    return error(f, VORBIS_invalid_setup);


   f->codebook_count = get_bits(f,8) + 1;
   f->codebooks = (Codebook *) setup_malloc(f, sizeof(*f->codebooks) * f->codebook_count);
   if (f->codebooks == NULL)                        return error(f, VORBIS_outofmem);
   memset(f->codebooks, 0, sizeof(*f->codebooks) * f->codebook_count);
   for (i=0; i < f->codebook_count; ++i) {
      uint32 *values;
      int ordered, sorted_count;
      int total=0;
      uint8 *lengths;
      Codebook *c = f->codebooks+i;
      CHECK(f);
      x = get_bits(f, 8); if (x != 0x42)            return error(f, VORBIS_invalid_setup);
      x = get_bits(f, 8); if (x != 0x43)            return error(f, VORBIS_invalid_setup);
      x = get_bits(f, 8); if (x != 0x56)            return error(f, VORBIS_invalid_setup);
      x = get_bits(f, 8);
      c->dimensions = (get_bits(f, 8)<<8) + x;
      x = get_bits(f, 8);
      y = get_bits(f, 8);
      c->entries = (get_bits(f, 8)<<16) + (y<<8) + x;
      ordered = get_bits(f,1);
      c->sparse = ordered ? 0 : get_bits(f,1);

      if (c->dimensions == 0 && c->entries != 0)    return error(f, VORBIS_invalid_setup);

      if (c->sparse)
         lengths = (uint8 *) setup_temp_malloc(f, c->entries);
      else
         lengths = c->codeword_lengths = (uint8 *) setup_malloc(f, c->entries);

      if (!lengths) return error(f, VORBIS_outofmem);

      if (ordered) {
         int current_entry = 0;
         int current_length = get_bits(f,5) + 1;
          while (current_entry < c->entries) {
             int limit = c->entries - current_entry;
             int n = get_bits(f, ilog(limit));
            if (current_length >= 32) return error(f, VORBIS_invalid_setup);
             if (current_entry + n > (int) c->entries) { return error(f, VORBIS_invalid_setup); }
             memset(lengths + current_entry, current_length, n);
             current_entry += n;
            ++current_length;
         }
      } else {
         for (j=0; j < c->entries; ++j) {
            int present = c->sparse ? get_bits(f,1) : 1;
            if (present) {
               lengths[j] = get_bits(f, 5) + 1;
               ++total;
               if (lengths[j] == 32)
                  return error(f, VORBIS_invalid_setup);
            } else {
               lengths[j] = NO_CODE;
            }
         }
      }

      if (c->sparse && total >= c->entries >> 2) {
         if (c->entries > (int) f->setup_temp_memory_required)
            f->setup_temp_memory_required = c->entries;

         c->codeword_lengths = (uint8 *) setup_malloc(f, c->entries);
         if (c->codeword_lengths == NULL) return error(f, VORBIS_outofmem);
         memcpy(c->codeword_lengths, lengths, c->entries);
         setup_temp_free(f, lengths, c->entries); // note this is only safe if there have been no intervening temp mallocs!
         lengths = c->codeword_lengths;
         c->sparse = 0;
      }

      if (c->sparse) {
         sorted_count = total;
      } else {
         sorted_count = 0;
         #ifndef STB_VORBIS_NO_HUFFMAN_BINARY_SEARCH
         for (j=0; j < c->entries; ++j)
            if (lengths[j] > STB_VORBIS_FAST_HUFFMAN_LENGTH && lengths[j] != NO_CODE)
               ++sorted_count;
         #endif
      }

      c->sorted_entries = sorted_count;
      values = NULL;

      CHECK(f);
      if (!c->sparse) {
         c->codewords = (uint32 *) setup_malloc(f, sizeof(c->codewords[0]) * c->entries);
         if (!c->codewords)                  return error(f, VORBIS_outofmem);
      } else {
         unsigned int size;
         if (c->sorted_entries) {
            c->codeword_lengths = (uint8 *) setup_malloc(f, c->sorted_entries);
            if (!c->codeword_lengths)           return error(f, VORBIS_outofmem);
            c->codewords = (uint32 *) setup_temp_malloc(f, sizeof(*c->codewords) * c->sorted_entries);
            if (!c->codewords)                  return error(f, VORBIS_outofmem);
            values = (uint32 *) setup_temp_malloc(f, sizeof(*values) * c->sorted_entries);
            if (!values)                        return error(f, VORBIS_outofmem);
         }
         size = c->entries + (sizeof(*c->codewords) + sizeof(*values)) * c->sorted_entries;
         if (size > f->setup_temp_memory_required)
            f->setup_temp_memory_required = size;
      }

      if (!compute_codewords(c, lengths, c->entries, values)) {
         if (c->sparse) setup_temp_free(f, values, 0);
         return error(f, VORBIS_invalid_setup);
      }

      if (c->sorted_entries) {
         c->sorted_codewords = (uint32 *) setup_malloc(f, sizeof(*c->sorted_codewords) * (c->sorted_entries+1));
         if (c->sorted_codewords == NULL) return error(f, VORBIS_outofmem);
         c->sorted_values    = ( int   *) setup_malloc(f, sizeof(*c->sorted_values   ) * (c->sorted_entries+1));
         if (c->sorted_values == NULL) return error(f, VORBIS_outofmem);
         ++c->sorted_values;
         c->sorted_values[-1] = -1;
         compute_sorted_huffman(c, lengths, values);
      }

      if (c->sparse) {
         setup_temp_free(f, values, sizeof(*values)*c->sorted_entries);
         setup_temp_free(f, c->codewords, sizeof(*c->codewords)*c->sorted_entries);
         setup_temp_free(f, lengths, c->entries);
         c->codewords = NULL;
      }

      compute_accelerated_huffman(c);

      CHECK(f);
      c->lookup_type = get_bits(f, 4);
      if (c->lookup_type > 2) return error(f, VORBIS_invalid_setup);
      if (c->lookup_type > 0) {
         uint16 *mults;
         c->minimum_value = float32_unpack(get_bits(f, 32));
         c->delta_value = float32_unpack(get_bits(f, 32));
          c->value_bits = get_bits(f, 4)+1;
          c->sequence_p = get_bits(f,1);
          if (c->lookup_type == 1) {
            int values = lookup1_values(c->entries, c->dimensions);
            if (values < 0) return error(f, VORBIS_invalid_setup);
            c->lookup_values = (uint32) values;
          } else {
             c->lookup_values = c->entries * c->dimensions;
          }
         if (c->lookup_values == 0) return error(f, VORBIS_invalid_setup);
         mults = (uint16 *) setup_temp_malloc(f, sizeof(mults[0]) * c->lookup_values);
         if (mults == NULL) return error(f, VORBIS_outofmem);
         for (j=0; j < (int) c->lookup_values; ++j) {
            int q = get_bits(f, c->value_bits);
            if (q == EOP) { setup_temp_free(f,mults,sizeof(mults[0])*c->lookup_values); return error(f, VORBIS_invalid_setup); }
            mults[j] = q;
         }

#ifndef STB_VORBIS_DIVIDES_IN_CODEBOOK
         if (c->lookup_type == 1) {
            int len, sparse = c->sparse;
            float last=0;
            if (sparse) {
               if (c->sorted_entries == 0) goto skip;
               c->multiplicands = (codetype *) setup_malloc(f, sizeof(c->multiplicands[0]) * c->sorted_entries * c->dimensions);
            } else
               c->multiplicands = (codetype *) setup_malloc(f, sizeof(c->multiplicands[0]) * c->entries        * c->dimensions);
            if (c->multiplicands == NULL) { setup_temp_free(f,mults,sizeof(mults[0])*c->lookup_values); return error(f, VORBIS_outofmem); }
            len = sparse ? c->sorted_entries : c->entries;
            for (j=0; j < len; ++j) {
               unsigned int z = sparse ? c->sorted_values[j] : j;
               unsigned int div=1;
               for (k=0; k < c->dimensions; ++k) {
                  int off = (z / div) % c->lookup_values;
                  float val = mults[off];
                  val = mults[off]*c->delta_value + c->minimum_value + last;
                  c->multiplicands[j*c->dimensions + k] = val;
                  if (c->sequence_p)
                     last = val;
                  if (k+1 < c->dimensions) {
                     if (div > UINT_MAX / (unsigned int) c->lookup_values) {
                        setup_temp_free(f, mults,sizeof(mults[0])*c->lookup_values);
                        return error(f, VORBIS_invalid_setup);
                     }
                     div *= c->lookup_values;
                  }
               }
            }
            c->lookup_type = 2;
         }
         else
#endif
         {
            float last=0;
            CHECK(f);
            c->multiplicands = (codetype *) setup_malloc(f, sizeof(c->multiplicands[0]) * c->lookup_values);
            if (c->multiplicands == NULL) { setup_temp_free(f, mults,sizeof(mults[0])*c->lookup_values); return error(f, VORBIS_outofmem); }
            for (j=0; j < (int) c->lookup_values; ++j) {
               float val = mults[j] * c->delta_value + c->minimum_value + last;
               c->multiplicands[j] = val;
               if (c->sequence_p)
                  last = val;
            }
         }
#ifndef STB_VORBIS_DIVIDES_IN_CODEBOOK
        skip:;
#endif
         setup_temp_free(f, mults, sizeof(mults[0])*c->lookup_values);

         CHECK(f);
      }
      CHECK(f);
   }


   x = get_bits(f, 6) + 1;
   for (i=0; i < x; ++i) {
      uint32 z = get_bits(f, 16);
      if (z != 0) return error(f, VORBIS_invalid_setup);
   }

   f->floor_count = get_bits(f, 6)+1;
   f->floor_config = (Floor *)  setup_malloc(f, f->floor_count * sizeof(*f->floor_config));
   if (f->floor_config == NULL) return error(f, VORBIS_outofmem);
   for (i=0; i < f->floor_count; ++i) {
      f->floor_types[i] = get_bits(f, 16);
      if (f->floor_types[i] > 1) return error(f, VORBIS_invalid_setup);
      if (f->floor_types[i] == 0) {
         Floor0 *g = &f->floor_config[i].floor0;
         g->order = get_bits(f,8);
         g->rate = get_bits(f,16);
         g->bark_map_size = get_bits(f,16);
         g->amplitude_bits = get_bits(f,6);
         g->amplitude_offset = get_bits(f,8);
         g->number_of_books = get_bits(f,4) + 1;
         for (j=0; j < g->number_of_books; ++j)
            g->book_list[j] = get_bits(f,8);
         return error(f, VORBIS_feature_not_supported);
      } else {
         stbv__floor_ordering p[31*8+2];
         Floor1 *g = &f->floor_config[i].floor1;
         int max_class = -1; 
         g->partitions = get_bits(f, 5);
         for (j=0; j < g->partitions; ++j) {
            g->partition_class_list[j] = get_bits(f, 4);
            if (g->partition_class_list[j] > max_class)
               max_class = g->partition_class_list[j];
         }
         for (j=0; j <= max_class; ++j) {
            g->class_dimensions[j] = get_bits(f, 3)+1;
            g->class_subclasses[j] = get_bits(f, 2);
            if (g->class_subclasses[j]) {
               g->class_masterbooks[j] = get_bits(f, 8);
               if (g->class_masterbooks[j] >= f->codebook_count) return error(f, VORBIS_invalid_setup);
            }
            for (k=0; k < 1 << g->class_subclasses[j]; ++k) {
               g->subclass_books[j][k] = get_bits(f,8)-1;
               if (g->subclass_books[j][k] >= f->codebook_count) return error(f, VORBIS_invalid_setup);
            }
         }
         g->floor1_multiplier = get_bits(f,2)+1;
         g->rangebits = get_bits(f,4);
         g->Xlist[0] = 0;
         g->Xlist[1] = 1 << g->rangebits;
         g->values = 2;
         for (j=0; j < g->partitions; ++j) {
            int c = g->partition_class_list[j];
            for (k=0; k < g->class_dimensions[c]; ++k) {
               g->Xlist[g->values] = get_bits(f, g->rangebits);
               ++g->values;
            }
         }
         for (j=0; j < g->values; ++j) {
            p[j].x = g->Xlist[j];
             p[j].id = j;
          }
          qsort(p, g->values, sizeof(p[0]), point_compare);
         for (j=0; j < g->values-1; ++j)
            if (p[j].x == p[j+1].x)
               return error(f, VORBIS_invalid_setup);
          for (j=0; j < g->values; ++j)
             g->sorted_order[j] = (uint8) p[j].id;
         for (j=2; j < g->values; ++j) {
            int low,hi;
            neighbors(g->Xlist, j, &low,&hi);
            g->neighbors[j][0] = low;
            g->neighbors[j][1] = hi;
         }

         if (g->values > longest_floorlist)
            longest_floorlist = g->values;
      }
   }

   f->residue_count = get_bits(f, 6)+1;
   f->residue_config = (Residue *) setup_malloc(f, f->residue_count * sizeof(f->residue_config[0]));
   if (f->residue_config == NULL) return error(f, VORBIS_outofmem);
   memset(f->residue_config, 0, f->residue_count * sizeof(f->residue_config[0]));
   for (i=0; i < f->residue_count; ++i) {
      uint8 residue_cascade[64];
      Residue *r = f->residue_config+i;
      f->residue_types[i] = get_bits(f, 16);
      if (f->residue_types[i] > 2) return error(f, VORBIS_invalid_setup);
      r->begin = get_bits(f, 24);
      r->end = get_bits(f, 24);
      if (r->end < r->begin) return error(f, VORBIS_invalid_setup);
      r->part_size = get_bits(f,24)+1;
      r->classifications = get_bits(f,6)+1;
      r->classbook = get_bits(f,8);
      if (r->classbook >= f->codebook_count) return error(f, VORBIS_invalid_setup);
      for (j=0; j < r->classifications; ++j) {
         uint8 high_bits=0;
         uint8 low_bits=get_bits(f,3);
         if (get_bits(f,1))
            high_bits = get_bits(f,5);
         residue_cascade[j] = high_bits*8 + low_bits;
      }
      r->residue_books = (short (*)[8]) setup_malloc(f, sizeof(r->residue_books[0]) * r->classifications);
      if (r->residue_books == NULL) return error(f, VORBIS_outofmem);
      for (j=0; j < r->classifications; ++j) {
         for (k=0; k < 8; ++k) {
            if (residue_cascade[j] & (1 << k)) {
               r->residue_books[j][k] = get_bits(f, 8);
               if (r->residue_books[j][k] >= f->codebook_count) return error(f, VORBIS_invalid_setup);
            } else {
               r->residue_books[j][k] = -1;
            }
         }
      }
      r->classdata = (uint8 **) setup_malloc(f, sizeof(*r->classdata) * f->codebooks[r->classbook].entries);
      if (!r->classdata) return error(f, VORBIS_outofmem);
      memset(r->classdata, 0, sizeof(*r->classdata) * f->codebooks[r->classbook].entries);
      for (j=0; j < f->codebooks[r->classbook].entries; ++j) {
         int classwords = f->codebooks[r->classbook].dimensions;
         int temp = j;
         r->classdata[j] = (uint8 *) setup_malloc(f, sizeof(r->classdata[j][0]) * classwords);
         if (r->classdata[j] == NULL) return error(f, VORBIS_outofmem);
         for (k=classwords-1; k >= 0; --k) {
            r->classdata[j][k] = temp % r->classifications;
            temp /= r->classifications;
         }
      }
   }

   f->mapping_count = get_bits(f,6)+1;
   f->mapping = (Mapping *) setup_malloc(f, f->mapping_count * sizeof(*f->mapping));
   if (f->mapping == NULL) return error(f, VORBIS_outofmem);
   memset(f->mapping, 0, f->mapping_count * sizeof(*f->mapping));
   for (i=0; i < f->mapping_count; ++i) {
      Mapping *m = f->mapping + i;      
      int mapping_type = get_bits(f,16);
      if (mapping_type != 0) return error(f, VORBIS_invalid_setup);
      m->chan = (MappingChannel *) setup_malloc(f, f->channels * sizeof(*m->chan));
      if (m->chan == NULL) return error(f, VORBIS_outofmem);
      if (get_bits(f,1))
         m->submaps = get_bits(f,4)+1;
      else
         m->submaps = 1;
      if (m->submaps > max_submaps)
          max_submaps = m->submaps;
       if (get_bits(f,1)) {
          m->coupling_steps = get_bits(f,8)+1;
         if (m->coupling_steps > f->channels) return error(f, VORBIS_invalid_setup);
          for (k=0; k < m->coupling_steps; ++k) {
             m->chan[k].magnitude = get_bits(f, ilog(f->channels-1));
             m->chan[k].angle = get_bits(f, ilog(f->channels-1));
            if (m->chan[k].magnitude >= f->channels)        return error(f, VORBIS_invalid_setup);
            if (m->chan[k].angle     >= f->channels)        return error(f, VORBIS_invalid_setup);
            if (m->chan[k].magnitude == m->chan[k].angle)   return error(f, VORBIS_invalid_setup);
         }
      } else
         m->coupling_steps = 0;

      if (get_bits(f,2)) return error(f, VORBIS_invalid_setup);
      if (m->submaps > 1) {
         for (j=0; j < f->channels; ++j) {
            m->chan[j].mux = get_bits(f, 4);
            if (m->chan[j].mux >= m->submaps)                return error(f, VORBIS_invalid_setup);
         }
      } else
         for (j=0; j < f->channels; ++j)
            m->chan[j].mux = 0;

      for (j=0; j < m->submaps; ++j) {
         get_bits(f,8); // discard
         m->submap_floor[j] = get_bits(f,8);
         m->submap_residue[j] = get_bits(f,8);
         if (m->submap_floor[j] >= f->floor_count)      return error(f, VORBIS_invalid_setup);
         if (m->submap_residue[j] >= f->residue_count)  return error(f, VORBIS_invalid_setup);
      }
   }

   f->mode_count = get_bits(f, 6)+1;
   for (i=0; i < f->mode_count; ++i) {
      Mode *m = f->mode_config+i;
      m->blockflag = get_bits(f,1);
      m->windowtype = get_bits(f,16);
      m->transformtype = get_bits(f,16);
      m->mapping = get_bits(f,8);
      if (m->windowtype != 0)                 return error(f, VORBIS_invalid_setup);
      if (m->transformtype != 0)              return error(f, VORBIS_invalid_setup);
      if (m->mapping >= f->mapping_count)     return error(f, VORBIS_invalid_setup);
   }

   flush_packet(f);

   f->previous_length = 0;

   for (i=0; i < f->channels; ++i) {
      f->channel_buffers[i] = (float *) setup_malloc(f, sizeof(float) * f->blocksize_1);
      f->previous_window[i] = (float *) setup_malloc(f, sizeof(float) * f->blocksize_1/2);
      f->finalY[i]          = (int16 *) setup_malloc(f, sizeof(int16) * longest_floorlist);
      if (f->channel_buffers[i] == NULL || f->previous_window[i] == NULL || f->finalY[i] == NULL) return error(f, VORBIS_outofmem);
      memset(f->channel_buffers[i], 0, sizeof(float) * f->blocksize_1);
      #ifdef STB_VORBIS_NO_DEFER_FLOOR
      f->floor_buffers[i]   = (float *) setup_malloc(f, sizeof(float) * f->blocksize_1/2);
      if (f->floor_buffers[i] == NULL) return error(f, VORBIS_outofmem);
      #endif
   }

   if (!init_blocksize(f, 0, f->blocksize_0)) return FALSE;
   if (!init_blocksize(f, 1, f->blocksize_1)) return FALSE;
   f->blocksize[0] = f->blocksize_0;
   f->blocksize[1] = f->blocksize_1;

#ifdef STB_VORBIS_DIVIDE_TABLE
   if (integer_divide_table[1][1]==0)
      for (i=0; i < DIVTAB_NUMER; ++i)
         for (j=1; j < DIVTAB_DENOM; ++j)
            integer_divide_table[i][j] = i / j;
#endif


   {
      uint32 imdct_mem = (f->blocksize_1 * sizeof(float) >> 1);
      uint32 classify_mem;
      int i,max_part_read=0;
      for (i=0; i < f->residue_count; ++i) {
         Residue *r = f->residue_config + i;
         unsigned int actual_size = f->blocksize_1 / 2;
         unsigned int limit_r_begin = r->begin < actual_size ? r->begin : actual_size;
         unsigned int limit_r_end   = r->end   < actual_size ? r->end   : actual_size;
         int n_read = limit_r_end - limit_r_begin;
         int part_read = n_read / r->part_size;
         if (part_read > max_part_read)
            max_part_read = part_read;
      }
      #ifndef STB_VORBIS_DIVIDES_IN_RESIDUE
      classify_mem = f->channels * (sizeof(void*) + max_part_read * sizeof(uint8 *));
      #else
      classify_mem = f->channels * (sizeof(void*) + max_part_read * sizeof(int *));
      #endif


      f->temp_memory_required = classify_mem;
      if (imdct_mem > f->temp_memory_required)
         f->temp_memory_required = imdct_mem;
   }

   f->first_decode = TRUE;

   if (f->alloc.alloc_buffer) {
      assert(f->temp_offset == f->alloc.alloc_buffer_length_in_bytes);
      if (f->setup_offset + sizeof(*f) + f->temp_memory_required > (unsigned) f->temp_offset)
         return error(f, VORBIS_outofmem);
   }

   f->first_audio_page_offset = stb_vorbis_get_file_offset(f);

   return TRUE;
}
