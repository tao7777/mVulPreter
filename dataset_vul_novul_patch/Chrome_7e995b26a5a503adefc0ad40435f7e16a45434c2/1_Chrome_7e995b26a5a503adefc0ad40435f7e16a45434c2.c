int ChromeBrowserMainPartsChromeos::PreEarlyInitialization() {
  base::CommandLine* singleton_command_line =
      base::CommandLine::ForCurrentProcess();

  if (parsed_command_line().HasSwitch(switches::kGuestSession)) {
    singleton_command_line->AppendSwitch(::switches::kDisableSync);
    singleton_command_line->AppendSwitch(::switches::kDisableExtensions);
    browser_defaults::bookmarks_enabled = false;
  }

  if (!base::SysInfo::IsRunningOnChromeOS() &&
      !parsed_command_line().HasSwitch(switches::kLoginManager) &&
      !parsed_command_line().HasSwitch(switches::kLoginUser) &&
      !parsed_command_line().HasSwitch(switches::kGuestSession)) {
    singleton_command_line->AppendSwitchASCII(
        switches::kLoginUser,
        cryptohome::Identification(user_manager::StubAccountId()).id());
    if (!parsed_command_line().HasSwitch(switches::kLoginProfile)) {
      singleton_command_line->AppendSwitchASCII(switches::kLoginProfile,
                                                chrome::kTestUserProfileDir);
    }
    LOG(WARNING) << "Running as stub user with profile dir: "
                 << singleton_command_line
                        ->GetSwitchValuePath(switches::kLoginProfile)
                        .value();
  }

  RegisterStubPathOverridesIfNecessary();

#if defined(GOOGLE_CHROME_BUILD)
  const char kChromeOSReleaseTrack[] = "CHROMEOS_RELEASE_TRACK";
  std::string channel;
  if (base::SysInfo::GetLsbReleaseValue(kChromeOSReleaseTrack, &channel))
    chrome::SetChannel(channel);
#endif
 
   dbus_pre_early_init_ = std::make_unique<internal::DBusPreEarlyInit>();
 
   return ChromeBrowserMainPartsLinux::PreEarlyInitialization();
 }
