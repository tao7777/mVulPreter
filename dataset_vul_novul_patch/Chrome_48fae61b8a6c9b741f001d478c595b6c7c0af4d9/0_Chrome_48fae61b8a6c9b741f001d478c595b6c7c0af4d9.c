bool AddPolicyForGPU(CommandLine* cmd_line, sandbox::TargetPolicy* policy) {
#if !defined(NACL_WIN64)  // We don't need this code on win nacl64.
  if (base::win::GetVersion() > base::win::VERSION_SERVER_2003) {
    if (cmd_line->GetSwitchValueASCII(switches::kUseGL) ==
        gfx::kGLImplementationDesktopName) {
      policy->SetTokenLevel(sandbox::USER_RESTRICTED_SAME_ACCESS,
                            sandbox::USER_LIMITED);
      policy->SetJobLevel(sandbox::JOB_UNPROTECTED, 0);
      policy->SetDelayedIntegrityLevel(sandbox::INTEGRITY_LEVEL_LOW);
    } else {
      if (cmd_line->GetSwitchValueASCII(switches::kUseGL) ==
          gfx::kGLImplementationSwiftShaderName ||
          cmd_line->HasSwitch(switches::kReduceGpuSandbox)) {
        policy->SetTokenLevel(sandbox::USER_RESTRICTED_SAME_ACCESS,
                              sandbox::USER_LIMITED);
        policy->SetJobLevel(sandbox::JOB_LIMITED_USER,
                            JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS |
                            JOB_OBJECT_UILIMIT_DESKTOP |
                            JOB_OBJECT_UILIMIT_EXITWINDOWS |
                            JOB_OBJECT_UILIMIT_DISPLAYSETTINGS);
      } else {
        policy->SetTokenLevel(sandbox::USER_RESTRICTED_SAME_ACCESS,
                               sandbox::USER_RESTRICTED);
         policy->SetJobLevel(sandbox::JOB_LOCKDOWN,
                             JOB_OBJECT_UILIMIT_HANDLES);
        // This is a trick to keep the GPU out of low-integrity processes. It
        // starts at low-integrity for UIPI to work, then drops below
        // low-integrity after warm-up.
        policy->SetDelayedIntegrityLevel(sandbox::INTEGRITY_LEVEL_UNTRUSTED);
       }
 
       policy->SetIntegrityLevel(sandbox::INTEGRITY_LEVEL_LOW);
    }
  } else {
    policy->SetJobLevel(sandbox::JOB_UNPROTECTED, 0);
    policy->SetTokenLevel(sandbox::USER_UNPROTECTED,
                          sandbox::USER_LIMITED);
  }

  sandbox::ResultCode result = policy->AddRule(
      sandbox::TargetPolicy::SUBSYS_NAMED_PIPES,
      sandbox::TargetPolicy::NAMEDPIPES_ALLOW_ANY,
      L"\\\\.\\pipe\\chrome.gpu.*");
  if (result != sandbox::SBOX_ALL_OK)
    return false;

  result = policy->AddRule(sandbox::TargetPolicy::SUBSYS_HANDLES,
                           sandbox::TargetPolicy::HANDLES_DUP_ANY,
                           L"Section");
  if (result != sandbox::SBOX_ALL_OK)
    return false;

  AddGenericDllEvictionPolicy(policy);
#endif
  return true;
}
