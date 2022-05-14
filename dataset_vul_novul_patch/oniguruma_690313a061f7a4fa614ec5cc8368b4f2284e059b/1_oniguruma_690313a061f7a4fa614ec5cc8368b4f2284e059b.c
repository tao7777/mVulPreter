match_at(regex_t* reg, const UChar* str, const UChar* end,
#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
	 const UChar* right_range,
#endif
	 const UChar* sstart, UChar* sprev, OnigMatchArg* msa)
{
  static UChar FinishCode[] = { OP_FINISH };

  int i, n, num_mem, best_len, pop_level;
  LengthType tlen, tlen2;
  MemNumType mem;
  RelAddrType addr;
  UChar *s, *q, *sbegin;
  int is_alloca;
  char *alloc_base;
  OnigStackType *stk_base, *stk, *stk_end;
  OnigStackType *stkp; /* used as any purpose. */
  OnigStackIndex si;
  OnigStackIndex *repeat_stk;
  OnigStackIndex *mem_start_stk, *mem_end_stk;
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  int scv;
  unsigned char* state_check_buff = msa->state_check_buff;
  int num_comb_exp_check = reg->num_comb_exp_check;
#endif
  UChar *p = reg->p;
  OnigOptionType option = reg->options;
  OnigEncoding encode = reg->enc;
  OnigCaseFoldType case_fold_flag = reg->case_fold_flag;

  pop_level = reg->stack_pop_level;
  num_mem = reg->num_mem;
  STACK_INIT(INIT_MATCH_STACK_SIZE);
  UPDATE_FOR_STACK_REALLOC;
  for (i = 1; i <= num_mem; i++) {
    mem_start_stk[i] = mem_end_stk[i] = INVALID_STACK_INDEX;
  }

#ifdef ONIG_DEBUG_MATCH
  fprintf(stderr, "match_at: str: %d, end: %d, start: %d, sprev: %d\n",
	  (int )str, (int )end, (int )sstart, (int )sprev);
  fprintf(stderr, "size: %d, start offset: %d\n",
	  (int )(end - str), (int )(sstart - str));
#endif

  STACK_PUSH_ENSURED(STK_ALT, FinishCode);  /* bottom stack */
  best_len = ONIG_MISMATCH;
  s = (UChar* )sstart;
  while (1) {
#ifdef ONIG_DEBUG_MATCH
    {
      UChar *q, *bp, buf[50];
      int len;
      fprintf(stderr, "%4d> \"", (int )(s - str));
      bp = buf;
      for (i = 0, q = s; i < 7 && q < end; i++) {
	len = enclen(encode, q);
	while (len-- > 0) *bp++ = *q++;
      }
      if (q < end) { xmemcpy(bp, "...\"", 4); bp += 4; }
      else         { xmemcpy(bp, "\"",    1); bp += 1; }
      *bp = 0;
      fputs((char* )buf, stderr);
      for (i = 0; i < 20 - (bp - buf); i++) fputc(' ', stderr);
      onig_print_compiled_byte_code(stderr, p, NULL, encode);
      fprintf(stderr, "\n");
    }
#endif

    sbegin = s;
    switch (*p++) {
    case OP_END:  MOP_IN(OP_END);
      n = s - sstart;
      if (n > best_len) {
        OnigRegion* region;
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
        if (IS_FIND_LONGEST(option)) {
          if (n > msa->best_len) {
            msa->best_len = n;
            msa->best_s   = (UChar* )sstart;
          }
          else
            goto end_best_len;
        }
#endif
        best_len = n;
        region = msa->region;
        if (region) {
#ifdef USE_POSIX_API_REGION_OPTION
          if (IS_POSIX_REGION(msa->options)) {
            posix_regmatch_t* rmt = (posix_regmatch_t* )region;

            rmt[0].rm_so = sstart - str;
            rmt[0].rm_eo = s      - str;
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                if (BIT_STATUS_AT(reg->bt_mem_start, i))
                  rmt[i].rm_so = STACK_AT(mem_start_stk[i])->u.mem.pstr - str;
                else
                  rmt[i].rm_so = (UChar* )((void* )(mem_start_stk[i])) - str;

                rmt[i].rm_eo = (BIT_STATUS_AT(reg->bt_mem_end, i)
                                ? STACK_AT(mem_end_stk[i])->u.mem.pstr
                                : (UChar* )((void* )mem_end_stk[i])) - str;
              }
              else {
                rmt[i].rm_so = rmt[i].rm_eo = ONIG_REGION_NOTPOS;
              }
            }
          }
          else {
#endif /* USE_POSIX_API_REGION_OPTION */
            region->beg[0] = sstart - str;
            region->end[0] = s      - str;
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                if (BIT_STATUS_AT(reg->bt_mem_start, i))
                  region->beg[i] = STACK_AT(mem_start_stk[i])->u.mem.pstr - str;
                else
                  region->beg[i] = (UChar* )((void* )mem_start_stk[i]) - str;

                region->end[i] = (BIT_STATUS_AT(reg->bt_mem_end, i)
                                  ? STACK_AT(mem_end_stk[i])->u.mem.pstr
                                  : (UChar* )((void* )mem_end_stk[i])) - str;
              }
              else {
                region->beg[i] = region->end[i] = ONIG_REGION_NOTPOS;
              }
            }

#ifdef USE_CAPTURE_HISTORY
            if (reg->capture_history != 0) {
              int r;
              OnigCaptureTreeNode* node;

              if (IS_NULL(region->history_root)) {
                region->history_root = node = history_node_new();
                CHECK_NULL_RETURN_MEMERR(node);
              }
              else {
                node = region->history_root;
                history_tree_clear(node);
              }

              node->group = 0;
              node->beg   = sstart - str;
              node->end   = s      - str;

              stkp = stk_base;
              r = make_capture_history_tree(region->history_root, &stkp,
                                            stk, (UChar* )str, reg);
              if (r < 0) {
                best_len = r; /* error code */
                goto finish;
              }
            }
#endif /* USE_CAPTURE_HISTORY */
#ifdef USE_POSIX_API_REGION_OPTION
          } /* else IS_POSIX_REGION() */
#endif
        } /* if (region) */
      } /* n > best_len */

