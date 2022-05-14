void BrowserProcessMainImpl::Shutdown() {
  if (state_ != STATE_STARTED) {
    CHECK_NE(state_, STATE_SHUTTING_DOWN);
    return;
 
   MessagePump::Get()->Stop();
 
  WebContentsUnloader::GetInstance()->Shutdown();
  if (process_model_ != PROCESS_MODEL_SINGLE_PROCESS) {
    BrowserContext::AssertNoContextsExist();
  }
   browser_main_runner_->Shutdown();
   browser_main_runner_.reset();

  if (process_model_ != PROCESS_MODEL_SINGLE_PROCESS) {
    BrowserContext::AssertNoContextsExist();
  }

  browser_main_runner_->Shutdown();
  browser_main_runner_.reset();

  exit_manager_.reset();

  main_delegate_.reset();
  platform_delegate_.reset();

  state_ = STATE_SHUTDOWN;
}

BrowserProcessMain::BrowserProcessMain() {}

BrowserProcessMain::~BrowserProcessMain() {}

ProcessModel BrowserProcessMain::GetProcessModelOverrideFromEnv() {
  static bool g_initialized = false;
  static ProcessModel g_process_model = PROCESS_MODEL_UNDEFINED;

  if (g_initialized) {
    return g_process_model;
  }

  g_initialized = true;

  std::unique_ptr<base::Environment> env = base::Environment::Create();

  if (IsEnvironmentOptionEnabled("SINGLE_PROCESS", env.get())) {
    g_process_model = PROCESS_MODEL_SINGLE_PROCESS;
  } else {
    std::string model = GetEnvironmentOption("PROCESS_MODEL", env.get());
    if (!model.empty()) {
      if (model == "multi-process") {
        g_process_model = PROCESS_MODEL_MULTI_PROCESS;
      } else if (model == "single-process") {
        g_process_model = PROCESS_MODEL_SINGLE_PROCESS;
      } else if (model == "process-per-site-instance") {
        g_process_model = PROCESS_MODEL_PROCESS_PER_SITE_INSTANCE;
      } else if (model == "process-per-view") {
        g_process_model = PROCESS_MODEL_PROCESS_PER_VIEW;
      } else if (model == "process-per-site") {
        g_process_model = PROCESS_MODEL_PROCESS_PER_SITE;
      } else if (model == "site-per-process") {
        g_process_model = PROCESS_MODEL_SITE_PER_PROCESS;
      } else {
        LOG(WARNING) << "Invalid process mode: " << model;
      }
    }
  }

  return g_process_model;
}

BrowserProcessMain* BrowserProcessMain::GetInstance() {
  static BrowserProcessMainImpl g_instance;
  return &g_instance;
}

} // namespace oxide
