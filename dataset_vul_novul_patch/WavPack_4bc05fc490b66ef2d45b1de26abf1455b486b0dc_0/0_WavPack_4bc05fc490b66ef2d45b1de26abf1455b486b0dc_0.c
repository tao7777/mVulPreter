int32_t FASTCALL get_word (WavpackStream *wps, int chan, int32_t *correction)
{
    register struct entropy_data *c = wps->w.c + chan;
    uint32_t ones_count, low, mid, high;
    int32_t value;
    int sign;

    if (!wps->wvbits.ptr)
        return WORD_EOF;

    if (correction)
        *correction = 0;

    if (!(wps->w.c [0].median [0] & ~1) && !wps->w.holding_zero && !wps->w.holding_one && !(wps->w.c [1].median [0] & ~1)) {
        uint32_t mask;
        int cbits;

        if (wps->w.zeros_acc) {
            if (--wps->w.zeros_acc) {
                c->slow_level -= (c->slow_level + SLO) >> SLS;
                return 0;
            }
        }
        else {
            for (cbits = 0; cbits < 33 && getbit (&wps->wvbits); ++cbits);

            if (cbits == 33)
                return WORD_EOF;

            if (cbits < 2)
                wps->w.zeros_acc = cbits;
            else {
                for (mask = 1, wps->w.zeros_acc = 0; --cbits; mask <<= 1)
                    if (getbit (&wps->wvbits))
                        wps->w.zeros_acc |= mask;

                wps->w.zeros_acc |= mask;
            }

            if (wps->w.zeros_acc) {
                c->slow_level -= (c->slow_level + SLO) >> SLS;
                CLEAR (wps->w.c [0].median);
                CLEAR (wps->w.c [1].median);
                return 0;
            }
        }
    }

    if (wps->w.holding_zero)
        ones_count = wps->w.holding_zero = 0;
    else {
#ifdef USE_CTZ_OPTIMIZATION
        while (wps->wvbits.bc < LIMIT_ONES) {
            if (++(wps->wvbits.ptr) == wps->wvbits.end)
                wps->wvbits.wrap (&wps->wvbits);

            wps->wvbits.sr |= *(wps->wvbits.ptr) << wps->wvbits.bc;
            wps->wvbits.bc += sizeof (*(wps->wvbits.ptr)) * 8;
        }

#ifdef _WIN32
        _BitScanForward (&ones_count, ~wps->wvbits.sr);
#else
        ones_count = __builtin_ctz (~wps->wvbits.sr);
#endif

        if (ones_count >= LIMIT_ONES) {
            wps->wvbits.bc -= ones_count;
            wps->wvbits.sr >>= ones_count;

            for (; ones_count < (LIMIT_ONES + 1) && getbit (&wps->wvbits); ++ones_count);

            if (ones_count == (LIMIT_ONES + 1))
                return WORD_EOF;

            if (ones_count == LIMIT_ONES) {
                uint32_t mask;
                int cbits;

                for (cbits = 0; cbits < 33 && getbit (&wps->wvbits); ++cbits);

                if (cbits == 33)
                    return WORD_EOF;

                if (cbits < 2)
                    ones_count = cbits;
                else {
                    for (mask = 1, ones_count = 0; --cbits; mask <<= 1)
                        if (getbit (&wps->wvbits))
                            ones_count |= mask;

                    ones_count |= mask;
                }

                ones_count += LIMIT_ONES;
            }
        }
        else {
            wps->wvbits.bc -= ones_count + 1;
            wps->wvbits.sr >>= ones_count + 1;
        }
#elif defined (USE_NEXT8_OPTIMIZATION)
        int next8;

        if (wps->wvbits.bc < 8) {
            if (++(wps->wvbits.ptr) == wps->wvbits.end)
                wps->wvbits.wrap (&wps->wvbits);

            next8 = (wps->wvbits.sr |= *(wps->wvbits.ptr) << wps->wvbits.bc) & 0xff;
            wps->wvbits.bc += sizeof (*(wps->wvbits.ptr)) * 8;
        }
        else
            next8 = wps->wvbits.sr & 0xff;

        if (next8 == 0xff) {
            wps->wvbits.bc -= 8;
            wps->wvbits.sr >>= 8;

            for (ones_count = 8; ones_count < (LIMIT_ONES + 1) && getbit (&wps->wvbits); ++ones_count);

            if (ones_count == (LIMIT_ONES + 1))
                return WORD_EOF;

            if (ones_count == LIMIT_ONES) {
                uint32_t mask;
                int cbits;

                for (cbits = 0; cbits < 33 && getbit (&wps->wvbits); ++cbits);

                if (cbits == 33)
                    return WORD_EOF;

                if (cbits < 2)
                    ones_count = cbits;
                else {
                    for (mask = 1, ones_count = 0; --cbits; mask <<= 1)
                        if (getbit (&wps->wvbits))
                            ones_count |= mask;

                    ones_count |= mask;
                }

                ones_count += LIMIT_ONES;
            }
        }
        else {
            wps->wvbits.bc -= (ones_count = ones_count_table [next8]) + 1;
            wps->wvbits.sr >>= ones_count + 1;
        }
#else
        for (ones_count = 0; ones_count < (LIMIT_ONES + 1) && getbit (&wps->wvbits); ++ones_count);

        if (ones_count >= LIMIT_ONES) {
            uint32_t mask;
            int cbits;

            if (ones_count == (LIMIT_ONES + 1))
                return WORD_EOF;

            for (cbits = 0; cbits < 33 && getbit (&wps->wvbits); ++cbits);

            if (cbits == 33)
                return WORD_EOF;

            if (cbits < 2)
                ones_count = cbits;
            else {
                for (mask = 1, ones_count = 0; --cbits; mask <<= 1)
                    if (getbit (&wps->wvbits))
                        ones_count |= mask;

                ones_count |= mask;
            }

            ones_count += LIMIT_ONES;
        }
#endif

        if (wps->w.holding_one) {
            wps->w.holding_one = ones_count & 1;
            ones_count = (ones_count >> 1) + 1;
        }
        else {
            wps->w.holding_one = ones_count & 1;
            ones_count >>= 1;
        }

        wps->w.holding_zero = ~wps->w.holding_one & 1;
    }

    if ((wps->wphdr.flags & HYBRID_FLAG) && !chan)
        update_error_limit (wps);

    if (ones_count == 0) {
        low = 0;
        high = GET_MED (0) - 1;
        DEC_MED0 ();
    }
    else {
        low = GET_MED (0);
        INC_MED0 ();

        if (ones_count == 1) {
            high = low + GET_MED (1) - 1;
            DEC_MED1 ();
        }
        else {
            low += GET_MED (1);
            INC_MED1 ();

            if (ones_count == 2) {
                high = low + GET_MED (2) - 1;
                DEC_MED2 ();
            }
            else {
                low += (ones_count - 2) * GET_MED (2);
                high = low + GET_MED (2) - 1;
                INC_MED2 ();
            }
        }
    }
 
     low &= 0x7fffffff;
     high &= 0x7fffffff;

    if (low > high)         // make sure high and low make sense
        high = low;

     mid = (high + low + 1) >> 1;
 
     if (!c->error_limit)
        mid = read_code (&wps->wvbits, high - low) + low;
    else while (high - low > c->error_limit) {
        if (getbit (&wps->wvbits))
            mid = (high + (low = mid) + 1) >> 1;
        else
            mid = ((high = mid - 1) + low + 1) >> 1;
    }

    sign = getbit (&wps->wvbits);

    if (bs_is_open (&wps->wvcbits) && c->error_limit) {
        value = read_code (&wps->wvcbits, high - low) + low;

        if (correction)
            *correction = sign ? (mid - value) : (value - mid);
    }

    if (wps->wphdr.flags & HYBRID_BITRATE) {
        c->slow_level -= (c->slow_level + SLO) >> SLS;
        c->slow_level += wp_log2 (mid);
    }

    return sign ? ~mid : mid;
}