#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
    end_best_len:
#endif
      MOP_OUT;

      if (IS_FIND_CONDITION(option)) {
        if (IS_FIND_NOT_EMPTY(option) && s == sstart) {
          best_len = ONIG_MISMATCH;
          goto fail; /* for retry */
        }
        if (IS_FIND_LONGEST(option) && DATA_ENSURE_CHECK1) {
          goto fail; /* for retry */
        }
      }

      /* default behavior: return first-matching result. */
      goto finish;
       break;
 
     case OP_EXACT1:  MOP_IN(OP_EXACT1);
#if 0
       DATA_ENSURE(1);
       if (*p != *s) goto fail;
       p++; s++;
#endif
      if (*p != *s++) goto fail;
      DATA_ENSURE(0);
      p++;
       MOP_OUT;
       break;
 
    case OP_EXACT1_IC:  MOP_IN(OP_EXACT1_IC);
      {
        int len;
        UChar *q, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        DATA_ENSURE(1);
        len = ONIGENC_MBC_CASE_FOLD(encode,
                 /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                    case_fold_flag,
                                    &s, end, lowbuf);
        DATA_ENSURE(0);
        q = lowbuf;
        while (len-- > 0) {
          if (*p != *q) {
            goto fail;
          }
          p++; q++;
        }
      }
      MOP_OUT;
      break;

    case OP_EXACT2:  MOP_IN(OP_EXACT2);
      DATA_ENSURE(2);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACT3:  MOP_IN(OP_EXACT3);
      DATA_ENSURE(3);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACT4:  MOP_IN(OP_EXACT4);
      DATA_ENSURE(4);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACT5:  MOP_IN(OP_EXACT5);
      DATA_ENSURE(5);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      sprev = s;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTN:  MOP_IN(OP_EXACTN);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen);
      while (tlen-- > 0) {
        if (*p++ != *s++) goto fail;
      }
      sprev = s - 1;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTN_IC:  MOP_IN(OP_EXACTN_IC);
      {
        int len;
        UChar *q, *endp, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        GET_LENGTH_INC(tlen, p);
        endp = p + tlen;

        while (p < endp) {
          sprev = s;
          DATA_ENSURE(1);
          len = ONIGENC_MBC_CASE_FOLD(encode,
                        /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                      case_fold_flag,
                                      &s, end, lowbuf);
          DATA_ENSURE(0);
          q = lowbuf;
          while (len-- > 0) {
            if (*p != *q) goto fail;
            p++; q++;
          }
        }
      }

      MOP_OUT;
      continue;
      break;

    case OP_EXACTMB2N1:  MOP_IN(OP_EXACTMB2N1);
      DATA_ENSURE(2);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      break;

    case OP_EXACTMB2N2:  MOP_IN(OP_EXACTMB2N2);
      DATA_ENSURE(4);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      sprev = s;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTMB2N3:  MOP_IN(OP_EXACTMB2N3);
      DATA_ENSURE(6);
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      sprev = s;
      if (*p != *s) goto fail;
      p++; s++;
      if (*p != *s) goto fail;
      p++; s++;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTMB2N:  MOP_IN(OP_EXACTMB2N);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen * 2);
      while (tlen-- > 0) {
        if (*p != *s) goto fail;
        p++; s++;
        if (*p != *s) goto fail;
        p++; s++;
      }
      sprev = s - 2;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTMB3N:  MOP_IN(OP_EXACTMB3N);
      GET_LENGTH_INC(tlen, p);
      DATA_ENSURE(tlen * 3);
      while (tlen-- > 0) {
        if (*p != *s) goto fail;
        p++; s++;
        if (*p != *s) goto fail;
        p++; s++;
        if (*p != *s) goto fail;
        p++; s++;
      }
      sprev = s - 3;
      MOP_OUT;
      continue;
      break;

    case OP_EXACTMBN:  MOP_IN(OP_EXACTMBN);
      GET_LENGTH_INC(tlen,  p);  /* mb-len */
      GET_LENGTH_INC(tlen2, p);  /* string len */
      tlen2 *= tlen;
      DATA_ENSURE(tlen2);
      while (tlen2-- > 0) {
        if (*p != *s) goto fail;
        p++; s++;
      }
      sprev = s - tlen;
      MOP_OUT;
      continue;
      break;

    case OP_CCLASS:  MOP_IN(OP_CCLASS);
      DATA_ENSURE(1);
      if (BITSET_AT(((BitSetRef )p), *s) == 0) goto fail;
      p += SIZE_BITSET;
      s += enclen(encode, s);   /* OP_CCLASS can match mb-code. \D, \S */
      MOP_OUT;
      break;

    case OP_CCLASS_MB:  MOP_IN(OP_CCLASS_MB);
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) goto fail;

    cclass_mb:
      GET_LENGTH_INC(tlen, p);
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len;

        DATA_ENSURE(1);
        mb_len = enclen(encode, s);
        DATA_ENSURE(mb_len);
        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
        if (! onig_is_in_code_range(p, code)) goto fail;
