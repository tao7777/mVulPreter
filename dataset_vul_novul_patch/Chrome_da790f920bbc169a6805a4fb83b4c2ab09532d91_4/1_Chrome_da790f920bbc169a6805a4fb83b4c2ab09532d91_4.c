extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size > 512)
    return 0;
 
   net::ProxyBypassRules rules;
   std::string input(data, data + size);
  rules.ParseFromString(input);
  rules.ParseFromStringUsingSuffixMatching(input);
   return 0;
 }
