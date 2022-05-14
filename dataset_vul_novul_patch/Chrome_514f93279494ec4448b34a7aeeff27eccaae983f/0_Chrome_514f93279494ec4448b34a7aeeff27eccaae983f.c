 bool DebugOnStart::FindArgument(wchar_t* command_line, const char* argument_c) {
  wchar_t argument[50] = {};
   for (int i = 0; argument_c[i]; ++i)
     argument[i] = argument_c[i];
 
  int argument_len = lstrlen(argument);
  int command_line_len = lstrlen(command_line);
  while (command_line_len > argument_len) {
    wchar_t first_char = command_line[0];
    wchar_t last_char = command_line[argument_len+1];
    if ((first_char == L'-' || first_char == L'/') &&
        (last_char == L' ' || last_char == 0 || last_char == L'=')) {
      command_line[argument_len+1] = 0;
      if (lstrcmpi(command_line+1, argument) == 0) {
        command_line[argument_len+1] = last_char;
        return true;
      }
      command_line[argument_len+1] = last_char;
    }
    ++command_line;
    --command_line_len;
  }
  return false;
}