#else
        q = p;
        ALIGNMENT_RIGHT(q);
        if (! onig_is_in_code_range(q, code)) goto fail;
#endif
      }
      p += tlen;
      MOP_OUT;
      break;

    case OP_CCLASS_MIX:  MOP_IN(OP_CCLASS_MIX);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        p += SIZE_BITSET;
        goto cclass_mb;
      }
      else {
        if (BITSET_AT(((BitSetRef )p), *s) == 0)
          goto fail;

        p += SIZE_BITSET;
        GET_LENGTH_INC(tlen, p);
        p += tlen;
        s++;
      }
      MOP_OUT;
      break;

    case OP_CCLASS_NOT:  MOP_IN(OP_CCLASS_NOT);
      DATA_ENSURE(1);
      if (BITSET_AT(((BitSetRef )p), *s) != 0) goto fail;
      p += SIZE_BITSET;
      s += enclen(encode, s);
      MOP_OUT;
      break;

    case OP_CCLASS_MB_NOT:  MOP_IN(OP_CCLASS_MB_NOT);
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) {
        s++;
        GET_LENGTH_INC(tlen, p);
        p += tlen;
        goto cc_mb_not_success;
      }

    cclass_mb_not:
      GET_LENGTH_INC(tlen, p);
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len = enclen(encode, s);

        if (! DATA_ENSURE_CHECK(mb_len)) {
          DATA_ENSURE(1);
          s = (UChar* )end;
          p += tlen;
          goto cc_mb_not_success;
        }

        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);

#ifdef PLATFORM_UNALIGNED_WORD_ACCESS
        if (onig_is_in_code_range(p, code)) goto fail;
#else
        q = p;
        ALIGNMENT_RIGHT(q);
        if (onig_is_in_code_range(q, code)) goto fail;
#endif
      }
      p += tlen;

    cc_mb_not_success:
      MOP_OUT;
      break;

    case OP_CCLASS_MIX_NOT:  MOP_IN(OP_CCLASS_MIX_NOT);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        p += SIZE_BITSET;
        goto cclass_mb_not;
      }
      else {
        if (BITSET_AT(((BitSetRef )p), *s) != 0)
          goto fail;

        p += SIZE_BITSET;
        GET_LENGTH_INC(tlen, p);
        p += tlen;
        s++;
      }
      MOP_OUT;
      break;

    case OP_CCLASS_NODE:  MOP_IN(OP_CCLASS_NODE);
      {
        OnigCodePoint code;
        void *node;
        int mb_len;
        UChar *ss;

        DATA_ENSURE(1);
        GET_POINTER_INC(node, p);
        mb_len = enclen(encode, s);
        ss = s;
        s += mb_len;
        DATA_ENSURE(0);
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);
        if (onig_is_code_in_cc_len(mb_len, code, node) == 0) goto fail;
      }
      MOP_OUT;
      break;

    case OP_ANYCHAR:  MOP_IN(OP_ANYCHAR);
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) goto fail;
      s += n;
      MOP_OUT;
      break;

    case OP_ANYCHAR_ML:  MOP_IN(OP_ANYCHAR_ML);
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      s += n;
      MOP_OUT;
      break;

    case OP_ANYCHAR_STAR:  MOP_IN(OP_ANYCHAR_STAR);
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        DATA_ENSURE(n);
        if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
        sprev = s;
        s += n;
      }
      MOP_OUT;
      break;

    case OP_ANYCHAR_ML_STAR:  MOP_IN(OP_ANYCHAR_ML_STAR);
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        if (n > 1) {
          DATA_ENSURE(n);
          sprev = s;
          s += n;
        }
        else {
          sprev = s;
          s++;
        }
      }
      MOP_OUT;
      break;

    case OP_ANYCHAR_STAR_PEEK_NEXT:  MOP_IN(OP_ANYCHAR_STAR_PEEK_NEXT);
      while (DATA_ENSURE_CHECK1) {
        if (*p == *s) {
          STACK_PUSH_ALT(p + 1, s, sprev);
        }
        n = enclen(encode, s);
        DATA_ENSURE(n);
        if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
        sprev = s;
        s += n;
      }
      p++;
      MOP_OUT;
      break;

    case OP_ANYCHAR_ML_STAR_PEEK_NEXT:MOP_IN(OP_ANYCHAR_ML_STAR_PEEK_NEXT);
      while (DATA_ENSURE_CHECK1) {
        if (*p == *s) {
          STACK_PUSH_ALT(p + 1, s, sprev);
        }
        n = enclen(encode, s);
        if (n > 1) {
          DATA_ENSURE(n);
          sprev = s;
          s += n;
        }
        else {
          sprev = s;
          s++;
        }
      }
      p++;
      MOP_OUT;
      break;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
    case OP_STATE_CHECK_ANYCHAR_STAR:  MOP_IN(OP_STATE_CHECK_ANYCHAR_STAR);
      GET_STATE_CHECK_NUM_INC(mem, p);
      while (DATA_ENSURE_CHECK1) {
        STATE_CHECK_VAL(scv, mem);
        if (scv) goto fail;

        STACK_PUSH_ALT_WITH_STATE_CHECK(p, s, sprev, mem);
        n = enclen(encode, s);
        DATA_ENSURE(n);
        if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
        sprev = s;
        s += n;
      }
      MOP_OUT;
      break;

    case OP_STATE_CHECK_ANYCHAR_ML_STAR:
      MOP_IN(OP_STATE_CHECK_ANYCHAR_ML_STAR);

      GET_STATE_CHECK_NUM_INC(mem, p);
      while (DATA_ENSURE_CHECK1) {
        STATE_CHECK_VAL(scv, mem);
        if (scv) goto fail;

        STACK_PUSH_ALT_WITH_STATE_CHECK(p, s, sprev, mem);
        n = enclen(encode, s);
        if (n > 1) {
          DATA_ENSURE(n);
          sprev = s;
          s += n;
        }
        else {
          sprev = s;
          s++;
        }
      }
      MOP_OUT;
      break;
