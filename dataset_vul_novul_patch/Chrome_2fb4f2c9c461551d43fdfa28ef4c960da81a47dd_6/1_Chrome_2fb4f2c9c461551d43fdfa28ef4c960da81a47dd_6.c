 bool ParseNewStyleVp9CodecID(const std::string& codec_id,
                              VideoCodecProfile* profile,
                              uint8_t* level_idc,
                             gfx::ColorSpace::TransferID* eotf) {
  *eotf = gfx::ColorSpace::TransferID::BT709;
 
   std::vector<std::string> fields = base::SplitString(
       codec_id, ".", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);

  if (fields.size() < 4 || fields.size() > 9) {
    DVLOG(3) << __func__ << " Invalid number of fields (" << fields.size()
             << ")";
    return false;
  }

  if (fields[0] != "vp09") {
    DVLOG(3) << __func__ << " Invalid 4CC (" << fields[0] << ")";
    return false;
  }

  std::vector<int> values;
  for (size_t i = 1; i < fields.size(); ++i) {
    if (fields[i] == "") {
      DVLOG(3) << __func__ << " Invalid missing field (position:" << i << ")";
      return false;
    }
    int value;
    if (!base::StringToInt(fields[i], &value) || value < 0) {
      DVLOG(3) << __func__ << " Invalid field value (" << value << ")";
      return false;
    }
    values.push_back(value);
  }

  const int profile_idc = values[0];
  switch (profile_idc) {
    case 0:
      *profile = VP9PROFILE_PROFILE0;
      break;
    case 1:
      *profile = VP9PROFILE_PROFILE1;
      break;
    case 2:
      *profile = VP9PROFILE_PROFILE2;
      break;
    case 3:
      *profile = VP9PROFILE_PROFILE3;
      break;
    default:
      DVLOG(3) << __func__ << " Invalid profile (" << profile_idc << ")";
      return false;
  }

  *level_idc = values[1];
  switch (*level_idc) {
    case 1:
    case 11:
    case 2:
    case 21:
    case 3:
    case 31:
    case 4:
    case 41:
    case 5:
    case 51:
    case 52:
    case 6:
    case 61:
    case 62:
      break;
    default:
      DVLOG(3) << __func__ << " Invalid level (" << *level_idc << ")";
      return false;
  }

  const int bit_depth = values[2];
  if (bit_depth != 8 && bit_depth != 10 && bit_depth != 12) {
    DVLOG(3) << __func__ << " Invalid bit-depth (" << bit_depth << ")";
    return false;
  }
 
   if (values.size() < 4)
     return true;
  const int color_primaries = values[3];
  if (color_primaries == 0 || color_primaries == 3 || color_primaries > 22) {
    DVLOG(3) << __func__ << " Invalid color primaries (" << color_primaries
             << ")";
     return false;
   }
 
   if (values.size() < 5)
     return true;
  if (!ParseVp9Eotf(values[4], eotf)) {
     DVLOG(3) << __func__ << " Invalid transfer function (" << values[4] << ")";
     return false;
   }
 
   if (values.size() < 6)
     return true;
  const int matrix_coefficients = values[5];
  if (matrix_coefficients > 11) {
    DVLOG(3) << __func__ << " Invalid matrix coefficients ("
             << matrix_coefficients << ")";
     return false;
   }
 
  if (values.size() < 7)
    return true;
  const int video_full_range_flag = values[6];
  if (video_full_range_flag > 1) {
    DVLOG(3) << __func__ << " Invalid full range flag ("
              << video_full_range_flag << ")";
     return false;
   }
 
   if (values.size() < 8)
     return true;
   const int chroma_subsampling = values[7];
   if (chroma_subsampling > 3 ||
      (chroma_subsampling != 3 && matrix_coefficients == 0)) {
     DVLOG(3) << __func__ << " Invalid chroma subsampling ("
              << chroma_subsampling << ")";
     return false;
  }

  return true;
}
