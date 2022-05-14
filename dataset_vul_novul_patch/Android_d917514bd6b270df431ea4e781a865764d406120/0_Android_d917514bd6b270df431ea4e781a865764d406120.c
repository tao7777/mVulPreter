static bool get_build_id(
 Backtrace* backtrace, uintptr_t base_addr, uint8_t* e_ident, std::string* build_id) {
 HdrType hdr;

  memcpy(&hdr.e_ident[0], e_ident, EI_NIDENT);

 if (backtrace->Read(base_addr + EI_NIDENT, reinterpret_cast<uint8_t*>(&hdr) + EI_NIDENT,
 sizeof(HdrType) - EI_NIDENT) != sizeof(HdrType) - EI_NIDENT) {
 return false;
 }

 for (size_t i = 0; i < hdr.e_phnum; i++) {
 PhdrType phdr;
 if (backtrace->Read(base_addr + hdr.e_phoff + i * hdr.e_phentsize,
 reinterpret_cast<uint8_t*>(&phdr), sizeof(phdr)) != sizeof(phdr)) {
 return false;
 }
 if (phdr.p_type == PT_NOTE) {
 size_t hdr_size = phdr.p_filesz;
 uintptr_t addr = base_addr + phdr.p_offset;
 while (hdr_size >= sizeof(NhdrType)) {
 NhdrType nhdr;
 if (backtrace->Read(addr, reinterpret_cast<uint8_t*>(&nhdr), sizeof(nhdr)) != sizeof(nhdr)) {
 return false;
 }
        addr += sizeof(nhdr);

         if (nhdr.n_type == NT_GNU_BUILD_ID) {
           addr += NOTE_ALIGN(nhdr.n_namesz);
          uint8_t build_id_data[160];
          if (nhdr.n_descsz > sizeof(build_id_data)) {
            ALOGE("Possible corrupted note, desc size value is too large: %u",
                  nhdr.n_descsz);
             return false;
           }
           if (backtrace->Read(addr, build_id_data, nhdr.n_descsz) != nhdr.n_descsz) {
 return false;
 }

          build_id->clear();
 for (size_t bytes = 0; bytes < nhdr.n_descsz; bytes++) {
 *build_id += android::base::StringPrintf("%02x", build_id_data[bytes]);
 }

 return true;
 } else {
          hdr_size -= sizeof(nhdr);
 size_t skip_bytes = NOTE_ALIGN(nhdr.n_namesz) + NOTE_ALIGN(nhdr.n_descsz);
          addr += skip_bytes;
 if (hdr_size < skip_bytes) {
 break;
 }
          hdr_size -= skip_bytes;
 }
 }
 }
 }
 return false;
}
