 std::wstring GetSwitchValueFromCommandLine(const std::wstring& command_line,
                                            const std::wstring& switch_name) {
   assert(!command_line.empty());
   assert(!switch_name.empty());
 
   std::vector<std::wstring> as_array = TokenizeCommandLineToArray(command_line);
   std::wstring switch_with_equal = L"--" + switch_name + L"=";
  for (size_t i = 1; i < as_array.size(); ++i) {
    const std::wstring& arg = as_array[i];
     if (arg.compare(0, switch_with_equal.size(), switch_with_equal) == 0)
       return arg.substr(switch_with_equal.size());
   }

  return std::wstring();
}