#endif /* USE_COMBINATION_EXPLOSION_CHECK */

    case OP_WORD:  MOP_IN(OP_WORD);
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      MOP_OUT;
      break;

    case OP_NOT_WORD:  MOP_IN(OP_NOT_WORD);
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      MOP_OUT;
      break;

    case OP_WORD_BOUND:  MOP_IN(OP_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
        DATA_ENSURE(1);
        if (! ONIGENC_IS_MBC_WORD(encode, s, end))
          goto fail;
      }
      else if (ON_STR_END(s)) {
        if (! ONIGENC_IS_MBC_WORD(encode, sprev, end))
          goto fail;
      }
      else {
        if (ONIGENC_IS_MBC_WORD(encode, s, end)
            == ONIGENC_IS_MBC_WORD(encode, sprev, end))
          goto fail;
      }
      MOP_OUT;
      continue;
      break;

    case OP_NOT_WORD_BOUND:  MOP_IN(OP_NOT_WORD_BOUND);
      if (ON_STR_BEGIN(s)) {
        if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_WORD(encode, s, end))
          goto fail;
      }
      else if (ON_STR_END(s)) {
        if (ONIGENC_IS_MBC_WORD(encode, sprev, end))
          goto fail;
      }
      else {
        if (ONIGENC_IS_MBC_WORD(encode, s, end)
            != ONIGENC_IS_MBC_WORD(encode, sprev, end))
          goto fail;
      }
      MOP_OUT;
      continue;
      break;

#ifdef USE_WORD_BEGIN_END
    case OP_WORD_BEGIN:  MOP_IN(OP_WORD_BEGIN);
      if (DATA_ENSURE_CHECK1 && ONIGENC_IS_MBC_WORD(encode, s, end)) {
        if (ON_STR_BEGIN(s) || !ONIGENC_IS_MBC_WORD(encode, sprev, end)) {
          MOP_OUT;
          continue;
        }
      }
      goto fail;
      break;

    case OP_WORD_END:  MOP_IN(OP_WORD_END);
      if (!ON_STR_BEGIN(s) && ONIGENC_IS_MBC_WORD(encode, sprev, end)) {
        if (ON_STR_END(s) || !ONIGENC_IS_MBC_WORD(encode, s, end)) {
          MOP_OUT;
          continue;
        }
      }
      goto fail;
      break;
#endif

    case OP_BEGIN_BUF:  MOP_IN(OP_BEGIN_BUF);
      if (! ON_STR_BEGIN(s)) goto fail;

      MOP_OUT;
      continue;
      break;

    case OP_END_BUF:  MOP_IN(OP_END_BUF);
      if (! ON_STR_END(s)) goto fail;

      MOP_OUT;
      continue;
      break;

    case OP_BEGIN_LINE:  MOP_IN(OP_BEGIN_LINE);
      if (ON_STR_BEGIN(s)) {
        if (IS_NOTBOL(msa->options)) goto fail;
        MOP_OUT;
        continue;
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, sprev, end) && !ON_STR_END(s)) {
        MOP_OUT;
        continue;
      }
      goto fail;
      break;

    case OP_END_LINE:  MOP_IN(OP_END_LINE);
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          MOP_OUT;
          continue;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) {
        MOP_OUT;
        continue;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        MOP_OUT;
        continue;
      }
