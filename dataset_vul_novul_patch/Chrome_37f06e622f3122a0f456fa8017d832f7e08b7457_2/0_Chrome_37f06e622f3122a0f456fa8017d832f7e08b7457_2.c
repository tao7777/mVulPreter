void HpackDecoder::HandleHeaderRepresentation(StringPiece name,
bool HpackDecoder::HandleHeaderRepresentation(StringPiece name,
                                               StringPiece value) {
   typedef std::pair<std::map<string, string>::iterator, bool> InsertResult;
 
  // Fail if pseudo-header follows regular header.
  if (name.size() > 0) {
    if (name[0] == kPseudoHeaderPrefix) {
      if (regular_header_seen_) return false;
    } else {
      regular_header_seen_ = true;
    }
  }

   if (name == kCookieKey) {
     if (cookie_value_.empty()) {
       cookie_value_.assign(value.data(), value.size());
    } else {
      cookie_value_ += "; ";
      cookie_value_.insert(cookie_value_.end(), value.begin(), value.end());
    }
  } else {
    InsertResult result = decoded_block_.insert(
        std::make_pair(name.as_string(), value.as_string()));
    if (!result.second) {
      result.first->second.push_back('\0');
      result.first->second.insert(result.first->second.end(),
                                  value.begin(),
                                   value.end());
     }
   }
  return true;
 }
