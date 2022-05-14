vmnc_handle_wmvi_rectangle (GstVMncDec * dec, struct RfbRectangle *rect,
    const guint8 * data, int len, gboolean decode)
{
  GstVideoFormat format;
  gint bpp, tc;
  guint32 redmask, greenmask, bluemask;
  guint32 endianness, dataendianness;
  GstVideoCodecState *state;

  /* A WMVi rectangle has a 16byte payload */
  if (len < 16) {
    GST_DEBUG_OBJECT (dec, "Bad WMVi rect: too short");
    return ERROR_INSUFFICIENT_DATA;
  }

  /* We only compare 13 bytes; ignoring the 3 padding bytes at the end */
  if (dec->have_format && memcmp (data, dec->format.descriptor, 13) == 0) {
    /* Nothing changed, so just exit */
    return 16;
  }

  /* Store the whole block for simple comparison later */
  memcpy (dec->format.descriptor, data, 16);

  if (rect->x != 0 || rect->y != 0) {
    GST_WARNING_OBJECT (dec, "Bad WMVi rect: wrong coordinates");
    return ERROR_INVALID;
  }

  bpp = data[0];
  dec->format.depth = data[1];
  dec->format.big_endian = data[2];
  dataendianness = data[2] ? G_BIG_ENDIAN : G_LITTLE_ENDIAN;
  tc = data[3];

  if (bpp != 8 && bpp != 16 && bpp != 32) {
    GST_WARNING_OBJECT (dec, "Bad bpp value: %d", bpp);
    return ERROR_INVALID;
  }

  if (!tc) {
    GST_WARNING_OBJECT (dec, "Paletted video not supported");
    return ERROR_INVALID;
  }

  dec->format.bytes_per_pixel = bpp / 8;
  dec->format.width = rect->width;
  dec->format.height = rect->height;

  redmask = (guint32) (RFB_GET_UINT16 (data + 4)) << data[10];
  greenmask = (guint32) (RFB_GET_UINT16 (data + 6)) << data[11];
  bluemask = (guint32) (RFB_GET_UINT16 (data + 8)) << data[12];

  GST_DEBUG_OBJECT (dec, "Red: mask %d, shift %d",
      RFB_GET_UINT16 (data + 4), data[10]);
  GST_DEBUG_OBJECT (dec, "Green: mask %d, shift %d",
      RFB_GET_UINT16 (data + 6), data[11]);
  GST_DEBUG_OBJECT (dec, "Blue: mask %d, shift %d",
      RFB_GET_UINT16 (data + 8), data[12]);
  GST_DEBUG_OBJECT (dec, "BPP: %d. endianness: %s", bpp,
      data[2] ? "big" : "little");

  /* GStreamer's RGB caps are a bit weird. */
  if (bpp == 8) {
    endianness = G_BYTE_ORDER;  /* Doesn't matter */
  } else if (bpp == 16) {
    /* We require host-endian. */
    endianness = G_BYTE_ORDER;
  } else {                      /* bpp == 32 */
    /* We require big endian */
    endianness = G_BIG_ENDIAN;
    if (endianness != dataendianness) {
      redmask = GUINT32_SWAP_LE_BE (redmask);
      greenmask = GUINT32_SWAP_LE_BE (greenmask);
      bluemask = GUINT32_SWAP_LE_BE (bluemask);
    }
  }

  format = gst_video_format_from_masks (dec->format.depth, bpp, endianness,
      redmask, greenmask, bluemask, 0);

  GST_DEBUG_OBJECT (dec, "From depth: %d bpp: %u endianess: %s redmask: %X "
      "greenmask: %X bluemask: %X got format %s",
      dec->format.depth, bpp, endianness == G_BIG_ENDIAN ? "BE" : "LE",
      GUINT32_FROM_BE (redmask), GUINT32_FROM_BE (greenmask),
      GUINT32_FROM_BE (bluemask),
      format == GST_VIDEO_FORMAT_UNKNOWN ? "UNKOWN" :
      gst_video_format_to_string (format));

  if (format == GST_VIDEO_FORMAT_UNKNOWN) {
    GST_WARNING_OBJECT (dec, "Video format unknown to GStreamer");
    return ERROR_INVALID;
  }

  dec->have_format = TRUE;
  if (!decode) {
    GST_LOG_OBJECT (dec, "Parsing, not setting caps");
    return 16;
  }


  state = gst_video_decoder_set_output_state (GST_VIDEO_DECODER (dec), format,
      rect->width, rect->height, dec->input_state);
   gst_video_codec_state_unref (state);
 
   g_free (dec->imagedata);
  dec->imagedata = g_malloc (dec->format.width * dec->format.height *
       dec->format.bytes_per_pixel);
   GST_DEBUG_OBJECT (dec, "Allocated image data at %p", dec->imagedata);
 
  dec->format.stride = dec->format.width * dec->format.bytes_per_pixel;

  return 16;
}