#endif
      goto fail;
      break;

    case OP_SEMI_END_BUF:  MOP_IN(OP_SEMI_END_BUF);
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          MOP_OUT;
          continue;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end) &&
               ON_STR_END(s + enclen(encode, s))) {
        MOP_OUT;
        continue;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        UChar* ss = s + enclen(encode, s);
        ss += enclen(encode, ss);
        if (ON_STR_END(ss)) {
          MOP_OUT;
          continue;
        }
      }
#endif
      goto fail;
      break;

    case OP_BEGIN_POSITION:  MOP_IN(OP_BEGIN_POSITION);
      if (s != msa->start)
        goto fail;

      MOP_OUT;
      continue;
      break;

    case OP_MEMORY_START_PUSH:  MOP_IN(OP_MEMORY_START_PUSH);
      GET_MEMNUM_INC(mem, p);
      STACK_PUSH_MEM_START(mem, s);
      MOP_OUT;
      continue;
      break;

    case OP_MEMORY_START:  MOP_IN(OP_MEMORY_START);
      GET_MEMNUM_INC(mem, p);
      mem_start_stk[mem] = (OnigStackIndex )((void* )s);
      MOP_OUT;
      continue;
      break;

    case OP_MEMORY_END_PUSH:  MOP_IN(OP_MEMORY_END_PUSH);
      GET_MEMNUM_INC(mem, p);
      STACK_PUSH_MEM_END(mem, s);
      MOP_OUT;
      continue;
      break;

    case OP_MEMORY_END:  MOP_IN(OP_MEMORY_END);
      GET_MEMNUM_INC(mem, p);
      mem_end_stk[mem] = (OnigStackIndex )((void* )s);
      MOP_OUT;
      continue;
      break;

#ifdef USE_SUBEXP_CALL
    case OP_MEMORY_END_PUSH_REC:  MOP_IN(OP_MEMORY_END_PUSH_REC);
      GET_MEMNUM_INC(mem, p);
      STACK_GET_MEM_START(mem, stkp); /* should be before push mem-end. */
      STACK_PUSH_MEM_END(mem, s);
      mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      MOP_OUT;
      continue;
      break;

    case OP_MEMORY_END_REC:  MOP_IN(OP_MEMORY_END_REC);
      GET_MEMNUM_INC(mem, p);
      mem_end_stk[mem] = (OnigStackIndex )((void* )s);
      STACK_GET_MEM_START(mem, stkp);

      if (BIT_STATUS_AT(reg->bt_mem_start, mem))
        mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      else
        mem_start_stk[mem] = (OnigStackIndex )((void* )stkp->u.mem.pstr);

      STACK_PUSH_MEM_END_MARK(mem);
      MOP_OUT;
      continue;
      break;
#endif

    case OP_BACKREF1:  MOP_IN(OP_BACKREF1);
      mem = 1;
      goto backref;
      break;

    case OP_BACKREF2:  MOP_IN(OP_BACKREF2);
      mem = 2;
      goto backref;
      break;

    case OP_BACKREFN:  MOP_IN(OP_BACKREFN);
      GET_MEMNUM_INC(mem, p);
    backref:
      {
        int len;
        UChar *pstart, *pend;

        /* if you want to remove following line, 
           you should check in parse and compile time. */
        if (mem > num_mem) goto fail;
        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        if (BIT_STATUS_AT(reg->bt_mem_start, mem))
          pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
        else
          pstart = (UChar* )((void* )mem_start_stk[mem]);

        pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
                ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                : (UChar* )((void* )mem_end_stk[mem]));
        n = pend - pstart;
        DATA_ENSURE(n);
        sprev = s;
        STRING_CMP(pstart, s, n);
        while (sprev + (len = enclen(encode, sprev)) < s)
          sprev += len;

        MOP_OUT;
        continue;
      }
      break;

    case OP_BACKREFN_IC:  MOP_IN(OP_BACKREFN_IC);
      GET_MEMNUM_INC(mem, p);
      {
        int len;
        UChar *pstart, *pend;

        /* if you want to remove following line, 
           you should check in parse and compile time. */
        if (mem > num_mem) goto fail;
        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        if (BIT_STATUS_AT(reg->bt_mem_start, mem))
          pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
        else
          pstart = (UChar* )((void* )mem_start_stk[mem]);

        pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
                ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                : (UChar* )((void* )mem_end_stk[mem]));
        n = pend - pstart;
        DATA_ENSURE(n);
        sprev = s;
        STRING_CMP_IC(case_fold_flag, pstart, &s, n);
        while (sprev + (len = enclen(encode, sprev)) < s)
          sprev += len;

        MOP_OUT;
        continue;
      }
      break;

    case OP_BACKREF_MULTI:  MOP_IN(OP_BACKREF_MULTI);
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        GET_LENGTH_INC(tlen, p);
        for (i = 0; i < tlen; i++) {
          GET_MEMNUM_INC(mem, p);

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          if (BIT_STATUS_AT(reg->bt_mem_start, mem))
            pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
          else
            pstart = (UChar* )((void* )mem_start_stk[mem]);

          pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
                  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                  : (UChar* )((void* )mem_end_stk[mem]));
          n = pend - pstart;
          DATA_ENSURE(n);
          sprev = s;
          swork = s;
          STRING_CMP_VALUE(pstart, swork, n, is_fail);
          if (is_fail) continue;
          s = swork;
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;

          p += (SIZE_MEMNUM * (tlen - i - 1));
          break; /* success */
        }
        if (i == tlen) goto fail;
        MOP_OUT;
        continue;
      }
      break;

    case OP_BACKREF_MULTI_IC:  MOP_IN(OP_BACKREF_MULTI_IC);
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        GET_LENGTH_INC(tlen, p);
        for (i = 0; i < tlen; i++) {
          GET_MEMNUM_INC(mem, p);

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          if (BIT_STATUS_AT(reg->bt_mem_start, mem))
            pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
          else
            pstart = (UChar* )((void* )mem_start_stk[mem]);

          pend = (BIT_STATUS_AT(reg->bt_mem_end, mem)
                  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                  : (UChar* )((void* )mem_end_stk[mem]));
          n = pend - pstart;
          DATA_ENSURE(n);
          sprev = s;
          swork = s;
          STRING_CMP_VALUE_IC(case_fold_flag, pstart, &swork, n, is_fail);
          if (is_fail) continue;
          s = swork;
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;

          p += (SIZE_MEMNUM * (tlen - i - 1));
          break; /* success */
        }
        if (i == tlen) goto fail;
        MOP_OUT;
        continue;
      }
      break;

