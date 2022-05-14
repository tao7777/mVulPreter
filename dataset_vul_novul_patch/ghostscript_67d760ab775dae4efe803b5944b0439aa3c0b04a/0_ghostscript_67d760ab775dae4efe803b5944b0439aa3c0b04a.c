int seticc(i_ctx_t * i_ctx_p, int ncomps, ref *ICCdict, float *range_buff)
{
    int                     code, k;
    gs_color_space *        pcs;
    ref *                   pstrmval;
    stream *                s = 0L;
    cmm_profile_t           *picc_profile = NULL;
    int                     i, expected = 0;
    ref *                   pnameval;
    static const char *const icc_std_profile_names[] = {
            GSICC_STANDARD_PROFILES
        };
    static const char *const icc_std_profile_keys[] = {
            GSICC_STANDARD_PROFILES_KEYS
        };

    /* verify the DataSource entry */
    if (dict_find_string(ICCdict, "DataSource", &pstrmval) <= 0)
        return_error(gs_error_undefined);
    check_read_file(i_ctx_p, s, pstrmval);

    /* build the color space object */
    code = gs_cspace_build_ICC(&pcs, NULL, gs_gstate_memory(igs));
    if (code < 0)
        return gs_rethrow(code, "building color space object");
    /*  For now, dump the profile into a buffer
        and obtain handle from the buffer when we need it.
        We may want to change this later.
        This depends to some degree on what the CMS is capable of doing.
        I don't want to get bogged down on stream I/O at this point.
        Note also, if we are going to be putting these into the clist we will
         want to have this buffer. */
     /* Check if we have the /Name entry.  This is used to associate with
        specs that have enumerated types to indicate sRGB sGray etc */
    if (dict_find_string(ICCdict, "Name", &pnameval) > 0 && r_has_type(pnameval, t_string)){
         uint size = r_size(pnameval);
         char *str = (char *)gs_alloc_bytes(gs_gstate_memory(igs), size+1, "seticc");
         memcpy(str, (const char *)pnameval->value.bytes, size);
        str[size] = 0;

        /* Compare this to the standard profile names */
        for (k = 0; k < GSICC_NUMBER_STANDARD_PROFILES; k++) {
            if ( strcmp( str, icc_std_profile_keys[k] ) == 0 ) {
                picc_profile = gsicc_get_profile_handle_file(icc_std_profile_names[k],
                    strlen(icc_std_profile_names[k]), gs_gstate_memory(igs));
                break;
            }
        }
        gs_free_object(gs_gstate_memory(igs), str, "seticc");
    } else {
        picc_profile = gsicc_profile_new(s, gs_gstate_memory(igs), NULL, 0);
        if (picc_profile == NULL)
            return gs_throw(gs_error_VMerror, "Creation of ICC profile failed");
        /* We have to get the profile handle due to the fact that we need to know
           if it has a data space that is CIELAB */
        picc_profile->profile_handle =
            gsicc_get_profile_handle_buffer(picc_profile->buffer,
                                            picc_profile->buffer_size,
                                            gs_gstate_memory(igs));
    }
    if (picc_profile == NULL || picc_profile->profile_handle == NULL) {
        /* Free up everything, the profile is not valid. We will end up going
           ahead and using a default based upon the number of components */
        rc_decrement(picc_profile,"seticc");
        rc_decrement(pcs,"seticc");
        return -1;
    }
    code = gsicc_set_gscs_profile(pcs, picc_profile, gs_gstate_memory(igs));
    if (code < 0) {
        rc_decrement(picc_profile,"seticc");
        rc_decrement(pcs,"seticc");
        return code;
    }
    picc_profile->num_comps = ncomps;

    picc_profile->data_cs =
        gscms_get_profile_data_space(picc_profile->profile_handle,
            picc_profile->memory);
    switch (picc_profile->data_cs) {
        case gsCIEXYZ:
        case gsCIELAB:
        case gsRGB:
            expected = 3;
            break;
        case gsGRAY:
            expected = 1;
            break;
        case gsCMYK:
            expected = 4;
            break;
        case gsNCHANNEL:
        case gsNAMED:            /* Silence warnings */
        case gsUNDEFINED:        /* Silence warnings */
            break;
    }
    if (!expected || ncomps != expected) {
        rc_decrement(picc_profile,"seticc");
        rc_decrement(pcs,"seticc");
        return_error(gs_error_rangecheck);
    }

    /* Lets go ahead and get the hash code and check if we match one of the default spaces */
    /* Later we may want to delay this, but for now lets go ahead and do it */
    gsicc_init_hash_cs(picc_profile, igs);

    /* Set the range according to the data type that is associated with the
       ICC input color type.  Occasionally, we will run into CIELAB to CIELAB
       profiles for spot colors in PDF documents. These spot colors are typically described
       as separation colors with tint transforms that go from a tint value
       to a linear mapping between the CIELAB white point and the CIELAB tint
       color.  This results in a CIELAB value that we need to use to fill.  We
       need to detect this to make sure we do the proper scaling of the data.  For
       CIELAB images in PDF, the source is always normal 8 or 16 bit encoded data
       in the range from 0 to 255 or 0 to 65535.  In that case, there should not
       be any encoding and decoding to CIELAB.  The PDF content will not include
       an ICC profile but will set the color space to \Lab.  In this case, we use
       our seticc_lab operation to install the LAB to LAB profile, but we detect
       that we did that through the use of the is_lab flag in the profile descriptor.
       When then avoid the CIELAB encode and decode */
    if (picc_profile->data_cs == gsCIELAB) {
    /* If the input space to this profile is CIELAB, then we need to adjust the limits */
        /* See ICC spec ICC.1:2004-10 Section 6.3.4.2 and 6.4.  I don't believe we need to
           worry about CIEXYZ profiles or any of the other odds ones.  Need to check that though
           at some point. */
        picc_profile->Range.ranges[0].rmin = 0.0;
        picc_profile->Range.ranges[0].rmax = 100.0;
        picc_profile->Range.ranges[1].rmin = -128.0;
        picc_profile->Range.ranges[1].rmax = 127.0;
        picc_profile->Range.ranges[2].rmin = -128.0;
        picc_profile->Range.ranges[2].rmax = 127.0;
        picc_profile->islab = true;
    } else {
        for (i = 0; i < ncomps; i++) {
            picc_profile->Range.ranges[i].rmin = range_buff[2 * i];
            picc_profile->Range.ranges[i].rmax = range_buff[2 * i + 1];
        }
    }
    /* Now see if we are in an overide situation.  We have to wait until now
       in case this is an LAB profile which we will not overide */
    if (gs_currentoverrideicc(igs) && picc_profile->data_cs != gsCIELAB) {
        /* Free up the profile structure */
        switch( picc_profile->data_cs ) {
            case gsRGB:
                pcs->cmm_icc_profile_data = igs->icc_manager->default_rgb;
                break;
            case gsGRAY:
                pcs->cmm_icc_profile_data = igs->icc_manager->default_gray;
                break;
            case gsCMYK:
                pcs->cmm_icc_profile_data = igs->icc_manager->default_cmyk;
                break;
            default:
                break;
        }
        /* Have one increment from the color space.  Having these tied
           together is not really correct.  Need to fix that.  ToDo.  MJV */
        rc_adjust(picc_profile, -2, "seticc");
        rc_increment(pcs->cmm_icc_profile_data);
    }
    /* Set the color space.  We are done.  No joint cache here... */
    code = gs_setcolorspace(igs, pcs);
    /* The context has taken a reference to the colorspace. We no longer need
     * ours, so drop it. */
    rc_decrement_only(pcs, "seticc");
    /* In this case, we already have a ref count of 2 on the icc profile
       one for when it was created and one for when it was set.  We really
       only want one here so adjust */
    rc_decrement(picc_profile,"seticc");
    /* Remove the ICC dict from the stack */
    pop(1);
    return code;
}
