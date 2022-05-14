ProfilingProcessHost::Mode ProfilingProcessHost::GetCurrentMode() {
  const base::CommandLine* cmdline = base::CommandLine::ForCurrentProcess();
#if BUILDFLAG(USE_ALLOCATOR_SHIM)
  if (cmdline->HasSwitch(switches::kMemlog) ||
      base::FeatureList::IsEnabled(kOOPHeapProfilingFeature)) {
    if (cmdline->HasSwitch(switches::kEnableHeapProfiling)) {
      LOG(ERROR) << "--" << switches::kEnableHeapProfiling
                 << " specified with --" << switches::kMemlog
                 << "which are not compatible. Memlog will be disabled.";
      return Mode::kNone;
    }

    std::string mode;
    if (cmdline->HasSwitch(switches::kMemlog)) {
      mode = cmdline->GetSwitchValueASCII(switches::kMemlog);
    } else {
      mode = base::GetFieldTrialParamValueByFeature(
           kOOPHeapProfilingFeature, kOOPHeapProfilingFeatureMode);
     }
 
    if (mode == switches::kMemlogModeAll)
      return Mode::kAll;
    if (mode == switches::kMemlogModeMinimal)
      return Mode::kMinimal;
    if (mode == switches::kMemlogModeBrowser)
      return Mode::kBrowser;
    if (mode == switches::kMemlogModeGpu)
      return Mode::kGpu;
    if (mode == switches::kMemlogModeRendererSampling)
      return Mode::kRendererSampling;
    DLOG(ERROR) << "Unsupported value: \"" << mode << "\" passed to --"
                << switches::kMemlog;
   }
   return Mode::kNone;
 #else
  LOG_IF(ERROR, cmdline->HasSwitch(switches::kMemlog))
      << "--" << switches::kMemlog
      << " specified but it will have no effect because the use_allocator_shim "
      << "is not available in this build.";
  return Mode::kNone;
 #endif
 }