#ifdef USE_BACKREF_WITH_LEVEL
    case OP_BACKREF_WITH_LEVEL:
      {
        int len;
        OnigOptionType ic;
        LengthType level;

        GET_OPTION_INC(ic,    p);
        GET_LENGTH_INC(level, p);
        GET_LENGTH_INC(tlen,  p);

        sprev = s;
        if (backref_match_at_nested_level(reg, stk, stk_base, ic
                     , case_fold_flag, (int )level, (int )tlen, p, &s, end)) {
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;

          p += (SIZE_MEMNUM * tlen);
        }
        else
          goto fail;

        MOP_OUT;
        continue;
      }
      break;
#endif

#if 0   /* no need: IS_DYNAMIC_OPTION() == 0 */
    case OP_SET_OPTION_PUSH:  MOP_IN(OP_SET_OPTION_PUSH);
      GET_OPTION_INC(option, p);
      STACK_PUSH_ALT(p, s, sprev);
      p += SIZE_OP_SET_OPTION + SIZE_OP_FAIL;
      MOP_OUT;
      continue;
      break;

    case OP_SET_OPTION:  MOP_IN(OP_SET_OPTION);
      GET_OPTION_INC(option, p);
      MOP_OUT;
      continue;
      break;
#endif

    case OP_NULL_CHECK_START:  MOP_IN(OP_NULL_CHECK_START);
      GET_MEMNUM_INC(mem, p);    /* mem: null check id */
      STACK_PUSH_NULL_CHECK_START(mem, s);
      MOP_OUT;
      continue;
      break;

    case OP_NULL_CHECK_END:  MOP_IN(OP_NULL_CHECK_END);
      {
        int isnull;

        GET_MEMNUM_INC(mem, p); /* mem: null check id */
        STACK_NULL_CHECK(isnull, mem, s);
        if (isnull) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "NULL_CHECK_END: skip  id:%d, s:%d\n",
                  (int )mem, (int )s);
#endif
        null_check_found:
          /* empty loop founded, skip next instruction */
          switch (*p++) {
          case OP_JUMP:
          case OP_PUSH:
            p += SIZE_RELADDR;
            break;
          case OP_REPEAT_INC:
          case OP_REPEAT_INC_NG:
          case OP_REPEAT_INC_SG:
          case OP_REPEAT_INC_NG_SG:
            p += SIZE_MEMNUM;
            break;
          default:
            goto unexpected_bytecode_error;
            break;
          }
        }
      }
      MOP_OUT;
      continue;
      break;

#ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
    case OP_NULL_CHECK_END_MEMST:  MOP_IN(OP_NULL_CHECK_END_MEMST);
      {
        int isnull;

        GET_MEMNUM_INC(mem, p); /* mem: null check id */
        STACK_NULL_CHECK_MEMST(isnull, mem, s, reg);
        if (isnull) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "NULL_CHECK_END_MEMST: skip  id:%d, s:%d\n",
                  (int )mem, (int )s);
#endif
          if (isnull == -1) goto fail;
          goto 	null_check_found;
        }
      }
      MOP_OUT;
      continue;
      break;
#endif

