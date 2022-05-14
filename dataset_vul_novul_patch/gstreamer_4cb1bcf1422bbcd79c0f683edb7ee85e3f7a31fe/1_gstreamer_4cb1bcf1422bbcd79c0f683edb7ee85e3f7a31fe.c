vmnc_handle_packet (GstVMncDec * dec, const guint8 * data, int len,
    gboolean decode)
{
  int type;
  int offset = 0;

  if (len < 4) {
    GST_LOG_OBJECT (dec, "Packet too short");
    return ERROR_INSUFFICIENT_DATA;
  }

  type = data[0];

  switch (type) {
    case 0:
    {
      int numrect = RFB_GET_UINT16 (data + 2);
      int i;
      int read;

      offset = 4;

      for (i = 0; i < numrect; i++) {
        struct RfbRectangle r;
        rectangle_handler handler;

        if (len < offset + 12) {
          GST_LOG_OBJECT (dec,
              "Packet too short for rectangle header: %d < %d",
              len, offset + 12);
          return ERROR_INSUFFICIENT_DATA;
        }
        GST_LOG_OBJECT (dec, "Reading rectangle %d", i);
        r.x = RFB_GET_UINT16 (data + offset);
        r.y = RFB_GET_UINT16 (data + offset + 2);
        r.width = RFB_GET_UINT16 (data + offset + 4);
        r.height = RFB_GET_UINT16 (data + offset + 6);
        r.type = RFB_GET_UINT32 (data + offset + 8);

        if (r.type != TYPE_WMVi) {
          /* We must have a WMVi packet to initialise things before we can 
           * continue */
          if (!dec->have_format) {
            GST_WARNING_OBJECT (dec, "Received packet without WMVi: %d",
                r.type);
            return ERROR_INVALID;
          }
          if (r.x + r.width > dec->format.width ||
              r.y + r.height > dec->format.height) {
             GST_WARNING_OBJECT (dec, "Rectangle out of range, type %d", r.type);
             return ERROR_INVALID;
           }
         }
 
         switch (r.type) {
            handler = vmnc_handle_wmve_rectangle;
            break;
          case TYPE_WMVf:
            handler = vmnc_handle_wmvf_rectangle;
            break;
          case TYPE_WMVg:
            handler = vmnc_handle_wmvg_rectangle;
            break;
          case TYPE_WMVh:
            handler = vmnc_handle_wmvh_rectangle;
            break;
          case TYPE_WMVi:
            handler = vmnc_handle_wmvi_rectangle;
            break;
          case TYPE_WMVj:
            handler = vmnc_handle_wmvj_rectangle;
            break;
          case TYPE_RAW:
            handler = vmnc_handle_raw_rectangle;
            break;
          case TYPE_COPY:
            handler = vmnc_handle_copy_rectangle;
            break;
          case TYPE_HEXTILE:
            handler = vmnc_handle_hextile_rectangle;
            break;
          default:
            GST_WARNING_OBJECT (dec, "Unknown rectangle type");
            return ERROR_INVALID;
        }

        read = handler (dec, &r, data + offset + 12, len - offset - 12, decode);
        if (read < 0) {
          GST_DEBUG_OBJECT (dec, "Error calling rectangle handler\n");
          return read;
        }
        offset += 12 + read;
      }
      break;
    }
    default:
      GST_WARNING_OBJECT (dec, "Packet type unknown: %d", type);
      return ERROR_INVALID;
  }

  return offset;
}
