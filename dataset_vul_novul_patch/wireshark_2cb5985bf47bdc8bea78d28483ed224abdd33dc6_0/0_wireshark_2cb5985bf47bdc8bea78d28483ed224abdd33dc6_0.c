dissect_ac_if_hdr_body(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_,
        proto_tree *tree, usb_conv_info_t *usb_conv_info)
{
    gint     offset_start;
    guint16  bcdADC;
    guint8   ver_major;
    double   ver;
    guint8   if_in_collection, i;
    audio_conv_info_t *audio_conv_info;


    offset_start = offset;

    bcdADC = tvb_get_letohs(tvb, offset);
    ver_major = USB_AUDIO_BCD44_TO_DEC(bcdADC>>8);
    ver = ver_major + USB_AUDIO_BCD44_TO_DEC(bcdADC&0xFF) / 100.0;

    proto_tree_add_double_format_value(tree, hf_ac_if_hdr_ver,
            tvb, offset, 2, ver, "%2.2f", ver);
    audio_conv_info = (audio_conv_info_t *)usb_conv_info->class_data;
     if(!audio_conv_info) {
         audio_conv_info = wmem_new(wmem_file_scope(), audio_conv_info_t);
         usb_conv_info->class_data = audio_conv_info;
        usb_conv_info->class_data_type = USB_CONV_AUDIO;
         /* XXX - set reasonable default values for all components
            that are not filled in by this function */
    } else if (usb_conv_info->class_data_type != USB_CONV_AUDIO) {
        /* Don't dissect if another USB type is in the conversation */
        return 0;
     }
     audio_conv_info->ver_major = ver_major;
     offset += 2;

    /* version 1 refers to the Basic Audio Device specification,
       version 2 is the Audio Device class specification, see above */
    if (ver_major==1) {
        proto_tree_add_item(tree, hf_ac_if_hdr_total_len,
                tvb, offset, 2, ENC_LITTLE_ENDIAN);
        offset += 2;
        if_in_collection = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(tree, hf_ac_if_hdr_bInCollection,
                tvb, offset, 1, ENC_LITTLE_ENDIAN);
        offset++;

        for (i=0; i<if_in_collection; i++) {
            proto_tree_add_item(tree, hf_ac_if_hdr_if_num,
                    tvb, offset, 1, ENC_LITTLE_ENDIAN);
            offset++;
        }
    }

    return offset-offset_start;
}
