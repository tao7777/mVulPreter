 bool IsHighDPIEnabled() {
  // Flag stored in HKEY_CURRENT_USER\SOFTWARE\\Google\\Chrome\\Profile,
  // under the DWORD value high-dpi-support.
  static DWORD value = ReadRegistryValue(
      HKEY_CURRENT_USER, kRegistryProfilePath,
      kHighDPISupportW, FALSE);
  return force_highdpi_for_testing || (value == 1);
 }
