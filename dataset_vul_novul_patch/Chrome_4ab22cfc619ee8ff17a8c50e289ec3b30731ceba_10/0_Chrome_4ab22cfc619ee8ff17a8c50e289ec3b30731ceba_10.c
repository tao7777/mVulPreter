int main(int argc, char *argv[]) {
  struct mg_context *ctx;
  base::AtExitManager exit;
  base::WaitableEvent shutdown_event(false, false);
  CommandLine::Init(argc, argv);
  CommandLine* cmd_line = CommandLine::ForCurrentProcess();

#if defined(OS_POSIX)
  signal(SIGPIPE, SIG_IGN);
#endif
  srand((unsigned int)time(NULL));

   chrome::RegisterPathProvider();
   TestTimeouts::Initialize();
 
   std::string port = "9515";
   std::string root;
   std::string url_base;
  bool verbose = false;
   if (cmd_line->HasSwitch("port"))
     port = cmd_line->GetSwitchValueASCII("port");
  if (cmd_line->HasSwitch("root"))
     root = cmd_line->GetSwitchValueASCII("root");
   if (cmd_line->HasSwitch("url-base"))
     url_base = cmd_line->GetSwitchValueASCII("url-base");
  // Whether or not to do verbose logging.
  if (cmd_line->HasSwitch("verbose"))
    verbose = true;

  webdriver::InitWebDriverLogging(
      verbose ? logging::LOG_INFO : logging::LOG_WARNING);
 
   webdriver::SessionManager* manager = webdriver::SessionManager::GetInstance();
   manager->set_port(port);
  manager->set_url_base(url_base);

  ctx = mg_start();
  if (!SetMongooseOptions(ctx, port, root)) {
    mg_stop(ctx);
#if defined(OS_WIN)
    return WSAEADDRINUSE;
#else
    return EADDRINUSE;
#endif
  }

  webdriver::Dispatcher dispatcher(ctx, url_base);
  webdriver::InitCallbacks(ctx, &dispatcher, &shutdown_event, root.empty());

  std::cout << "Started ChromeDriver" << std::endl
            << "port=" << port << std::endl;

  if (root.length()) {
    VLOG(1) << "Serving files from the current working directory";
  }

  shutdown_event.Wait();

  mg_stop(ctx);
  return (EXIT_SUCCESS);
}
