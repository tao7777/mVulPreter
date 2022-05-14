int BrowserMainLoop::PreCreateThreads() {

  if (parts_) {
    TRACE_EVENT0("startup",
        "BrowserMainLoop::CreateThreads:PreCreateThreads");

    result_code_ = parts_->PreCreateThreads();
  }

  if (!base::SequencedWorkerPool::IsEnabled())
    base::SequencedWorkerPool::EnableForProcess();

  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  base::FeatureList::InitializeInstance(
      command_line->GetSwitchValueASCII(switches::kEnableFeatures),
      command_line->GetSwitchValueASCII(switches::kDisableFeatures));

  InitializeMemoryManagementComponent();

#if defined(OS_MACOSX)
  if (base::CommandLine::InitializedForCurrentProcess() &&
      base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableHeapProfiling)) {
    base::allocator::PeriodicallyShimNewMallocZones();
  }
#endif

#if BUILDFLAG(ENABLE_PLUGINS)
  {
    TRACE_EVENT0("startup", "BrowserMainLoop::CreateThreads:PluginService");
    PluginService::GetInstance()->Init();
  }
#endif

#if BUILDFLAG(ENABLE_LIBRARY_CDMS)
  CdmRegistry::GetInstance()->Init();
#endif

#if defined(OS_MACOSX)
  ui::WindowResizeHelperMac::Get()->Init(base::ThreadTaskRunnerHandle::Get());
#endif

  GpuDataManagerImpl* gpu_data_manager = GpuDataManagerImpl::GetInstance();

#if defined(USE_X11)
  gpu_data_manager_visual_proxy_.reset(
      new internal::GpuDataManagerVisualProxy(gpu_data_manager));
#endif

  gpu_data_manager->Initialize();

#if !defined(GOOGLE_CHROME_BUILD) || defined(OS_ANDROID)
  if (parsed_command_line_.HasSwitch(switches::kSingleProcess))
    RenderProcessHost::SetRunRendererInProcess(true);
#endif

  std::vector<url::Origin> origins =
      GetContentClient()->browser()->GetOriginsRequiringDedicatedProcess();
  ChildProcessSecurityPolicyImpl* policy =
      ChildProcessSecurityPolicyImpl::GetInstance();
   for (auto origin : origins)
     policy->AddIsolatedOrigin(origin);
 
  EVP_set_buggy_rsa_parser(
      base::FeatureList::IsEnabled(features::kBuggyRSAParser));
   return result_code_;
 }
