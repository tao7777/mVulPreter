 base::string16 GetAppForProtocolUsingRegistry(const GURL& url) {
  base::string16 command_to_launch;
 
  base::string16 cmd_key_path = base::ASCIIToUTF16(url.scheme());
  base::win::RegKey cmd_key_name(HKEY_CLASSES_ROOT, cmd_key_path.c_str(),
                                  KEY_READ);
   if (cmd_key_name.ReadValue(NULL, &command_to_launch) == ERROR_SUCCESS &&
       !command_to_launch.empty()) {
    return command_to_launch;
  }
 
  cmd_key_path = base::ASCIIToUTF16(url.scheme() + "\\shell\\open\\command");
   base::win::RegKey cmd_key_exe(HKEY_CLASSES_ROOT, cmd_key_path.c_str(),
                                 KEY_READ);
   if (cmd_key_exe.ReadValue(NULL, &command_to_launch) == ERROR_SUCCESS) {
    base::CommandLine command_line(
        base::CommandLine::FromString(command_to_launch));
    return command_line.GetProgram().BaseName().value();
  }

  return base::string16();
}
