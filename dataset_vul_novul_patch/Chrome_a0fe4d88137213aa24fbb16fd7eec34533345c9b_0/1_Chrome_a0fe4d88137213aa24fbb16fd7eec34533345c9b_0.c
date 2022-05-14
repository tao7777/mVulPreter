 bool IsHighDPIEnabled() {
  if (CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kHighDPISupport)) {
    return CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
        switches::kHighDPISupport).compare("1") == 0;
  }
  return false;
 }
