static HB_Bool myanmar_shape_syllable(HB_Bool openType, HB_ShaperItem *item, HB_Bool invalid)
{
    /*
    */

#ifndef NO_OPENTYPE
    const int availableGlyphs = item->num_glyphs;
#endif
    const HB_UChar16 *uc = item->string + item->item.pos;
    int vowel_e = -1;
    int kinzi = -1;
    int medial_ra = -1;
    int base = -1;
    int i;
    int len = 0;
    unsigned short reordered[32];
    unsigned char properties[32];
    enum {
	AboveForm = 0x01,
	PreForm = 0x02,
	PostForm = 0x04,
	BelowForm = 0x08
    };
    HB_Bool lastWasVirama = FALSE;
    int basePos = -1;

    memset(properties, 0, 32*sizeof(unsigned char));

    /* according to the table the max length of a syllable should be around 14 chars */
    assert(item->item.length < 32);

#ifdef MYANMAR_DEBUG
    printf("original:");
    for (i = 0; i < (int)item->item.length; i++) {
        printf("    %d: %4x", i, uc[i]);
    }
#endif
    for (i = 0; i < (int)item->item.length; ++i) {
        HB_UChar16 chr = uc[i];

        if (chr == Mymr_C_VOWEL_E) {
            vowel_e = i;
            continue;
        }
        if (i == 0
            && chr == Mymr_C_NGA
            && i + 2 < (int)item->item.length
            && uc[i+1] == Mymr_C_VIRAMA) {
            int mc = getMyanmarCharClass(uc[i+2]);
            /*MMDEBUG("maybe kinzi: mc=%x", mc);*/
            if ((mc & Mymr_CF_CONSONANT) == Mymr_CF_CONSONANT) {
                kinzi = i;
                continue;
            }
        }
        if (base >= 0
            && chr == Mymr_C_VIRAMA
            && i + 1 < (int)item->item.length
            && uc[i+1] == Mymr_C_RA) {
            medial_ra = i;
            continue;
        }
        if (base < 0)
            base = i;
    }

    MMDEBUG("\n  base=%d, vowel_e=%d, kinzi=%d, medial_ra=%d", base, vowel_e, kinzi, medial_ra);
    /* write vowel_e if found */
    if (vowel_e >= 0) {
        reordered[0] = Mymr_C_VOWEL_E;
        len = 1;
    }
    /* write medial_ra */
    if (medial_ra >= 0) {
        reordered[len] = Mymr_C_VIRAMA;
        reordered[len+1] = Mymr_C_RA;
        properties[len] = PreForm;
        properties[len+1] = PreForm;
        len += 2;
    }

    /* shall we add a dotted circle?
       If in the position in which the base should be (first char in the string) there is
       a character that has the Dotted circle flag (a character that cannot be a base)
       then write a dotted circle */
    if (invalid) {
        reordered[len] = C_DOTTED_CIRCLE;
        ++len;
    }

    /* copy the rest of the syllable to the output, inserting the kinzi
       at the correct place */
    for (i = 0; i < (int)item->item.length; ++i) {
        hb_uint16 chr = uc[i];
        MymrCharClass cc;
        if (i == vowel_e)
            continue;
        if (i == medial_ra || i == kinzi) {
            ++i;
            continue;
        }

        cc = getMyanmarCharClass(uc[i]);
         if (kinzi >= 0 && i > base && (cc & Mymr_CF_AFTER_KINZI)) {
             reordered[len] = Mymr_C_NGA;
             reordered[len+1] = Mymr_C_VIRAMA;
            if (len > 0)
                properties[len-1] = AboveForm;
             properties[len] = AboveForm;
             len += 2;
             kinzi = -1;
        }

        if (lastWasVirama) {
            int prop = 0;
            switch(cc & Mymr_CF_POS_MASK) {
            case Mymr_CF_POS_BEFORE:
                prop = PreForm;
                break;
            case Mymr_CF_POS_BELOW:
                prop = BelowForm;
                break;
            case Mymr_CF_POS_ABOVE:
                prop = AboveForm;
                break;
            case Mymr_CF_POS_AFTER:
                prop = PostForm;
                break;
            default:
                break;
            }
            properties[len-1] = prop;
            properties[len] = prop;
            if(basePos >= 0 && basePos == len-2)
                properties[len-2] = prop;
        }
        lastWasVirama = (chr == Mymr_C_VIRAMA);
        if(i == base)
            basePos = len;

        if ((chr != Mymr_C_SIGN_ZWNJ && chr != Mymr_C_SIGN_ZWJ) || !len) {
            reordered[len] = chr;
            ++len;
        }
    }
    if (kinzi >= 0) {
        reordered[len] = Mymr_C_NGA;
        reordered[len+1] = Mymr_C_VIRAMA;
        properties[len] = AboveForm;
        properties[len+1] = AboveForm;
        len += 2;
    }

    if (!item->font->klass->convertStringToGlyphIndices(item->font,
                                                        reordered, len,
                                                        item->glyphs, &item->num_glyphs,
                                                        item->item.bidiLevel % 2))
        return FALSE;

    MMDEBUG("after shaping: len=%d", len);
    for (i = 0; i < len; i++) {
	item->attributes[i].mark = FALSE;
	item->attributes[i].clusterStart = FALSE;
	item->attributes[i].justification = 0;
	item->attributes[i].zeroWidth = FALSE;
	MMDEBUG("    %d: %4x property=%x", i, reordered[i], properties[i]);
    }

    /* now we have the syllable in the right order, and can start running it through open type. */

#ifndef NO_OPENTYPE
    if (openType) {
 	hb_uint32 where[32];

        for (i = 0; i < len; ++i) {
            where[i] = ~(PreSubstProperty
                         | BelowSubstProperty
                         | AboveSubstProperty
                         | PostSubstProperty
                         | CligProperty
                         | PositioningProperties);
            if (properties[i] & PreForm)
                where[i] &= ~PreFormProperty;
            if (properties[i] & BelowForm)
                where[i] &= ~BelowFormProperty;
            if (properties[i] & AboveForm)
                where[i] &= ~AboveFormProperty;
            if (properties[i] & PostForm)
                where[i] &= ~PostFormProperty;
        }

        HB_OpenTypeShape(item, where);
        if (!HB_OpenTypePosition(item, availableGlyphs, /*doLogClusters*/FALSE))
            return FALSE;
    } else
#endif
    {
	MMDEBUG("Not using openType");
        HB_HeuristicPosition(item);
    }

    item->attributes[0].clusterStart = TRUE;
    return TRUE;
}
