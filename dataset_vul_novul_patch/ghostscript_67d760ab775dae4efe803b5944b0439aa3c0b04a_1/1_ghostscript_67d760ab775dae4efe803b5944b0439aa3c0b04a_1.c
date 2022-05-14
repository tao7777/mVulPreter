zset_outputintent(i_ctx_t * i_ctx_p)
{
    os_ptr                  op = osp;
    int                     code = 0;
    gx_device *dev = gs_currentdevice(igs);
    cmm_dev_profile_t       *dev_profile;
    stream *                s = 0L;
    ref *                   pnval;
    ref *                   pstrmval;
    int                     ncomps, dev_comps;
    cmm_profile_t           *picc_profile;
    int                     expected = 0;
    gs_color_space_index    index;
    gsicc_manager_t         *icc_manager = igs->icc_manager;
    cmm_profile_t           *source_profile = NULL;

    check_type(*op, t_dictionary);
    check_dict_read(*op);
    if_debug0m(gs_debug_flag_icc, imemory, "[icc] Using OutputIntent\n");

    /* Get the device structure */
    code = dev_proc(dev, get_profile)(dev,  &dev_profile);
    if (code < 0)
        return code;

    if (dev_profile == NULL) {
        code = gsicc_init_device_profile_struct(dev, NULL, 0);
        if (code < 0)
            return code;
        code = dev_proc(dev, get_profile)(dev,  &dev_profile);
        if (code < 0)
            return code;
    }
    if (dev_profile->oi_profile != NULL) {
        return 0;  /* Allow only one setting of this object */
    }
    code = dict_find_string(op, "N", &pnval);
    if (code < 0)
         return code;
     if (code == 0)
         return_error(gs_error_undefined);
     ncomps = pnval->value.intval;
 
     /* verify the DataSource entry. Creat profile from stream */
    check_read_file(i_ctx_p, s, pstrmval);

    picc_profile = gsicc_profile_new(s, gs_gstate_memory(igs), NULL, 0);
    if (picc_profile == NULL)
        return gs_throw(gs_error_VMerror, "Creation of ICC profile failed");
    picc_profile->num_comps = ncomps;
    picc_profile->profile_handle =
        gsicc_get_profile_handle_buffer(picc_profile->buffer,
                                        picc_profile->buffer_size,
                                        gs_gstate_memory(igs));
    if (picc_profile->profile_handle == NULL) {
        rc_decrement(picc_profile,"zset_outputintent");
        return -1;
    }
    picc_profile->data_cs =
        gscms_get_profile_data_space(picc_profile->profile_handle,
            picc_profile->memory);
    switch (picc_profile->data_cs) {
        case gsCIEXYZ:
        case gsCIELAB:
        case gsRGB:
            expected = 3;
            source_profile = icc_manager->default_rgb;
            break;
        case gsGRAY:
            expected = 1;
            source_profile = icc_manager->default_gray;
            break;
        case gsCMYK:
            expected = 4;
            source_profile = icc_manager->default_cmyk;
            break;
        case gsNCHANNEL:
            expected = 0;
            break;
        case gsNAMED:
        case gsUNDEFINED:
            break;
    }
    if (expected && ncomps != expected) {
        rc_decrement(picc_profile,"zset_outputintent");
        return_error(gs_error_rangecheck);
    }
    gsicc_init_hash_cs(picc_profile, igs);

    /* All is well with the profile.  Lets set the stuff that needs to be set */
    dev_profile->oi_profile = picc_profile;
    picc_profile->name = (char *) gs_alloc_bytes(picc_profile->memory,
                                                 MAX_DEFAULT_ICC_LENGTH,
                                                 "zset_outputintent");
    strncpy(picc_profile->name, OI_PROFILE, strlen(OI_PROFILE));
    picc_profile->name[strlen(OI_PROFILE)] = 0;
    picc_profile->name_length = strlen(OI_PROFILE);
    /* Set the range of the profile */
    gsicc_set_icc_range(&picc_profile);

    /* If the output device has a different number of componenets, then we are
       going to set the output intent as the proofing profile, unless the
       proofing profile has already been set.

       If the device has the same number of components (and color model) then as
       the profile we will use this as the output profile, unless someone has
       explicitly set the output profile.

       Finally, we will use the output intent profile for the default profile
       of the proper Device profile in the icc manager, again, unless someone
       has explicitly set this default profile. */

    dev_comps = dev_profile->device_profile[0]->num_comps;
    index = gsicc_get_default_type(dev_profile->device_profile[0]);
    if (ncomps == dev_comps && index < gs_color_space_index_DevicePixel) {
        /* The OI profile is the same type as the profile for the device and a
           "default" profile for the device was not externally set. So we go
           ahead and use the OI profile as the device profile.  Care needs to be
           taken here to keep from screwing up any device parameters.   We will
           use a keyword of OIProfile for the user/device parameter to indicate
           its usage.  Also, note conflicts if one is setting object dependent
           color management */
        rc_assign(dev_profile->device_profile[0], picc_profile,
                  "zset_outputintent");
        if_debug0m(gs_debug_flag_icc, imemory, "[icc] OutputIntent used for device profile\n");
    } else {
        if (dev_profile->proof_profile == NULL) {
            /* This means that we should use the OI profile as the proofing
               profile.  Note that if someone already has specified a
               proofing profile it is unclear what they are trying to do
               with the output intent.  In this case, we will use it
               just for the source data below */
            dev_profile->proof_profile = picc_profile;
            rc_increment(picc_profile);
            if_debug0m(gs_debug_flag_icc, imemory, "[icc] OutputIntent used for proof profile\n");
        }
    }
    /* Now the source colors.  See which source color space needs to use the
       output intent ICC profile */
    index = gsicc_get_default_type(source_profile);
    if (index < gs_color_space_index_DevicePixel) {
        /* source_profile is currently the default.  Set it to the OI profile */
        switch (picc_profile->data_cs) {
            case gsGRAY:
                if_debug0m(gs_debug_flag_icc, imemory, "[icc] OutputIntent used source Gray\n");
                rc_assign(icc_manager->default_gray, picc_profile,
                          "zset_outputintent");
                break;
            case gsRGB:
                if_debug0m(gs_debug_flag_icc, imemory, "[icc] OutputIntent used source RGB\n");
                rc_assign(icc_manager->default_rgb, picc_profile,
                          "zset_outputintent");
                break;
            case gsCMYK:
                if_debug0m(gs_debug_flag_icc, imemory, "[icc] OutputIntent used source CMYK\n");
                rc_assign(icc_manager->default_cmyk, picc_profile,
                          "zset_outputintent");
                break;
            default:
                break;
        }
    }
    /* Remove the output intent dict from the stack */
    pop(1);
    return code;
}
