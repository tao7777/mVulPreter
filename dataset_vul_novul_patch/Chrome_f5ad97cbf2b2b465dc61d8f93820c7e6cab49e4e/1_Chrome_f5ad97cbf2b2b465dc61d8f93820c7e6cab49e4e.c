 void SetUpFontconfig() {
  std::unique_ptr<Environment> env = Environment::Create();
  if (!env->HasVar("FONTCONFIG_FILE")) {
    FilePath dir_module;
    PathService::Get(DIR_MODULE, &dir_module);
    FilePath font_cache = dir_module.Append("fontconfig_caches");
    FilePath test_fonts = dir_module.Append("test_fonts");
    std::string fonts_conf = ReplaceStringPlaceholders(
        kFontsConfTemplate, {font_cache.value(), test_fonts.value()}, nullptr);
    FilePath fonts_conf_file_temp;
    CHECK(CreateTemporaryFileInDir(dir_module, &fonts_conf_file_temp));
    CHECK(
        WriteFile(fonts_conf_file_temp, fonts_conf.c_str(), fonts_conf.size()));
    FilePath fonts_conf_file = dir_module.Append("fonts.conf");
    CHECK(ReplaceFile(fonts_conf_file_temp, fonts_conf_file, nullptr));
    env->SetVar("FONTCONFIG_FILE", fonts_conf_file.value());
  }
  CHECK(FcInit());
 }