#ifdef USE_SUBEXP_CALL
    case OP_NULL_CHECK_END_MEMST_PUSH:
      MOP_IN(OP_NULL_CHECK_END_MEMST_PUSH);
      {
        int isnull;

        GET_MEMNUM_INC(mem, p); /* mem: null check id */
#ifdef USE_MONOMANIAC_CHECK_CAPTURES_IN_ENDLESS_REPEAT
        STACK_NULL_CHECK_MEMST_REC(isnull, mem, s, reg);
#else
        STACK_NULL_CHECK_REC(isnull, mem, s);
#endif
        if (isnull) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "NULL_CHECK_END_MEMST_PUSH: skip  id:%d, s:%d\n",
                  (int )mem, (int )s);
#endif
          if (isnull == -1) goto fail;
          goto 	null_check_found;
        }
        else {
          STACK_PUSH_NULL_CHECK_END(mem);
        }
      }
      MOP_OUT;
      continue;
      break;
#endif

    case OP_JUMP:  MOP_IN(OP_JUMP);
      GET_RELADDR_INC(addr, p);
      p += addr;
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      continue;
      break;

    case OP_PUSH:  MOP_IN(OP_PUSH);
      GET_RELADDR_INC(addr, p);
      STACK_PUSH_ALT(p + addr, s, sprev);
      MOP_OUT;
      continue;
      break;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
    case OP_STATE_CHECK_PUSH:  MOP_IN(OP_STATE_CHECK_PUSH);
      GET_STATE_CHECK_NUM_INC(mem, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) goto fail;

      GET_RELADDR_INC(addr, p);
      STACK_PUSH_ALT_WITH_STATE_CHECK(p + addr, s, sprev, mem);
      MOP_OUT;
      continue;
      break;

    case OP_STATE_CHECK_PUSH_OR_JUMP:  MOP_IN(OP_STATE_CHECK_PUSH_OR_JUMP);
      GET_STATE_CHECK_NUM_INC(mem, p);
      GET_RELADDR_INC(addr, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) {
        p += addr;
      }
      else {
        STACK_PUSH_ALT_WITH_STATE_CHECK(p + addr, s, sprev, mem);
      }
      MOP_OUT;
      continue;
      break;

    case OP_STATE_CHECK:  MOP_IN(OP_STATE_CHECK);
      GET_STATE_CHECK_NUM_INC(mem, p);
      STATE_CHECK_VAL(scv, mem);
      if (scv) goto fail;

      STACK_PUSH_STATE_CHECK(s, mem);
      MOP_OUT;
      continue;
      break;
