static BROTLI_INLINE BrotliResult ProcessCommandsInternal(int safe,
    BrotliState* s) {
  int pos = s->pos;
  int i = s->loop_counter;
  BrotliResult result = BROTLI_RESULT_SUCCESS;
  BrotliBitReader* br = &s->br;

  if (!CheckInputAmount(safe, br, 28) || !WarmupBitReader(safe, br)) {
    result = BROTLI_RESULT_NEEDS_MORE_INPUT;
    goto saveStateAndReturn;
  }

  /* Jump into state machine. */
  if (s->state == BROTLI_STATE_COMMAND_BEGIN) {
    goto CommandBegin;
  } else if (s->state == BROTLI_STATE_COMMAND_INNER) {
    goto CommandInner;
  } else if (s->state == BROTLI_STATE_COMMAND_POST_DECODE_LITERALS) {
    goto CommandPostDecodeLiterals;
  } else if (s->state == BROTLI_STATE_COMMAND_POST_WRAP_COPY) {
    goto CommandPostWrapCopy;
  } else {
    return BROTLI_FAILURE();
  }

CommandBegin:
  if (safe) {
    s->state = BROTLI_STATE_COMMAND_BEGIN;
  }
  if (!CheckInputAmount(safe, br, 28)) { /* 156 bits + 7 bytes */
    s->state = BROTLI_STATE_COMMAND_BEGIN;
    result = BROTLI_RESULT_NEEDS_MORE_INPUT;
    goto saveStateAndReturn;
  }
  if (PREDICT_FALSE(s->block_length[1] == 0)) {
    BROTLI_SAFE(DecodeCommandBlockSwitch(s));
    goto CommandBegin;
  }
  /* Read the insert/copy length in the command */
  BROTLI_SAFE(ReadCommand(s, br, &i));
  BROTLI_LOG_UINT(i);
  BROTLI_LOG_UINT(s->copy_length);
  BROTLI_LOG_UINT(s->distance_code);
  if (i == 0) {
    goto CommandPostDecodeLiterals;
  }
  s->meta_block_remaining_len -= i;

CommandInner:
  if (safe) {
    s->state = BROTLI_STATE_COMMAND_INNER;
  }
  /* Read the literals in the command */
  if (s->trivial_literal_context) {
    uint32_t bits;
    uint32_t value;
    PreloadSymbol(safe, s->literal_htree, br, &bits, &value);
    do {
      if (!CheckInputAmount(safe, br, 28)) { /* 162 bits + 7 bytes */
        s->state = BROTLI_STATE_COMMAND_INNER;
        result = BROTLI_RESULT_NEEDS_MORE_INPUT;
        goto saveStateAndReturn;
      }
      if (PREDICT_FALSE(s->block_length[0] == 0)) {
        BROTLI_SAFE(DecodeLiteralBlockSwitch(s));
        PreloadSymbol(safe, s->literal_htree, br, &bits, &value);
      }
      if (!safe) {
        s->ringbuffer[pos] = (uint8_t)ReadPreloadedSymbol(
            s->literal_htree, br, &bits, &value);
      } else {
        uint32_t literal;
        if (!SafeReadSymbol(s->literal_htree, br, &literal)) {
          result = BROTLI_RESULT_NEEDS_MORE_INPUT;
          goto saveStateAndReturn;
        }
        s->ringbuffer[pos] = (uint8_t)literal;
      }
      --s->block_length[0];
      BROTLI_LOG_UINT(s->literal_htree_index);
      BROTLI_LOG_ARRAY_INDEX(s->ringbuffer, pos);
      ++pos;
      if (PREDICT_FALSE(pos == s->ringbuffer_size)) {
        s->state = BROTLI_STATE_COMMAND_INNER_WRITE;
        --i;
        goto saveStateAndReturn;
      }
    } while (--i != 0);
  } else {
    uint8_t p1 = s->ringbuffer[(pos - 1) & s->ringbuffer_mask];
    uint8_t p2 = s->ringbuffer[(pos - 2) & s->ringbuffer_mask];
    do {
      const HuffmanCode* hc;
      uint8_t context;
      if (!CheckInputAmount(safe, br, 28)) { /* 162 bits + 7 bytes */
        s->state = BROTLI_STATE_COMMAND_INNER;
        result = BROTLI_RESULT_NEEDS_MORE_INPUT;
        goto saveStateAndReturn;
      }
      if (PREDICT_FALSE(s->block_length[0] == 0)) {
        BROTLI_SAFE(DecodeLiteralBlockSwitch(s));
      }
      context = s->context_lookup1[p1] | s->context_lookup2[p2];
      BROTLI_LOG_UINT(context);
      hc = s->literal_hgroup.htrees[s->context_map_slice[context]];
      p2 = p1;
      if (!safe) {
        p1 = (uint8_t)ReadSymbol(hc, br);
      } else {
        uint32_t literal;
        if (!SafeReadSymbol(hc, br, &literal)) {
          result = BROTLI_RESULT_NEEDS_MORE_INPUT;
          goto saveStateAndReturn;
        }
        p1 = (uint8_t)literal;
      }
      s->ringbuffer[pos] = p1;
      --s->block_length[0];
      BROTLI_LOG_UINT(s->context_map_slice[context]);
      BROTLI_LOG_ARRAY_INDEX(s->ringbuffer, pos & s->ringbuffer_mask);
      ++pos;
      if (PREDICT_FALSE(pos == s->ringbuffer_size)) {
        s->state = BROTLI_STATE_COMMAND_INNER_WRITE;
        --i;
        goto saveStateAndReturn;
      }
    } while (--i != 0);
  }
  if (s->meta_block_remaining_len <= 0) {
    s->state = BROTLI_STATE_METABLOCK_DONE;
    goto saveStateAndReturn;
  }

CommandPostDecodeLiterals:
  if (safe) {
    s->state = BROTLI_STATE_COMMAND_POST_DECODE_LITERALS;
  }
  if (s->distance_code >= 0) {
    --s->dist_rb_idx;
    s->distance_code = s->dist_rb[s->dist_rb_idx & 3];
    goto postReadDistance;  /* We already have the implicit distance */
  }
  /* Read distance code in the command, unless it was implicitly zero. */
  if (PREDICT_FALSE(s->block_length[2] == 0)) {
    BROTLI_SAFE(DecodeDistanceBlockSwitch(s));
  }
  BROTLI_SAFE(ReadDistance(s, br));
postReadDistance:
  BROTLI_LOG_UINT(s->distance_code);
  if (s->max_distance != s->max_backward_distance) {
    if (pos < s->max_backward_distance_minus_custom_dict_size) {
      s->max_distance = pos + s->custom_dict_size;
    } else {
      s->max_distance = s->max_backward_distance;
    }
  }
  i = s->copy_length;
  /* Apply copy of LZ77 back-reference, or static dictionary reference if
  the distance is larger than the max LZ77 distance */
  if (s->distance_code > s->max_distance) {
    if (i >= kBrotliMinDictionaryWordLength &&
        i <= kBrotliMaxDictionaryWordLength) {
      int offset = kBrotliDictionaryOffsetsByLength[i];
      int word_id = s->distance_code - s->max_distance - 1;
      uint32_t shift = kBrotliDictionarySizeBitsByLength[i];
      int mask = (int)BitMask(shift);
      int word_idx = word_id & mask;
      int transform_idx = word_id >> shift;
      offset += word_idx * i;
      if (transform_idx < kNumTransforms) {
        const uint8_t* word = &kBrotliDictionary[offset];
        int len = i;
        if (transform_idx == 0) {
          memcpy(&s->ringbuffer[pos], word, (size_t)len);
        } else {
          len = TransformDictionaryWord(
              &s->ringbuffer[pos], word, len, transform_idx);
        }
        pos += len;
        s->meta_block_remaining_len -= len;
        if (pos >= s->ringbuffer_size) {
          /*s->partial_pos_rb += (size_t)s->ringbuffer_size;*/
          s->state = BROTLI_STATE_COMMAND_POST_WRITE_1;
          goto saveStateAndReturn;
        }
      } else {
        BROTLI_LOG(("Invalid backward reference. pos: %d distance: %d "
               "len: %d bytes left: %d\n",
            pos, s->distance_code, i,
            s->meta_block_remaining_len));
        return BROTLI_FAILURE();
      }
    } else {
      BROTLI_LOG(("Invalid backward reference. pos: %d distance: %d "
             "len: %d bytes left: %d\n", pos, s->distance_code, i,
             s->meta_block_remaining_len));
      return BROTLI_FAILURE();
    }
  } else {
    const uint8_t *ringbuffer_end_minus_copy_length =
        s->ringbuffer_end - i;
     uint8_t* copy_src = &s->ringbuffer[
         (pos - s->distance_code) & s->ringbuffer_mask];
     uint8_t* copy_dst = &s->ringbuffer[pos];
    /* Check for possible underflow and clamp the pointer to 0. */
    if (PREDICT_FALSE(s->ringbuffer_end < (const uint8_t*)0 + i)) {
      ringbuffer_end_minus_copy_length = 0;
    }
     /* update the recent distances cache */
     s->dist_rb[s->dist_rb_idx & 3] = s->distance_code;
     ++s->dist_rb_idx;
    s->meta_block_remaining_len -= i;
    if (PREDICT_FALSE(s->meta_block_remaining_len < 0)) {
      BROTLI_LOG(("Invalid backward reference. pos: %d distance: %d "
             "len: %d bytes left: %d\n", pos, s->distance_code, i,
             s->meta_block_remaining_len));
      return BROTLI_FAILURE();
    }
    /* There is 128+ bytes of slack in the ringbuffer allocation.
       Also, we have 16 short codes, that make these 16 bytes irrelevant
       in the ringbuffer. Let's copy over them as a first guess.
     */
    memmove16(copy_dst, copy_src);
    /* Now check if the copy extends over the ringbuffer end,
       or if the copy overlaps with itself, if yes, do wrap-copy. */
    if (copy_src < copy_dst) {
      if (copy_dst >= ringbuffer_end_minus_copy_length) {
        goto CommandPostWrapCopy;
      }
      if (copy_src + i > copy_dst) {
        goto postSelfintersecting;
      }
    } else {
      if (copy_src >= ringbuffer_end_minus_copy_length) {
        goto CommandPostWrapCopy;
      }
      if (copy_dst + i > copy_src) {
        goto postSelfintersecting;
      }
    }
    pos += i;
    if (i > 16) {
      if (i > 32) {
        memcpy(copy_dst + 16, copy_src + 16, (size_t)(i - 16));
      } else {
        /* This branch covers about 45% cases.
           Fixed size short copy allows more compiler optimizations. */
        memmove16(copy_dst + 16, copy_src + 16);
      }
    }
  }
  if (s->meta_block_remaining_len <= 0) {
    /* Next metablock, if any */
    s->state = BROTLI_STATE_METABLOCK_DONE;
    goto saveStateAndReturn;
  } else {
    goto CommandBegin;
  }
postSelfintersecting:
  while (--i >= 0) {
    s->ringbuffer[pos] =
        s->ringbuffer[(pos - s->distance_code) & s->ringbuffer_mask];
    ++pos;
  }
  if (s->meta_block_remaining_len <= 0) {
    /* Next metablock, if any */
    s->state = BROTLI_STATE_METABLOCK_DONE;
    goto saveStateAndReturn;
  } else {
    goto CommandBegin;
  }

CommandPostWrapCopy:
  s->state = BROTLI_STATE_COMMAND_POST_WRAP_COPY;
  while (--i >= 0) {
    s->ringbuffer[pos] =
        s->ringbuffer[(pos - s->distance_code) & s->ringbuffer_mask];
    ++pos;
    if (pos == s->ringbuffer_size) {
      /*s->partial_pos_rb += (size_t)s->ringbuffer_size;*/
      s->state = BROTLI_STATE_COMMAND_POST_WRITE_2;
      goto saveStateAndReturn;
    }
  }
  if (s->meta_block_remaining_len <= 0) {
    /* Next metablock, if any */
    s->state = BROTLI_STATE_METABLOCK_DONE;
    goto saveStateAndReturn;
  } else {
    goto CommandBegin;
  }

saveStateAndReturn:
  s->pos = pos;
  s->loop_counter = i;
  return result;
}
