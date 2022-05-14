int PpapiPluginMain(const content::MainFunctionParams& parameters) {
  const CommandLine& command_line = parameters.command_line;

#if defined(OS_WIN)
  g_target_services = parameters.sandbox_info->target_services;
#endif

  if (command_line.HasSwitch(switches::kPpapiStartupDialog)) {
    if (g_target_services)
      base::debug::WaitForDebugger(2*60, false);
    else
       ChildProcess::WaitForDebugger("Ppapi");
   }
 
  // Set the default locale to be the current UI language. WebKit uses ICU's
  // default locale for some font settings (especially switching between
  // Japanese and Chinese fonts for the same characters).
  if (command_line.HasSwitch(switches::kLang)) {
    std::string locale = command_line.GetSwitchValueASCII(switches::kLang);
    base::i18n::SetICUDefaultLocale(locale);
  }

   MessageLoop main_message_loop;
   base::PlatformThread::SetName("CrPPAPIMain");
 
#if defined(OS_LINUX)
  content::InitializeSandbox();
#endif

  ChildProcess ppapi_process;
  ppapi_process.set_main_thread(
      new PpapiThread(parameters.command_line, false));  // Not a broker.

  main_message_loop.Run();
  return 0;
}
