METHODDEF(JDIMENSION)
get_8bit_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 8-bit colormap indexes */
 {
   bmp_source_ptr source = (bmp_source_ptr)sinfo;
   register JSAMPARRAY colormap = source->colormap;
  int cmaplen = source->cmap_length;
   JSAMPARRAY image_ptr;
   register int t;
   register JSAMPROW inptr, outptr;
  register JDIMENSION col;

  if (source->use_inversion_array) {
    /* Fetch next row from virtual array */
    source->source_row--;
    image_ptr = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr)cinfo, source->whole_image,
       source->source_row, (JDIMENSION)1, FALSE);
    inptr = image_ptr[0];
  } else {
    if (!ReadOK(source->pub.input_file, source->iobuffer, source->row_width))
      ERREXIT(cinfo, JERR_INPUT_EOF);
    inptr = source->iobuffer;
  }

  /* Expand the colormap indexes to real data */
  outptr = source->pub.buffer[0];
   if (cinfo->in_color_space == JCS_GRAYSCALE) {
     for (col = cinfo->image_width; col > 0; col--) {
       t = GETJSAMPLE(*inptr++);
      if (t >= cmaplen)
        ERREXIT(cinfo, JERR_BMP_OUTOFRANGE);
       *outptr++ = colormap[0][t];
     }
   } else if (cinfo->in_color_space == JCS_CMYK) {
     for (col = cinfo->image_width; col > 0; col--) {
       t = GETJSAMPLE(*inptr++);
      if (t >= cmaplen)
        ERREXIT(cinfo, JERR_BMP_OUTOFRANGE);
       rgb_to_cmyk(colormap[0][t], colormap[1][t], colormap[2][t], outptr,
                   outptr + 1, outptr + 2, outptr + 3);
       outptr += 4;
    }
  } else {
    register int rindex = rgb_red[cinfo->in_color_space];
    register int gindex = rgb_green[cinfo->in_color_space];
    register int bindex = rgb_blue[cinfo->in_color_space];
    register int aindex = alpha_index[cinfo->in_color_space];
    register int ps = rgb_pixelsize[cinfo->in_color_space];

     if (aindex >= 0) {
       for (col = cinfo->image_width; col > 0; col--) {
         t = GETJSAMPLE(*inptr++);
        if (t >= cmaplen)
          ERREXIT(cinfo, JERR_BMP_OUTOFRANGE);
         outptr[rindex] = colormap[0][t];
         outptr[gindex] = colormap[1][t];
         outptr[bindex] = colormap[2][t];
        outptr[aindex] = 0xFF;
        outptr += ps;
      }
     } else {
       for (col = cinfo->image_width; col > 0; col--) {
         t = GETJSAMPLE(*inptr++);
        if (t >= cmaplen)
          ERREXIT(cinfo, JERR_BMP_OUTOFRANGE);
         outptr[rindex] = colormap[0][t];
         outptr[gindex] = colormap[1][t];
         outptr[bindex] = colormap[2][t];
        outptr += ps;
      }
    }
  }

  return 1;
}
