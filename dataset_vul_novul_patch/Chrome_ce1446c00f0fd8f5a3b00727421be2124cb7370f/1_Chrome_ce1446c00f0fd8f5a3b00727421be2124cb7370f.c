 bool XmlReader::Load(const std::string& input) {
   const int kParseOptions = XML_PARSE_RECOVER |  // recover on errors
                            XML_PARSE_NONET |    // forbid network access
                            XML_PARSE_NOXXE;     // no external entities
   reader_ = xmlReaderForMemory(input.data(), static_cast<int>(input.size()),
                               NULL, NULL, kParseOptions);
  return reader_ != NULL;
}
