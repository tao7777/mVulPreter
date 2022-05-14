 void SetUpFontconfig() {
  FilePath dir_module;
  PathService::Get(DIR_MODULE, &dir_module);
  FilePath font_cache = dir_module.Append("fontconfig_caches");
  FilePath test_fonts = dir_module.Append("test_fonts");
  std::string fonts_conf = ReplaceStringPlaceholders(
      kFontsConfTemplate, {font_cache.value(), test_fonts.value()}, nullptr);

  FcConfig* config = FcConfigCreate();
  CHECK(config);
#if FC_VERSION >= 21205
  CHECK(FcConfigParseAndLoadFromMemory(
      config, reinterpret_cast<const FcChar8*>(fonts_conf.c_str()), FcTrue));
#else
  FilePath temp;
  CHECK(CreateTemporaryFile(&temp));
  CHECK(WriteFile(temp, fonts_conf.c_str(), fonts_conf.size()));
  CHECK(FcConfigParseAndLoad(
      config, reinterpret_cast<const FcChar8*>(temp.value().c_str()), FcTrue));
  CHECK(DeleteFile(temp, false));
#endif
  CHECK(FcConfigBuildFonts(config));
  CHECK(FcConfigSetCurrent(config));

  // Decrement the reference count for |config|.  It's now owned by fontconfig.
  FcConfigDestroy(config);
 }