#endif /* USE_COMBINATION_EXPLOSION_CHECK */

    case OP_POP:  MOP_IN(OP_POP);
      STACK_POP_ONE;
      MOP_OUT;
      continue;
      break;

    case OP_PUSH_OR_JUMP_EXACT1:  MOP_IN(OP_PUSH_OR_JUMP_EXACT1);
      GET_RELADDR_INC(addr, p);
      if (*p == *s && DATA_ENSURE_CHECK1) {
        p++;
        STACK_PUSH_ALT(p + addr, s, sprev);
        MOP_OUT;
        continue;
      }
      p += (addr + 1);
      MOP_OUT;
      continue;
      break;

    case OP_PUSH_IF_PEEK_NEXT:  MOP_IN(OP_PUSH_IF_PEEK_NEXT);
      GET_RELADDR_INC(addr, p);
      if (*p == *s) {
        p++;
        STACK_PUSH_ALT(p + addr, s, sprev);
        MOP_OUT;
        continue;
      }
      p++;
      MOP_OUT;
      continue;
      break;

    case OP_REPEAT:  MOP_IN(OP_REPEAT);
      {
        GET_MEMNUM_INC(mem, p);    /* mem: OP_REPEAT ID */
        GET_RELADDR_INC(addr, p);

        STACK_ENSURE(1);
        repeat_stk[mem] = GET_STACK_INDEX(stk);
        STACK_PUSH_REPEAT(mem, p);

        if (reg->repeat_range[mem].lower == 0) {
          STACK_PUSH_ALT(p + addr, s, sprev);
        }
      }
      MOP_OUT;
      continue;
      break;

    case OP_REPEAT_NG:  MOP_IN(OP_REPEAT_NG);
      {
        GET_MEMNUM_INC(mem, p);    /* mem: OP_REPEAT ID */
        GET_RELADDR_INC(addr, p);

        STACK_ENSURE(1);
        repeat_stk[mem] = GET_STACK_INDEX(stk);
        STACK_PUSH_REPEAT(mem, p);

        if (reg->repeat_range[mem].lower == 0) {
          STACK_PUSH_ALT(p, s, sprev);
          p += addr;
        }
      }
      MOP_OUT;
      continue;
      break;

    case OP_REPEAT_INC:  MOP_IN(OP_REPEAT_INC);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      si = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count >= reg->repeat_range[mem].upper) {
        /* end of repeat. Nothing to do. */
      }
      else if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
        STACK_PUSH_ALT(p, s, sprev);
        p = STACK_AT(si)->u.repeat.pcode; /* Don't use stkp after PUSH. */
      }
      else {
        p = stkp->u.repeat.pcode;
      }
      STACK_PUSH_REPEAT_INC(si);
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      continue;
      break;

    case OP_REPEAT_INC_SG:  MOP_IN(OP_REPEAT_INC_SG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc;
      break;

    case OP_REPEAT_INC_NG:  MOP_IN(OP_REPEAT_INC_NG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      si = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc_ng:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count < reg->repeat_range[mem].upper) {
        if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
          UChar* pcode = stkp->u.repeat.pcode;

          STACK_PUSH_REPEAT_INC(si);
          STACK_PUSH_ALT(pcode, s, sprev);
        }
        else {
          p = stkp->u.repeat.pcode;
          STACK_PUSH_REPEAT_INC(si);
        }
      }
      else if (stkp->u.repeat.count == reg->repeat_range[mem].upper) {
        STACK_PUSH_REPEAT_INC(si);
      }
      MOP_OUT;
      CHECK_INTERRUPT_IN_MATCH_AT;
      continue;
      break;

    case OP_REPEAT_INC_NG_SG:  MOP_IN(OP_REPEAT_INC_NG_SG);
      GET_MEMNUM_INC(mem, p); /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc_ng;
      break;

    case OP_PUSH_POS:  MOP_IN(OP_PUSH_POS);
      STACK_PUSH_POS(s, sprev);
      MOP_OUT;
      continue;
      break;

    case OP_POP_POS:  MOP_IN(OP_POP_POS);
      {
        STACK_POS_END(stkp);
        s     = stkp->u.state.pstr;
        sprev = stkp->u.state.pstr_prev;
      }
      MOP_OUT;
      continue;
      break;

    case OP_PUSH_POS_NOT:  MOP_IN(OP_PUSH_POS_NOT);
      GET_RELADDR_INC(addr, p);
      STACK_PUSH_POS_NOT(p + addr, s, sprev);
      MOP_OUT;
      continue;
      break;

    case OP_FAIL_POS:  MOP_IN(OP_FAIL_POS);
      STACK_POP_TIL_POS_NOT;
      goto fail;
      break;

    case OP_PUSH_STOP_BT:  MOP_IN(OP_PUSH_STOP_BT);
      STACK_PUSH_STOP_BT;
      MOP_OUT;
      continue;
      break;

    case OP_POP_STOP_BT:  MOP_IN(OP_POP_STOP_BT);
      STACK_STOP_BT_END;
      MOP_OUT;
      continue;
      break;

    case OP_LOOK_BEHIND:  MOP_IN(OP_LOOK_BEHIND);
      GET_LENGTH_INC(tlen, p);
      s = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(s)) goto fail;
      sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
      MOP_OUT;
      continue;
      break;

    case OP_PUSH_LOOK_BEHIND_NOT:  MOP_IN(OP_PUSH_LOOK_BEHIND_NOT);
      GET_RELADDR_INC(addr, p);
      GET_LENGTH_INC(tlen, p);
      q = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(q)) {
        /* too short case -> success. ex. /(?<!XXX)a/.match("a")
           If you want to change to fail, replace following line. */
        p += addr;
        /* goto fail; */
      }
      else {
        STACK_PUSH_LOOK_BEHIND_NOT(p + addr, s, sprev);
        s = q;
        sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
      }
      MOP_OUT;
      continue;
      break;

    case OP_FAIL_LOOK_BEHIND_NOT:  MOP_IN(OP_FAIL_LOOK_BEHIND_NOT);
      STACK_POP_TIL_LOOK_BEHIND_NOT;
      goto fail;
      break;

#ifdef USE_SUBEXP_CALL
    case OP_CALL:  MOP_IN(OP_CALL);
      GET_ABSADDR_INC(addr, p);
      STACK_PUSH_CALL_FRAME(p);
      p = reg->p + addr;
      MOP_OUT;
      continue;
      break;

    case OP_RETURN:  MOP_IN(OP_RETURN);
      STACK_RETURN(p);
      STACK_PUSH_RETURN;
      MOP_OUT;
      continue;
      break;
#endif

    case OP_FINISH:
      goto finish;
      break;

    fail:
      MOP_OUT;
      /* fall */
    case OP_FAIL:  MOP_IN(OP_FAIL);
      STACK_POP;
      p     = stk->u.state.pcode;
      s     = stk->u.state.pstr;
      sprev = stk->u.state.pstr_prev;

#ifdef USE_COMBINATION_EXPLOSION_CHECK
      if (stk->u.state.state_check != 0) {
        stk->type = STK_STATE_CHECK_MARK;
        stk++;
      }
#endif

      MOP_OUT;
      continue;
      break;

    default:
      goto bytecode_error;

    } /* end of switch */
    sprev = sbegin;
  } /* end of while(1) */

 finish:
  STACK_SAVE;
  return best_len;

#ifdef ONIG_DEBUG
 stack_error:
  STACK_SAVE;
  return ONIGERR_STACK_BUG;
#endif

 bytecode_error:
  STACK_SAVE;
  return ONIGERR_UNDEFINED_BYTECODE;

 unexpected_bytecode_error:
  STACK_SAVE;
  return ONIGERR_UNEXPECTED_BYTECODE;
}
