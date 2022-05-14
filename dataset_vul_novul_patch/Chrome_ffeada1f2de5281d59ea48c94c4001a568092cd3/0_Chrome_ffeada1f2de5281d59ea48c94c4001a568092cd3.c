bool SandboxedExtensionUnpacker::ValidateSignature() {
  ScopedStdioHandle file(file_util::OpenFile(crx_path_, "rb"));
  if (!file.get()) {
    ReportFailure("Could not open crx file for reading");
    return false;
  }

  ExtensionHeader header;
  size_t len;

  len = fread(&header, 1, sizeof(ExtensionHeader),
      file.get());
  if (len < sizeof(ExtensionHeader)) {
    ReportFailure("Invalid crx header");
    return false;
  }
  if (strncmp(kExtensionHeaderMagic, header.magic,
      sizeof(header.magic))) {
    ReportFailure("Bad magic number");
    return false;
  }
  if (header.version != kCurrentVersion) {
    ReportFailure("Bad version number");
    return false;
  }
  if (header.key_size > kMaxPublicKeySize ||
      header.signature_size > kMaxSignatureSize) {
    ReportFailure("Excessively large key or signature");
    return false;
  }
  if (header.key_size == 0) {
     ReportFailure("Key length is zero");
     return false;
   }
  if (header.signature_size == 0) {
    ReportFailure("Signature length is zero");
    return false;
  }
 
   std::vector<uint8> key;
   key.resize(header.key_size);
  len = fread(&key.front(), sizeof(uint8), header.key_size, file.get());
  if (len < header.key_size) {
    ReportFailure("Invalid public key");
    return false;
  }

  std::vector<uint8> signature;
  signature.resize(header.signature_size);
  len = fread(&signature.front(), sizeof(uint8), header.signature_size,
      file.get());
  if (len < header.signature_size) {
    ReportFailure("Invalid signature");
    return false;
  }

  base::SignatureVerifier verifier;
  if (!verifier.VerifyInit(extension_misc::kSignatureAlgorithm,
                           sizeof(extension_misc::kSignatureAlgorithm),
                           &signature.front(),
                           signature.size(),
                           &key.front(),
                           key.size())) {
    ReportFailure("Signature verification initialization failed. "
                  "This is most likely caused by a public key in "
                  "the wrong format (should encode algorithm).");
    return false;
  }

  unsigned char buf[1 << 12];
  while ((len = fread(buf, 1, sizeof(buf), file.get())) > 0)
    verifier.VerifyUpdate(buf, len);

  if (!verifier.VerifyFinal()) {
    ReportFailure("Signature verification failed");
    return false;
  }

  base::Base64Encode(std::string(reinterpret_cast<char*>(&key.front()),
      key.size()), &public_key_);
  return true;
}
