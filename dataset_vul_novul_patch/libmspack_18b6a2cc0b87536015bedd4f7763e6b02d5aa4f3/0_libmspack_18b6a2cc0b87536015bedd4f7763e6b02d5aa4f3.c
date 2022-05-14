int lzxd_decompress(struct lzxd_stream *lzx, off_t out_bytes) {
  /* bitstream and huffman reading variables */
  register unsigned int bit_buffer;
  register int bits_left, i=0;
  unsigned char *i_ptr, *i_end;
  register unsigned short sym;

  int match_length, length_footer, extra, verbatim_bits, bytes_todo;
  int this_run, main_element, aligned_bits, j;
  unsigned char *window, *runsrc, *rundest, buf[12];
  unsigned int frame_size=0, end_frame, match_offset, window_posn;
  unsigned int R0, R1, R2;

  /* easy answers */
  if (!lzx || (out_bytes < 0)) return MSPACK_ERR_ARGS;
  if (lzx->error) return lzx->error;

  /* flush out any stored-up bytes before we begin */
  i = lzx->o_end - lzx->o_ptr;
  if ((off_t) i > out_bytes) i = (int) out_bytes;
  if (i) {
    if (lzx->sys->write(lzx->output, lzx->o_ptr, i) != i) {
      return lzx->error = MSPACK_ERR_WRITE;
    }
    lzx->o_ptr  += i;
    lzx->offset += i;
    out_bytes   -= i;
  }
  if (out_bytes == 0) return MSPACK_ERR_OK;

  /* restore local state */
  RESTORE_BITS;
  window = lzx->window;
  window_posn = lzx->window_posn;
  R0 = lzx->R0;
  R1 = lzx->R1;
  R2 = lzx->R2;

  end_frame = (unsigned int)((lzx->offset + out_bytes) / LZX_FRAME_SIZE) + 1;

  while (lzx->frame < end_frame) {
    /* have we reached the reset interval? (if there is one?) */
    if (lzx->reset_interval && ((lzx->frame % lzx->reset_interval) == 0)) {
      if (lzx->block_remaining) {
	D(("%d bytes remaining at reset interval", lzx->block_remaining))
	return lzx->error = MSPACK_ERR_DECRUNCH;
      }

      /* re-read the intel header and reset the huffman lengths */
      lzxd_reset_state(lzx);
      R0 = lzx->R0;
      R1 = lzx->R1;
      R2 = lzx->R2;
    }

    /* LZX DELTA format has chunk_size, not present in LZX format */
    if (lzx->is_delta) {
      ENSURE_BITS(16);
      REMOVE_BITS(16);
    }

    /* read header if necessary */
    if (!lzx->header_read) {
      /* read 1 bit. if bit=0, intel filesize = 0.
       * if bit=1, read intel filesize (32 bits) */
      j = 0; READ_BITS(i, 1); if (i) { READ_BITS(i, 16); READ_BITS(j, 16); }
      lzx->intel_filesize = (i << 16) | j;
      lzx->header_read = 1;
    } 

    /* calculate size of frame: all frames are 32k except the final frame
     * which is 32kb or less. this can only be calculated when lzx->length
     * has been filled in. */
    frame_size = LZX_FRAME_SIZE;
    if (lzx->length && (lzx->length - lzx->offset) < (off_t)frame_size) {
      frame_size = lzx->length - lzx->offset;
    }

    /* decode until one more frame is available */
    bytes_todo = lzx->frame_posn + frame_size - window_posn;
    while (bytes_todo > 0) {
      /* initialise new block, if one is needed */
      if (lzx->block_remaining == 0) {
	/* realign if previous block was an odd-sized UNCOMPRESSED block */
	if ((lzx->block_type == LZX_BLOCKTYPE_UNCOMPRESSED) &&
	    (lzx->block_length & 1))
	{
	  READ_IF_NEEDED;
	  i_ptr++;
	}

	/* read block type (3 bits) and block length (24 bits) */
	READ_BITS(lzx->block_type, 3);
	READ_BITS(i, 16); READ_BITS(j, 8);
	lzx->block_remaining = lzx->block_length = (i << 8) | j;
	/*D(("new block t%d len %u", lzx->block_type, lzx->block_length))*/

	/* read individual block headers */
	switch (lzx->block_type) {
	case LZX_BLOCKTYPE_ALIGNED:
	  /* read lengths of and build aligned huffman decoding tree */
	  for (i = 0; i < 8; i++) { READ_BITS(j, 3); lzx->ALIGNED_len[i] = j; }
	  BUILD_TABLE(ALIGNED);
	  /* no break -- rest of aligned header is same as verbatim */
	case LZX_BLOCKTYPE_VERBATIM:
	  /* read lengths of and build main huffman decoding tree */
	  READ_LENGTHS(MAINTREE, 0, 256);
	  READ_LENGTHS(MAINTREE, 256, LZX_NUM_CHARS + lzx->num_offsets);
	  BUILD_TABLE(MAINTREE);
	  /* if the literal 0xE8 is anywhere in the block... */
	  if (lzx->MAINTREE_len[0xE8] != 0) lzx->intel_started = 1;
	  /* read lengths of and build lengths huffman decoding tree */
	  READ_LENGTHS(LENGTH, 0, LZX_NUM_SECONDARY_LENGTHS);
	  BUILD_TABLE_MAYBE_EMPTY(LENGTH);
	  break;

	case LZX_BLOCKTYPE_UNCOMPRESSED:
	  /* because we can't assume otherwise */
 	  lzx->intel_started = 1;
 
 	  /* read 1-16 (not 0-15) bits to align to bytes */
	  if (bits_left == 0) ENSURE_BITS(16);
 	  bits_left = 0; bit_buffer = 0;
 
 	  /* read 12 bytes of stored R0 / R1 / R2 values */
	  for (rundest = &buf[0], i = 0; i < 12; i++) {
	    READ_IF_NEEDED;
	    *rundest++ = *i_ptr++;
	  }
	  R0 = buf[0] | (buf[1] << 8) | (buf[2]  << 16) | (buf[3]  << 24);
	  R1 = buf[4] | (buf[5] << 8) | (buf[6]  << 16) | (buf[7]  << 24);
	  R2 = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
	  break;

	default:
	  D(("bad block type"))
	  return lzx->error = MSPACK_ERR_DECRUNCH;
	}
      }

      /* decode more of the block:
       * run = min(what's available, what's needed) */
      this_run = lzx->block_remaining;
      if (this_run > bytes_todo) this_run = bytes_todo;

      /* assume we decode exactly this_run bytes, for now */
      bytes_todo           -= this_run;
      lzx->block_remaining -= this_run;

      /* decode at least this_run bytes */
      switch (lzx->block_type) {
      case LZX_BLOCKTYPE_VERBATIM:
	while (this_run > 0) {
	  READ_HUFFSYM(MAINTREE, main_element);
	  if (main_element < LZX_NUM_CHARS) {
	    /* literal: 0 to LZX_NUM_CHARS-1 */
	    window[window_posn++] = main_element;
	    this_run--;
	  }
	  else {
	    /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
	    main_element -= LZX_NUM_CHARS;

	    /* get match length */
	    match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
	    if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
	      if (lzx->LENGTH_empty) {
                D(("LENGTH symbol needed but tree is empty"))
                return lzx->error = MSPACK_ERR_DECRUNCH;
              }
	      READ_HUFFSYM(LENGTH, length_footer);
	      match_length += length_footer;
	    }
	    match_length += LZX_MIN_MATCH;

	    /* get match offset */
	    switch ((match_offset = (main_element >> 3))) {
	    case 0: match_offset = R0;                                  break;
	    case 1: match_offset = R1; R1=R0;        R0 = match_offset; break;
	    case 2: match_offset = R2; R2=R0;        R0 = match_offset; break;
	    case 3: match_offset = 1;  R2=R1; R1=R0; R0 = match_offset; break;
	    default:
	      extra = (match_offset >= 36) ? 17 : extra_bits[match_offset];
	      READ_BITS(verbatim_bits, extra);
	      match_offset = position_base[match_offset] - 2 + verbatim_bits;
	      R2 = R1; R1 = R0; R0 = match_offset;
	    }

	    /* LZX DELTA uses max match length to signal even longer match */
	    if (match_length == LZX_MAX_MATCH && lzx->is_delta) {
		int extra_len = 0;
		ENSURE_BITS(3); /* 4 entry huffman tree */
		if (PEEK_BITS(1) == 0) {
		    REMOVE_BITS(1); /* '0' -> 8 extra length bits */
		    READ_BITS(extra_len, 8);
		}
		else if (PEEK_BITS(2) == 2) {
		    REMOVE_BITS(2); /* '10' -> 10 extra length bits + 0x100 */
		    READ_BITS(extra_len, 10);
		    extra_len += 0x100;
		}
		else if (PEEK_BITS(3) == 6) {
		    REMOVE_BITS(3); /* '110' -> 12 extra length bits + 0x500 */
		    READ_BITS(extra_len, 12);
		    extra_len += 0x500;
		}
		else {
		    REMOVE_BITS(3); /* '111' -> 15 extra length bits */
		    READ_BITS(extra_len, 15);
		}
		match_length += extra_len;
	    }

	    if ((window_posn + match_length) > lzx->window_size) {
	      D(("match ran over window wrap"))
	      return lzx->error = MSPACK_ERR_DECRUNCH;
	    }
	    
	    /* copy match */
	    rundest = &window[window_posn];
	    i = match_length;
	    /* does match offset wrap the window? */
	    if (match_offset > window_posn) {
	      if (match_offset > lzx->offset &&
		  (match_offset - window_posn) > lzx->ref_data_size)
	      {
		D(("match offset beyond LZX stream"))
		return lzx->error = MSPACK_ERR_DECRUNCH;
	      }
	      /* j = length from match offset to end of window */
	      j = match_offset - window_posn;
	      if (j > (int) lzx->window_size) {
		D(("match offset beyond window boundaries"))
		return lzx->error = MSPACK_ERR_DECRUNCH;
	      }
	      runsrc = &window[lzx->window_size - j];
	      if (j < i) {
		/* if match goes over the window edge, do two copy runs */
		i -= j; while (j-- > 0) *rundest++ = *runsrc++;
		runsrc = window;
	      }
	      while (i-- > 0) *rundest++ = *runsrc++;
	    }
	    else {
	      runsrc = rundest - match_offset;
	      while (i-- > 0) *rundest++ = *runsrc++;
	    }

	    this_run    -= match_length;
	    window_posn += match_length;
	  }
	} /* while (this_run > 0) */
	break;

      case LZX_BLOCKTYPE_ALIGNED:
	while (this_run > 0) {
	  READ_HUFFSYM(MAINTREE, main_element);
	  if (main_element < LZX_NUM_CHARS) {
	    /* literal: 0 to LZX_NUM_CHARS-1 */
	    window[window_posn++] = main_element;
	    this_run--;
	  }
	  else {
	    /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
	    main_element -= LZX_NUM_CHARS;

	    /* get match length */
	    match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
	    if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
              if (lzx->LENGTH_empty) {
                D(("LENGTH symbol needed but tree is empty"))
                return lzx->error = MSPACK_ERR_DECRUNCH;
              } 
	      READ_HUFFSYM(LENGTH, length_footer);
	      match_length += length_footer;
	    }
	    match_length += LZX_MIN_MATCH;

	    /* get match offset */
	    switch ((match_offset = (main_element >> 3))) {
	    case 0: match_offset = R0;                             break;
	    case 1: match_offset = R1; R1 = R0; R0 = match_offset; break;
	    case 2: match_offset = R2; R2 = R0; R0 = match_offset; break;
	    default:
	      extra = (match_offset >= 36) ? 17 : extra_bits[match_offset];
	      match_offset = position_base[match_offset] - 2;
	      if (extra > 3) {
		/* verbatim and aligned bits */
		extra -= 3;
		READ_BITS(verbatim_bits, extra);
		match_offset += (verbatim_bits << 3);
		READ_HUFFSYM(ALIGNED, aligned_bits);
		match_offset += aligned_bits;
	      }
	      else if (extra == 3) {
		/* aligned bits only */
		READ_HUFFSYM(ALIGNED, aligned_bits);
		match_offset += aligned_bits;
	      }
	      else if (extra > 0) { /* extra==1, extra==2 */
		/* verbatim bits only */
		READ_BITS(verbatim_bits, extra);
		match_offset += verbatim_bits;
	      }
	      else /* extra == 0 */ {
		/* ??? not defined in LZX specification! */
		match_offset = 1;
	      }
	      /* update repeated offset LRU queue */
	      R2 = R1; R1 = R0; R0 = match_offset;
	    }

	    /* LZX DELTA uses max match length to signal even longer match */
	    if (match_length == LZX_MAX_MATCH && lzx->is_delta) {
		int extra_len = 0;
		ENSURE_BITS(3); /* 4 entry huffman tree */
		if (PEEK_BITS(1) == 0) {
		    REMOVE_BITS(1); /* '0' -> 8 extra length bits */
		    READ_BITS(extra_len, 8);
		}
		else if (PEEK_BITS(2) == 2) {
		    REMOVE_BITS(2); /* '10' -> 10 extra length bits + 0x100 */
		    READ_BITS(extra_len, 10);
		    extra_len += 0x100;
		}
		else if (PEEK_BITS(3) == 6) {
		    REMOVE_BITS(3); /* '110' -> 12 extra length bits + 0x500 */
		    READ_BITS(extra_len, 12);
		    extra_len += 0x500;
		}
		else {
		    REMOVE_BITS(3); /* '111' -> 15 extra length bits */
		    READ_BITS(extra_len, 15);
		}
		match_length += extra_len;
	    }

	    if ((window_posn + match_length) > lzx->window_size) {
	      D(("match ran over window wrap"))
	      return lzx->error = MSPACK_ERR_DECRUNCH;
	    }

	    /* copy match */
	    rundest = &window[window_posn];
	    i = match_length;
	    /* does match offset wrap the window? */
	    if (match_offset > window_posn) {
	      if (match_offset > lzx->offset &&
		  (match_offset - window_posn) > lzx->ref_data_size)
	      {
		D(("match offset beyond LZX stream"))
		return lzx->error = MSPACK_ERR_DECRUNCH;
	      }
	      /* j = length from match offset to end of window */
	      j = match_offset - window_posn;
	      if (j > (int) lzx->window_size) {
		D(("match offset beyond window boundaries"))
		return lzx->error = MSPACK_ERR_DECRUNCH;
	      }
	      runsrc = &window[lzx->window_size - j];
	      if (j < i) {
		/* if match goes over the window edge, do two copy runs */
		i -= j; while (j-- > 0) *rundest++ = *runsrc++;
		runsrc = window;
	      }
	      while (i-- > 0) *rundest++ = *runsrc++;
	    }
	    else {
	      runsrc = rundest - match_offset;
	      while (i-- > 0) *rundest++ = *runsrc++;
	    }

	    this_run    -= match_length;
	    window_posn += match_length;
	  }
	} /* while (this_run > 0) */
	break;

      case LZX_BLOCKTYPE_UNCOMPRESSED:
	/* as this_run is limited not to wrap a frame, this also means it
	 * won't wrap the window (as the window is a multiple of 32k) */
	rundest = &window[window_posn];
	window_posn += this_run;
	while (this_run > 0) {
	  if ((i = i_end - i_ptr) == 0) {
	    READ_IF_NEEDED;
	  }
	  else {
	    if (i > this_run) i = this_run;
	    lzx->sys->copy(i_ptr, rundest, (size_t) i);
	    rundest  += i;
	    i_ptr    += i;
	    this_run -= i;
	  }
	}
	break;

      default:
	return lzx->error = MSPACK_ERR_DECRUNCH; /* might as well */
      }

      /* did the final match overrun our desired this_run length? */
      if (this_run < 0) {
	if ((unsigned int)(-this_run) > lzx->block_remaining) {
	  D(("overrun went past end of block by %d (%d remaining)",
	     -this_run, lzx->block_remaining ))
	  return lzx->error = MSPACK_ERR_DECRUNCH;
	}
	lzx->block_remaining -= -this_run;
      }
    } /* while (bytes_todo > 0) */

    /* streams don't extend over frame boundaries */
    if ((window_posn - lzx->frame_posn) != frame_size) {
      D(("decode beyond output frame limits! %d != %d",
	 window_posn - lzx->frame_posn, frame_size))
      return lzx->error = MSPACK_ERR_DECRUNCH;
    }

    /* re-align input bitstream */
    if (bits_left > 0) ENSURE_BITS(16);
    if (bits_left & 15) REMOVE_BITS(bits_left & 15);

    /* check that we've used all of the previous frame first */
    if (lzx->o_ptr != lzx->o_end) {
      D(("%ld avail bytes, new %d frame",
          (long)(lzx->o_end - lzx->o_ptr), frame_size))
      return lzx->error = MSPACK_ERR_DECRUNCH;
    }

    /* does this intel block _really_ need decoding? */
    if (lzx->intel_started && lzx->intel_filesize &&
	(lzx->frame <= 32768) && (frame_size > 10))
    {
      unsigned char *data    = &lzx->e8_buf[0];
      unsigned char *dataend = &lzx->e8_buf[frame_size - 10];
      signed int curpos      = lzx->intel_curpos;
      signed int filesize    = lzx->intel_filesize;
      signed int abs_off, rel_off;

      /* copy e8 block to the e8 buffer and tweak if needed */
      lzx->o_ptr = data;
      lzx->sys->copy(&lzx->window[lzx->frame_posn], data, frame_size);

      while (data < dataend) {
	if (*data++ != 0xE8) { curpos++; continue; }
	abs_off = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
	if ((abs_off >= -curpos) && (abs_off < filesize)) {
	  rel_off = (abs_off >= 0) ? abs_off - curpos : abs_off + filesize;
	  data[0] = (unsigned char) rel_off;
	  data[1] = (unsigned char) (rel_off >> 8);
	  data[2] = (unsigned char) (rel_off >> 16);
	  data[3] = (unsigned char) (rel_off >> 24);
	}
	data += 4;
	curpos += 5;
      }
      lzx->intel_curpos += frame_size;
    }
    else {
      lzx->o_ptr = &lzx->window[lzx->frame_posn];
      if (lzx->intel_filesize) lzx->intel_curpos += frame_size;
    }
    lzx->o_end = &lzx->o_ptr[frame_size];

    /* write a frame */
    i = (out_bytes < (off_t)frame_size) ? (unsigned int)out_bytes : frame_size;
    if (lzx->sys->write(lzx->output, lzx->o_ptr, i) != i) {
      return lzx->error = MSPACK_ERR_WRITE;
    }
    lzx->o_ptr  += i;
    lzx->offset += i;
    out_bytes   -= i;

    /* advance frame start position */
    lzx->frame_posn += frame_size;
    lzx->frame++;

    /* wrap window / frame position pointers */
    if (window_posn == lzx->window_size)     window_posn = 0;
    if (lzx->frame_posn == lzx->window_size) lzx->frame_posn = 0;

  } /* while (lzx->frame < end_frame) */

  if (out_bytes) {
    D(("bytes left to output"))
    return lzx->error = MSPACK_ERR_DECRUNCH;
  }

  /* store local state */
  STORE_BITS;
  lzx->window_posn = window_posn;
  lzx->R0 = R0;
  lzx->R1 = R1;
  lzx->R2 = R2;

  return MSPACK_ERR_OK;
}
