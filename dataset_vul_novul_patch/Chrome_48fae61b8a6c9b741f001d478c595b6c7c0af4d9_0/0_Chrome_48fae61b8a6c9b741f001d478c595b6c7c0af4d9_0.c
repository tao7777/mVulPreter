bool AddPolicyForRenderer(sandbox::TargetPolicy* policy) {
  sandbox::ResultCode result;
  result = policy->AddRule(sandbox::TargetPolicy::SUBSYS_HANDLES,
                           sandbox::TargetPolicy::HANDLES_DUP_ANY,
                           L"Section");
  if (result != sandbox::SBOX_ALL_OK)
    return false;

  result = policy->AddRule(sandbox::TargetPolicy::SUBSYS_HANDLES,
                           sandbox::TargetPolicy::HANDLES_DUP_ANY,
                           L"Event");
  if (result != sandbox::SBOX_ALL_OK)
    return false;

  policy->SetJobLevel(sandbox::JOB_LOCKDOWN, 0);

  sandbox::TokenLevel initial_token = sandbox::USER_UNPROTECTED;
  if (base::win::GetVersion() > base::win::VERSION_XP) {
    initial_token = sandbox::USER_RESTRICTED_SAME_ACCESS;
   }
 
   policy->SetTokenLevel(initial_token, sandbox::USER_LOCKDOWN);
  // Prevents the renderers from manipulating low-integrity processes.
  policy->SetDelayedIntegrityLevel(sandbox::INTEGRITY_LEVEL_UNTRUSTED);
 
   bool use_winsta = !CommandLine::ForCurrentProcess()->HasSwitch(
                         switches::kDisableAltWinstation);

  if (sandbox::SBOX_ALL_OK !=  policy->SetAlternateDesktop(use_winsta)) {
    DLOG(WARNING) << "Failed to apply desktop security to the renderer";
  }

  AddGenericDllEvictionPolicy(policy);

  return true;
}
