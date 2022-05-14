bool BrowserInit::LaunchWithProfile::OpenApplicationURL(Profile* profile) {
   if (!command_line_.HasSwitch(switches::kApp))
     return false;
 
  GURL url(command_line_.GetSwitchValueASCII(switches::kApp));
   if (!url.is_empty() && url.is_valid()) {
     Browser::OpenApplicationWindow(profile, url);
     return true;
  }
  return false;
}
