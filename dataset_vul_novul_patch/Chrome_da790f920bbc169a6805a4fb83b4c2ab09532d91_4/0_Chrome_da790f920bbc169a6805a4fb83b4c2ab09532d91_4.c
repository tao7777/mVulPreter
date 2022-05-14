extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size > 512)
    return 0;
 
   net::ProxyBypassRules rules;
   std::string input(data, data + size);

  const net::ProxyBypassRules::ParseFormat kFormats[] = {
      net::ProxyBypassRules::ParseFormat::kDefault,
      net::ProxyBypassRules::ParseFormat::kHostnameSuffixMatching,
  };

  for (auto format : kFormats)
    rules.ParseFromString(input, format);

   return 0;
 }
