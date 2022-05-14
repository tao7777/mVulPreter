void InProcessBrowserTest::PrepareTestCommandLine(CommandLine* command_line) {
  test_launcher_utils::PrepareBrowserCommandLineForTests(command_line);

  command_line->AppendSwitchASCII(switches::kTestType, kBrowserTestType);

#if defined(OS_WIN)
  if (command_line->HasSwitch(switches::kAshBrowserTests)) {
    command_line->AppendSwitchNative(switches::kViewerLaunchViaAppId,
                                     win8::test::kDefaultTestAppUserModelId);
    command_line->AppendSwitch(switches::kSilentLaunch);
  }
#endif

#if defined(OS_MACOSX)
  base::FilePath subprocess_path;
  PathService::Get(base::FILE_EXE, &subprocess_path);
  subprocess_path = subprocess_path.DirName().DirName();
  DCHECK_EQ(subprocess_path.BaseName().value(), "Contents");
  subprocess_path =
      subprocess_path.Append("Versions").Append(chrome::kChromeVersion);
  subprocess_path =
      subprocess_path.Append(chrome::kHelperProcessExecutablePath);
  command_line->AppendSwitchPath(switches::kBrowserSubprocessPath,
                                 subprocess_path);
#endif

   if (exit_when_last_browser_closes_)
     command_line->AppendSwitch(switches::kDisableZeroBrowsersOpenForTests);
 
  if (command_line->GetArgs().empty())
     command_line->AppendArg(url::kAboutBlankURL);
 }
