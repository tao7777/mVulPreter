int ShellBrowserMain(const content::MainFunctionParams& parameters) {
  bool layout_test_mode =
      CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree);
  base::ScopedTempDir browser_context_path_for_layout_tests;

  if (layout_test_mode) {
    CHECK(browser_context_path_for_layout_tests.CreateUniqueTempDir());
    CHECK(!browser_context_path_for_layout_tests.path().MaybeAsASCII().empty());
    CommandLine::ForCurrentProcess()->AppendSwitchASCII(
        switches::kContentShellDataPath,
        browser_context_path_for_layout_tests.path().MaybeAsASCII());
  }

  scoped_ptr<content::BrowserMainRunner> main_runner_(
      content::BrowserMainRunner::Create());

  int exit_code = main_runner_->Initialize(parameters);

  if (exit_code >= 0)
    return exit_code;

  if (CommandLine::ForCurrentProcess()->HasSwitch(
        switches::kCheckLayoutTestSysDeps)) {
    MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
    main_runner_->Run();
    main_runner_->Shutdown();
    return 0;
  }

  if (layout_test_mode) {
    content::WebKitTestController test_controller;
    std::string test_string;
    CommandLine::StringVector args =
        CommandLine::ForCurrentProcess()->GetArgs();
    size_t command_line_position = 0;
    bool ran_at_least_once = false;

#if defined(OS_ANDROID)
    std::cout << "#READY\n";
     std::cout.flush();
 #endif
 
    FilePath original_cwd;
    {
      // We're outside of the message loop here, and this is a test.
      base::ThreadRestrictions::ScopedAllowIO allow_io;
      file_util::GetCurrentDirectory(&original_cwd);
    }

     while (GetNextTest(args, &command_line_position, &test_string)) {
       if (test_string.empty())
         continue;
      if (test_string == "QUIT")
        break;

      bool enable_pixel_dumps;
      std::string pixel_hash;
      FilePath cwd;
      GURL test_url = GetURLForLayoutTest(
          test_string, &cwd, &enable_pixel_dumps, &pixel_hash);
      if (!content::WebKitTestController::Get()->PrepareForLayoutTest(
              test_url, cwd, enable_pixel_dumps, pixel_hash)) {
        break;
      }

       ran_at_least_once = true;
       main_runner_->Run();
 
      {
        // We're outside of the message loop here, and this is a test.
        base::ThreadRestrictions::ScopedAllowIO allow_io;
        file_util::SetCurrentDirectory(original_cwd);
      }

       if (!content::WebKitTestController::Get()->ResetAfterLayoutTest())
         break;
     }
    if (!ran_at_least_once) {
      MessageLoop::current()->PostTask(FROM_HERE, MessageLoop::QuitClosure());
      main_runner_->Run();
    }
    exit_code = 0;
  } else {
    exit_code = main_runner_->Run();
  }

  main_runner_->Shutdown();

  return exit_code;
}
