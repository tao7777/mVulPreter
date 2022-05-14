gdev_pdf_put_params_impl(gx_device * dev, const gx_device_pdf * save_dev, gs_param_list * plist)
{
    int ecode, code;
    gx_device_pdf *pdev = (gx_device_pdf *) dev;
    float cl = (float)pdev->CompatibilityLevel;
    bool locked = pdev->params.LockDistillerParams, ForOPDFRead;
    gs_param_name param_name;

    pdev->pdf_memory = gs_memory_stable(pdev->memory);
    /*
     * If this is a pseudo-parameter (pdfmark or DSC),
     * don't bother checking for any real ones.
     */

    {
        gs_param_string_array ppa;
        gs_param_string pps;

        code = param_read_string_array(plist, (param_name = "pdfmark"), &ppa);
        switch (code) {
            case 0:
                code = pdfwrite_pdf_open_document(pdev);
                if (code < 0)
                    return code;
                code = pdfmark_process(pdev, &ppa);
                if (code >= 0)
                    return code;
                /* falls through for errors */
            default:
                param_signal_error(plist, param_name, code);
                return code;
            case 1:
                break;
        }

        code = param_read_string_array(plist, (param_name = "DSC"), &ppa);
        switch (code) {
            case 0:
                code = pdfwrite_pdf_open_document(pdev);
                if (code < 0)
                    return code;
                code = pdf_dsc_process(pdev, &ppa);
                if (code >= 0)
                    return code;
                /* falls through for errors */
            default:
                param_signal_error(plist, param_name, code);
                return code;
            case 1:
                break;
        }

        code = param_read_string(plist, (param_name = "pdfpagelabels"), &pps);
        switch (code) {
            case 0:
                {
                    if (!pdev->ForOPDFRead) {
                        cos_dict_t *const pcd = pdev->Catalog;
                        code = pdfwrite_pdf_open_document(pdev);
                        if (code < 0)
                            return code;
                        code = cos_dict_put_string(pcd, (const byte *)"/PageLabels", 11,
                                   pps.data, pps.size);
                        if (code >= 0)
                            return code;
                    } else
                        return 0;
                 }
                /* falls through for errors */
            default:
                param_signal_error(plist, param_name, code);
                return code;
            case 1:
                break;
        }
    }

    /*
     * Check for LockDistillerParams before doing anything else.
     * If LockDistillerParams is true and is not being set to false,
     * ignore all resettings of PDF-specific parameters.  Note that
      * LockDistillerParams is read again, and reset if necessary, in
      * psdf_put_params.
      */
    ecode = param_read_bool(plist, "LockDistillerParams", &locked);
     if (ecode < 0)
         param_signal_error(plist, param_name, ecode);
    /* General parameters. */

    {
        int efo = 1;

        ecode = param_put_int(plist, (param_name = ".EmbedFontObjects"), &efo, ecode);
        if (ecode < 0)
            param_signal_error(plist, param_name, ecode);
        if (efo != 1)
            param_signal_error(plist, param_name, ecode = gs_error_rangecheck);
    }
    {
        int cdv = CoreDistVersion;

        ecode = param_put_int(plist, (param_name = "CoreDistVersion"), &cdv, ecode);
        if (ecode < 0)
            return gs_note_error(ecode);
        if (cdv != CoreDistVersion)
            param_signal_error(plist, param_name, ecode = gs_error_rangecheck);
    }

    switch (code = param_read_float(plist, (param_name = "CompatibilityLevel"), &cl)) {
        default:
            ecode = code;
            param_signal_error(plist, param_name, ecode);
            break;
        case 0:
            if (!(locked && pdev->params.LockDistillerParams)) {
                /*
                 * Must be 1.2, 1.3, 1.4, or 1.5.  Per Adobe documentation, substitute
                 * the nearest achievable value.
                 */
                if (cl < (float)1.15)
                    cl = (float)1.1;
                else if (cl < (float)1.25)
                    cl = (float)1.2;
                else if (cl < (float)1.35)
                    cl = (float)1.3;
                else if (cl < (float)1.45)
                    cl = (float)1.4;
                else if (cl < (float)1.55)
                    cl = (float)1.5;
                else if (cl < (float)1.65)
                    cl = (float)1.6;
                else if (cl < (float)1.75)
                    cl = (float)1.7;
                else {
                    cl = (float)2.0;
                    if (pdev->params.TransferFunctionInfo == tfi_Preserve)
                        pdev->params.TransferFunctionInfo = tfi_Apply;
                }
            }
        case 1:
            break;
    }
    {   /* HACK : gs_param_list_s::memory is documented in gsparam.h as
           "for allocating coerced arrays". Not sure why zputdeviceparams
           sets it to the current memory space, while the device
           assumes to store them in the device's memory space.
           As a hackish workaround we temporary replace it here.
           Doing so because we don't want to change the global code now
           because we're unable to test it with all devices.
           Bug 688531 "Segmentation fault running pdfwrite from 219-01.ps".

           This solution to be reconsidered after fixing
           the bug 688533 "zputdeviceparams specifies a wrong memory space.".
        */
        gs_memory_t *mem = plist->memory;

        plist->memory = pdev->pdf_memory;
        code = gs_param_read_items(plist, pdev, pdf_param_items);
        if (code < 0 || (code = param_read_bool(plist, "ForOPDFRead", &ForOPDFRead)) < 0)
        {
        }
        if (code == 0 && !pdev->is_ps2write && !(locked && pdev->params.LockDistillerParams))
            pdev->ForOPDFRead = ForOPDFRead;
        plist->memory = mem;
    }
    if (code < 0)
        ecode = code;
    {
        /*
         * Setting FirstObjectNumber is only legal if the file
         * has just been opened and nothing has been written,
         * or if we are setting it to the same value.
         */
        long fon = pdev->FirstObjectNumber;

        if (fon != save_dev->FirstObjectNumber) {
            if (fon <= 0 || fon > 0x7fff0000 ||
                (pdev->next_id != 0 &&
                 pdev->next_id !=
                 save_dev->FirstObjectNumber + pdf_num_initial_ids)
                ) {
                ecode = gs_error_rangecheck;
                param_signal_error(plist, "FirstObjectNumber", ecode);
            }
        }
    }
    {
        /*
         * Set ProcessColorModel now, because gx_default_put_params checks
         * it.
         */
        static const char *const pcm_names[] = {
            "DeviceGray", "DeviceRGB", "DeviceCMYK", "DeviceN", 0
        };
        int pcm = -1;

        ecode = param_put_enum(plist, "ProcessColorModel", &pcm,
                               pcm_names, ecode);
        if (pcm >= 0) {
            pdf_set_process_color_model(pdev, pcm);
            rc_decrement(pdev->icc_struct, "gdev_pdf_put_params_impl, ProcessColorModel changed");
            pdev->icc_struct = 0;
        }
    }
    if (ecode < 0)
        goto fail;

    if (pdev->is_ps2write && (code = param_read_bool(plist, "ProduceDSC", &pdev->ProduceDSC)) < 0) {
        param_signal_error(plist, param_name, code);
    }

    /* PDFA and PDFX are stored in the page device dictionary and therefore
     * set on every setpagedevice. However, if we have encountered a file which
     * can't be made this way, and the PDFACompatibilityPolicy is 1, we want to
     * continue producing the file, but not as a PDF/A or PDF/X file. Its more
     * or less impossible to alter the setting in the (potentially saved) page
     * device dictionary, so we use this rather clunky method.
     */
    if (pdev->PDFA < 0 || pdev->PDFA > 3){
        ecode = gs_note_error(gs_error_rangecheck);
        param_signal_error(plist, "PDFA", ecode);
        goto fail;
    }
    if(pdev->PDFA != 0 && pdev->AbortPDFAX)
        pdev->PDFA = 0;
    if(pdev->PDFX && pdev->AbortPDFAX)
        pdev->PDFX = 0;
    if (pdev->PDFX && pdev->PDFA != 0) {
        ecode = gs_note_error(gs_error_rangecheck);
        param_signal_error(plist, "PDFA", ecode);
        goto fail;
    }
    if (pdev->PDFX && pdev->ForOPDFRead) {
        ecode = gs_note_error(gs_error_rangecheck);
        param_signal_error(plist, "PDFX", ecode);
        goto fail;
    }
    if (pdev->PDFA != 0 && pdev->ForOPDFRead) {
        ecode = gs_note_error(gs_error_rangecheck);
        param_signal_error(plist, "PDFA", ecode);
        goto fail;
    }
    if (pdev->PDFA == 1 || pdev->PDFX || pdev->CompatibilityLevel < 1.4) {
         pdev->HaveTransparency = false;
         pdev->PreserveSMask = false;
    }

    /*
     * We have to set version to the new value, because the set of
     * legal parameter values for psdf_put_params varies according to
     * the version.
     */
    if (pdev->PDFX)
        cl = (float)1.3; /* Instead pdev->CompatibilityLevel = 1.2; - see below. */
    if (pdev->PDFA != 0 && cl < 1.4)
        cl = (float)1.4;
    pdev->version = (cl < 1.2 ? psdf_version_level2 : psdf_version_ll3);
    if (pdev->ForOPDFRead) {
        pdev->ResourcesBeforeUsage = true;
        pdev->HaveCFF = false;
        pdev->HavePDFWidths = false;
        pdev->HaveStrokeColor = false;
        cl = (float)1.2; /* Instead pdev->CompatibilityLevel = 1.2; - see below. */
        pdev->MaxInlineImageSize = max_long; /* Save printer's RAM from saving temporary image data.
                                                Immediate images doen't need buffering. */
        pdev->version = psdf_version_level2;
    } else {
        pdev->ResourcesBeforeUsage = false;
        pdev->HaveCFF = true;
        pdev->HavePDFWidths = true;
        pdev->HaveStrokeColor = true;
    }
    pdev->ParamCompatibilityLevel = cl;
    if (cl < 1.2) {
        pdev->HaveCFF = false;
    }
    ecode = gdev_psdf_put_params(dev, plist);
    if (ecode < 0)
        goto fail;

    if (pdev->CompatibilityLevel > 1.7 && pdev->params.TransferFunctionInfo == tfi_Preserve) {
        pdev->params.TransferFunctionInfo = tfi_Apply;
        emprintf(pdev->memory, "\nIt is not possible to preserve transfer functions in PDF 2.0\ntransfer functions will be applied instead\n");
    }

    if (pdev->params.ConvertCMYKImagesToRGB) {
        if (pdev->params.ColorConversionStrategy == ccs_CMYK) {
            emprintf(pdev->memory, "ConvertCMYKImagesToRGB is not compatible with ColorConversionStrategy of CMYK\n");
        } else {
            if (pdev->params.ColorConversionStrategy == ccs_Gray) {
                emprintf(pdev->memory, "ConvertCMYKImagesToRGB is not compatible with ColorConversionStrategy of Gray\n");
            } else {
                if (pdev->icc_struct)
                    rc_decrement(pdev->icc_struct,
                                 "reset default profile\n");
                pdf_set_process_color_model(pdev,1);
                ecode = gsicc_init_device_profile_struct((gx_device *)pdev, NULL, 0);
                if (ecode < 0)
                    goto fail;
            }
        }
    }
    switch (pdev->params.ColorConversionStrategy) {
        case ccs_ByObjectType:
        case ccs_LeaveColorUnchanged:
            break;
        case ccs_UseDeviceDependentColor:
        case ccs_UseDeviceIndependentColor:
        case ccs_UseDeviceIndependentColorForImages:
            pdev->params.TransferFunctionInfo = tfi_Apply;
            break;
        case ccs_CMYK:
            pdev->params.TransferFunctionInfo = tfi_Apply;
            if (pdev->icc_struct)
                rc_decrement(pdev->icc_struct,
                             "reset default profile\n");
            pdf_set_process_color_model(pdev, 2);
            ecode = gsicc_init_device_profile_struct((gx_device *)pdev, NULL, 0);
            if (ecode < 0)
                goto fail;
            break;
        case ccs_Gray:
            pdev->params.TransferFunctionInfo = tfi_Apply;
            if (pdev->icc_struct)
                rc_decrement(pdev->icc_struct,
                             "reset default profile\n");
            pdf_set_process_color_model(pdev,0);
            ecode = gsicc_init_device_profile_struct((gx_device *)pdev, NULL, 0);
            if (ecode < 0)
                goto fail;
            break;
        case ccs_sRGB:
        case ccs_RGB:
            pdev->params.TransferFunctionInfo = tfi_Apply;
            /* Only bother to do this if we didn't handle it above */
            if (!pdev->params.ConvertCMYKImagesToRGB) {
                if (pdev->icc_struct)
                    rc_decrement(pdev->icc_struct,
                                 "reset default profile\n");
                pdf_set_process_color_model(pdev,1);
                ecode = gsicc_init_device_profile_struct((gx_device *)pdev, NULL, 0);
                if (ecode < 0)
                    goto fail;
            }
            break;
        default:
            break;
    }
    if (cl < 1.5f && pdev->params.ColorImage.Filter != NULL &&
            !strcmp(pdev->params.ColorImage.Filter, "JPXEncode")) {
        emprintf(pdev->memory,
                 "JPXEncode requires CompatibilityLevel >= 1.5 .\n");
        ecode = gs_note_error(gs_error_rangecheck);
    }
    if (cl < 1.5f && pdev->params.GrayImage.Filter != NULL &&
            !strcmp(pdev->params.GrayImage.Filter, "JPXEncode")) {
        emprintf(pdev->memory,
                 "JPXEncode requires CompatibilityLevel >= 1.5 .\n");
        ecode = gs_note_error(gs_error_rangecheck);
    }
    if (cl < 1.4f && pdev->params.MonoImage.Filter != NULL &&
            !strcmp(pdev->params.MonoImage.Filter, "JBIG2Encode")) {
        emprintf(pdev->memory,
                 "JBIG2Encode requires CompatibilityLevel >= 1.4 .\n");
        ecode = gs_note_error(gs_error_rangecheck);
    }
    if (pdev->HaveTrueTypes && pdev->version == psdf_version_level2) {
        pdev->version = psdf_version_level2_with_TT ;
    }
    if (ecode < 0)
        goto fail;

    if (pdev->FirstObjectNumber != save_dev->FirstObjectNumber) {
        if (pdev->xref.file != 0) {
            if (gp_fseek_64(pdev->xref.file, 0L, SEEK_SET) != 0) {
                ecode = gs_error_ioerror;
                goto fail;
            }
            pdf_initialize_ids(pdev);
        }
    }
    /* Handle the float/double mismatch. */
    pdev->CompatibilityLevel = (int)(cl * 10 + 0.5) / 10.0;
    if(pdev->OwnerPassword.size != save_dev->OwnerPassword.size ||
        (pdev->OwnerPassword.size != 0 &&
         memcmp(pdev->OwnerPassword.data, save_dev->OwnerPassword.data,
         pdev->OwnerPassword.size) != 0)) {
        if (pdev->is_open) {
            if (pdev->PageCount == 0) {
                gs_closedevice((gx_device *)save_dev);
                return 0;
            }
            else
                emprintf(pdev->memory, "Owner Password changed mid-job, ignoring.\n");
        }
    }

    if (pdev->Linearise && pdev->is_ps2write) {
        emprintf(pdev->memory, "Can't linearise PostScript output, ignoring\n");
        pdev->Linearise = false;
    }

    if (pdev->Linearise && pdev->OwnerPassword.size != 0) {
        emprintf(pdev->memory, "Can't linearise encrypted PDF, ignoring\n");
        pdev->Linearise = false;
    }

    if (pdev->FlattenFonts)
        pdev->PreserveTrMode = false;
    return 0;
 fail:
    /* Restore all the parameters to their original state. */
    pdev->version = save_dev->version;
    pdf_set_process_color_model(pdev, save_dev->pcm_color_info_index);
    pdev->saved_fill_color = save_dev->saved_fill_color;
    pdev->saved_stroke_color = save_dev->saved_fill_color;
    {
        const gs_param_item_t *ppi = pdf_param_items;

        for (; ppi->key; ++ppi)
            memcpy((char *)pdev + ppi->offset,
                   (char *)save_dev + ppi->offset,
                   gs_param_type_sizes[ppi->type]);
        pdev->ForOPDFRead = save_dev->ForOPDFRead;
    }
    return ecode;
}
