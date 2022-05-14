void color_apply_icc_profile(opj_image_t *image)
{
    cmsHPROFILE in_prof, out_prof;
    cmsHTRANSFORM transform;
    cmsColorSpaceSignature in_space, out_space;
    cmsUInt32Number intent, in_type, out_type;
    int *r, *g, *b;
    size_t nr_samples, i, max, max_w, max_h;
    int prec, ok = 0;
    OPJ_COLOR_SPACE new_space;

    in_prof = cmsOpenProfileFromMem(image->icc_profile_buf, image->icc_profile_len);
#ifdef DEBUG_PROFILE
    FILE *icm = fopen("debug.icm", "wb");
    fwrite(image->icc_profile_buf, 1, image->icc_profile_len, icm);
    fclose(icm);
#endif

    if (in_prof == NULL) {
        return;
    }

    in_space = cmsGetPCS(in_prof);
    out_space = cmsGetColorSpace(in_prof);
    intent = cmsGetHeaderRenderingIntent(in_prof);


    max_w = image->comps[0].w;
    max_h = image->comps[0].h;
    prec = (int)image->comps[0].prec;

    if (out_space == cmsSigRgbData) { /* enumCS 16 */
        unsigned int i, nr_comp = image->numcomps;

        if (nr_comp > 4) {
            nr_comp = 4;
        }
        for (i = 1; i < nr_comp; ++i) { /* AFL test */
            if (image->comps[0].dx != image->comps[i].dx) {
                break;
            }

            if (image->comps[0].dy != image->comps[i].dy) {
                break;
            }

            if (image->comps[0].prec != image->comps[i].prec) {
                break;
            }

            if (image->comps[0].sgnd != image->comps[i].sgnd) {
                break;
            }

        }
        if (i != nr_comp) {
            cmsCloseProfile(in_prof);
            return;
        }

        if (prec <= 8) {
            in_type = TYPE_RGB_8;
            out_type = TYPE_RGB_8;
        } else {
            in_type = TYPE_RGB_16;
            out_type = TYPE_RGB_16;
        }
        out_prof = cmsCreate_sRGBProfile();
        new_space = OPJ_CLRSPC_SRGB;
    } else if (out_space == cmsSigGrayData) { /* enumCS 17 */
        in_type = TYPE_GRAY_8;
        out_type = TYPE_RGB_8;
        out_prof = cmsCreate_sRGBProfile();
        new_space = OPJ_CLRSPC_SRGB;
    } else if (out_space == cmsSigYCbCrData) { /* enumCS 18 */
        in_type = TYPE_YCbCr_16;
        out_type = TYPE_RGB_16;
        out_prof = cmsCreate_sRGBProfile();
        new_space = OPJ_CLRSPC_SRGB;
    } else {
#ifdef DEBUG_PROFILE
        fprintf(stderr, "%s:%d: color_apply_icc_profile\n\tICC Profile has unknown "
                "output colorspace(%#x)(%c%c%c%c)\n\tICC Profile ignored.\n",
                __FILE__, __LINE__, out_space,
                (out_space >> 24) & 0xff, (out_space >> 16) & 0xff,
                (out_space >> 8) & 0xff, out_space & 0xff);
#endif
        cmsCloseProfile(in_prof);

        return;
    }
    if (out_prof == NULL) {
        cmsCloseProfile(in_prof);
        return;
    }

#ifdef DEBUG_PROFILE
    fprintf(stderr,
            "%s:%d:color_apply_icc_profile\n\tchannels(%d) prec(%d) w(%d) h(%d)"
            "\n\tprofile: in(%p) out(%p)\n", __FILE__, __LINE__, image->numcomps, prec,
            max_w, max_h, (void*)in_prof, (void*)out_prof);

    fprintf(stderr, "\trender_intent (%u)\n\t"
            "color_space: in(%#x)(%c%c%c%c)   out:(%#x)(%c%c%c%c)\n\t"
            "       type: in(%u)              out:(%u)\n",
            intent,
            in_space,
            (in_space >> 24) & 0xff, (in_space >> 16) & 0xff,
            (in_space >> 8) & 0xff, in_space & 0xff,

            out_space,
            (out_space >> 24) & 0xff, (out_space >> 16) & 0xff,
            (out_space >> 8) & 0xff, out_space & 0xff,

            in_type, out_type
           );
#else
    (void)prec;
    (void)in_space;
#endif /* DEBUG_PROFILE */

    transform = cmsCreateTransform(in_prof, in_type, out_prof, out_type, intent, 0);

#ifdef OPJ_HAVE_LIBLCMS2
    /* Possible for: LCMS_VERSION >= 2000 :*/
    cmsCloseProfile(in_prof);
    cmsCloseProfile(out_prof);
#endif

    if (transform == NULL) {
#ifdef DEBUG_PROFILE
        fprintf(stderr, "%s:%d:color_apply_icc_profile\n\tcmsCreateTransform failed. "
                "ICC Profile ignored.\n", __FILE__, __LINE__);
#endif

#ifdef OPJ_HAVE_LIBLCMS1
        cmsCloseProfile(in_prof);
        cmsCloseProfile(out_prof);
#endif
        return;
     }
 
     if (image->numcomps > 2) { /* RGB, RGBA */
        if ((image->comps[0].w == image->comps[1].w &&
                image->comps[0].w == image->comps[2].w) &&
                (image->comps[0].h == image->comps[1].h &&
                 image->comps[0].h == image->comps[2].h)) {
            if (prec <= 8) {
                unsigned char *inbuf, *outbuf, *in, *out;

                max = max_w * max_h;
                nr_samples = (size_t)(max * 3U * sizeof(unsigned char));
                in = inbuf = (unsigned char*)opj_image_data_alloc(nr_samples);
                out = outbuf = (unsigned char*)opj_image_data_alloc(nr_samples);

                if (inbuf == NULL || outbuf == NULL) {
                    goto fails0;
                }

                r = image->comps[0].data;
                g = image->comps[1].data;
                b = image->comps[2].data;

                for (i = 0U; i < max; ++i) {
                    *in++ = (unsigned char) * r++;
                    *in++ = (unsigned char) * g++;
                    *in++ = (unsigned char) * b++;
                }

                cmsDoTransform(transform, inbuf, outbuf, (cmsUInt32Number)max);

                r = image->comps[0].data;
                g = image->comps[1].data;
                b = image->comps[2].data;

                for (i = 0U; i < max; ++i) {
                    *r++ = (int) * out++;
                    *g++ = (int) * out++;
                    *b++ = (int) * out++;
                }
                ok = 1;
 
 fails0:
                opj_image_data_free(inbuf);
                opj_image_data_free(outbuf);
            } else { /* prec > 8 */
                unsigned short *inbuf, *outbuf, *in, *out;

                max = max_w * max_h;
                nr_samples = (size_t)(max * 3U * sizeof(unsigned short));
                in = inbuf = (unsigned short*)opj_image_data_alloc(nr_samples);
                out = outbuf = (unsigned short*)opj_image_data_alloc(nr_samples);

                if (inbuf == NULL || outbuf == NULL) {
                    goto fails1;
                }

                r = image->comps[0].data;
                g = image->comps[1].data;
                b = image->comps[2].data;

                for (i = 0U  ; i < max; ++i) {
                    *in++ = (unsigned short) * r++;
                    *in++ = (unsigned short) * g++;
                    *in++ = (unsigned short) * b++;
                }

                cmsDoTransform(transform, inbuf, outbuf, (cmsUInt32Number)max);

                r = image->comps[0].data;
                g = image->comps[1].data;
                b = image->comps[2].data;

                for (i = 0; i < max; ++i) {
                    *r++ = (int) * out++;
                    *g++ = (int) * out++;
                    *b++ = (int) * out++;
                }
                ok = 1;
 
 fails1:
                opj_image_data_free(inbuf);
                opj_image_data_free(outbuf);
            }
        } else {
            fprintf(stderr,
                    "[ERROR] Image components should have the same width and height\n");
            cmsDeleteTransform(transform);
            return;
         }
     } else { /* image->numcomps <= 2 : GRAY, GRAYA */
         if (prec <= 8) {
            unsigned char *in, *inbuf, *out, *outbuf;
            opj_image_comp_t *new_comps;

            max = max_w * max_h;
            nr_samples = (size_t)(max * 3 * sizeof(unsigned char));
            in = inbuf = (unsigned char*)opj_image_data_alloc(nr_samples);
            out = outbuf = (unsigned char*)opj_image_data_alloc(nr_samples);
            g = (int*)opj_image_data_alloc((size_t)max * sizeof(int));
            b = (int*)opj_image_data_alloc((size_t)max * sizeof(int));

            if (inbuf == NULL || outbuf == NULL || g == NULL || b == NULL) {
                goto fails2;
            }

            new_comps = (opj_image_comp_t*)realloc(image->comps,
                                                   (image->numcomps + 2) * sizeof(opj_image_comp_t));

            if (new_comps == NULL) {
                goto fails2;
            }

            image->comps = new_comps;

            if (image->numcomps == 2) {
                image->comps[3] = image->comps[1];
            }

            image->comps[1] = image->comps[0];
            image->comps[2] = image->comps[0];

            image->comps[1].data = g;
            image->comps[2].data = b;

            image->numcomps += 2;

            r = image->comps[0].data;

            for (i = 0U; i < max; ++i) {
                *in++ = (unsigned char) * r++;
            }
            cmsDoTransform(transform, inbuf, outbuf, (cmsUInt32Number)max);

            r = image->comps[0].data;
            g = image->comps[1].data;
            b = image->comps[2].data;

            for (i = 0U; i < max; ++i) {
                *r++ = (int) * out++;
                *g++ = (int) * out++;
                *b++ = (int) * out++;
            }
            r = g = b = NULL;
            ok = 1;

fails2:
            opj_image_data_free(inbuf);
            opj_image_data_free(outbuf);
            opj_image_data_free(g);
            opj_image_data_free(b);
        } else { /* prec > 8 */
            unsigned short *in, *inbuf, *out, *outbuf;
            opj_image_comp_t *new_comps;

            max = max_w * max_h;
            nr_samples = (size_t)(max * 3U * sizeof(unsigned short));
            in = inbuf = (unsigned short*)opj_image_data_alloc(nr_samples);
            out = outbuf = (unsigned short*)opj_image_data_alloc(nr_samples);
            g = (int*)opj_image_data_alloc((size_t)max * sizeof(int));
            b = (int*)opj_image_data_alloc((size_t)max * sizeof(int));

            if (inbuf == NULL || outbuf == NULL || g == NULL || b == NULL) {
                goto fails3;
            }

            new_comps = (opj_image_comp_t*)realloc(image->comps,
                                                   (image->numcomps + 2) * sizeof(opj_image_comp_t));

            if (new_comps == NULL) {
                goto fails3;
            }

            image->comps = new_comps;

            if (image->numcomps == 2) {
                image->comps[3] = image->comps[1];
            }

            image->comps[1] = image->comps[0];
            image->comps[2] = image->comps[0];

            image->comps[1].data = g;
            image->comps[2].data = b;

            image->numcomps += 2;

            r = image->comps[0].data;

            for (i = 0U; i < max; ++i) {
                *in++ = (unsigned short) * r++;
            }
            cmsDoTransform(transform, inbuf, outbuf, (cmsUInt32Number)max);

            r = image->comps[0].data;
            g = image->comps[1].data;
            b = image->comps[2].data;

            for (i = 0; i < max; ++i) {
                *r++ = (int) * out++;
                *g++ = (int) * out++;
                *b++ = (int) * out++;
            }
            r = g = b = NULL;
            ok = 1;

fails3:
            opj_image_data_free(inbuf);
            opj_image_data_free(outbuf);
            opj_image_data_free(g);
            opj_image_data_free(b);
        }
    }/* if(image->numcomps > 2) */

    cmsDeleteTransform(transform);

#ifdef OPJ_HAVE_LIBLCMS1
    cmsCloseProfile(in_prof);
    cmsCloseProfile(out_prof);
#endif
    if (ok) {
        image->color_space = new_space;
    }
}/* color_apply_icc_profile() */
