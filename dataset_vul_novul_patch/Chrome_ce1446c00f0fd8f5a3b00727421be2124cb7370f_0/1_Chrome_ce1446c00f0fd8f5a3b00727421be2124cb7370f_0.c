 bool XmlReader::LoadFile(const std::string& file_path) {
   const int kParseOptions = XML_PARSE_RECOVER |  // recover on errors
                            XML_PARSE_NONET |    // forbid network access
                            XML_PARSE_NOXXE;     // no external entities
   reader_ = xmlReaderForFile(file_path.c_str(), NULL, kParseOptions);
   return reader_ != NULL;
 }
