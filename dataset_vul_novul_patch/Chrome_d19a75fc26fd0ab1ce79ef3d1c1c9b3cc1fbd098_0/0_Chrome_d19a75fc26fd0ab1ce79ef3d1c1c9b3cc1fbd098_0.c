 base::string16 GetAppForProtocolUsingRegistry(const GURL& url) {
  const base::string16 url_scheme = base::ASCIIToUTF16(url.scheme());
  if (!IsValidCustomProtocol(url_scheme))
    return base::string16();
 
  base::string16 command_to_launch;
  base::win::RegKey cmd_key_name(HKEY_CLASSES_ROOT, url_scheme.c_str(),
                                  KEY_READ);
   if (cmd_key_name.ReadValue(NULL, &command_to_launch) == ERROR_SUCCESS &&
       !command_to_launch.empty()) {
    return command_to_launch;
  }
 
  const base::string16 cmd_key_path = url_scheme + L"\\shell\\open\\command";
   base::win::RegKey cmd_key_exe(HKEY_CLASSES_ROOT, cmd_key_path.c_str(),
                                 KEY_READ);
   if (cmd_key_exe.ReadValue(NULL, &command_to_launch) == ERROR_SUCCESS) {
    base::CommandLine command_line(
        base::CommandLine::FromString(command_to_launch));
    return command_line.GetProgram().BaseName().value();
  }

  return base::string16();
}
