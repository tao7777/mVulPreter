CPError CPB_GetCommandLineArgumentsCommon(const char* url,
                                          std::string* arguments) {
  const CommandLine cmd = *CommandLine::ForCurrentProcess();
  std::wstring arguments_w;

  std::wstring user_data_dir = cmd.GetSwitchValue(switches::kUserDataDir);
  if (!user_data_dir.empty()) {
    if (file_util::AbsolutePath(&user_data_dir) &&
        file_util::PathExists(FilePath::FromWStringHack(user_data_dir))) {
      arguments_w += std::wstring(L"--") + ASCIIToWide(switches::kUserDataDir) +
                     L"=\"" + user_data_dir + L"\" ";
    }
  }

#if defined (OS_CHROMEOS)
  std::wstring profile = cmd.GetSwitchValue(switches::kProfile);
  if (!profile.empty()) {
    arguments_w += std::wstring(L"--") + ASCIIToWide(switches::kProfile) +
                   L"=\"" + profile + L"\" ";
  }
#endif

   std::string url_string(url);
  ReplaceSubstringsAfterOffset(&url_string, 0, "\\", "%5C");
  ReplaceSubstringsAfterOffset(&url_string, 0, "\"", "%22");
  ReplaceSubstringsAfterOffset(&url_string, 0, ";",  "%3B");
  ReplaceSubstringsAfterOffset(&url_string, 0, "$",  "%24");
#if defined(OS_WIN)  // Windows shortcuts can't escape % so we use \x instead.
  ReplaceSubstringsAfterOffset(&url_string, 0, "%",  "\\x");
#endif
   std::wstring url_w = UTF8ToWide(url_string);
   arguments_w += std::wstring(L"--") + ASCIIToWide(switches::kApp) +
      L"=\"" + url_w + L"\"";

  *arguments = WideToUTF8(arguments_w);

  return CPERR_SUCCESS;
}
