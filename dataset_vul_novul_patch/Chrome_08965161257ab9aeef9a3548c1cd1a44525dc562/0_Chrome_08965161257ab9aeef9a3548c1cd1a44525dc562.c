 std::wstring GetSwitchValueFromCommandLine(const std::wstring& command_line,
                                            const std::wstring& switch_name) {
  static constexpr wchar_t kSwitchTerminator[] = L"--";
   assert(!command_line.empty());
   assert(!switch_name.empty());
 
   std::vector<std::wstring> as_array = TokenizeCommandLineToArray(command_line);
   std::wstring switch_with_equal = L"--" + switch_name + L"=";
  auto end = std::find(as_array.cbegin(), as_array.cend(), kSwitchTerminator);
  for (auto scan = as_array.cbegin(); scan != end; ++scan) {
    const std::wstring& arg = *scan;
     if (arg.compare(0, switch_with_equal.size(), switch_with_equal) == 0)
       return arg.substr(switch_with_equal.size());
   }

  return std::wstring();
}
