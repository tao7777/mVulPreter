void InitChromeDriverLogging(const CommandLine& command_line) {
  bool success = InitLogging(
      FILE_PATH_LITERAL("chromedriver.log"),
      logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG,
      logging::LOCK_LOG_FILE,
      logging::DELETE_OLD_LOG_FILE,
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS);
  if (!success) {
    PLOG(ERROR) << "Unable to initialize logging";
  }
  logging::SetLogItems(false,  // enable_process_id
                       false,  // enable_thread_id
                       true,   // enable_timestamp
                       false); // enable_tickcount
  if (command_line.HasSwitch(switches::kLoggingLevel)) {
    std::string log_level = command_line.GetSwitchValueASCII(
        switches::kLoggingLevel);
    int level = 0;
    if (base::StringToInt(log_level, &level)) {
      logging::SetMinLogLevel(level);
    } else {
      LOG(WARNING) << "Bad log level: " << log_level;
    }
  }
}
