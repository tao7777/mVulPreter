ChromotingHost::ChromotingHost(ChromotingHostContext* context,
                               MutableHostConfig* config,
                               DesktopEnvironment* environment,
                               AccessVerifier* access_verifier,
                               bool allow_nat_traversal)
    : context_(context),
      desktop_environment_(environment),
       config_(config),
       access_verifier_(access_verifier),
       allow_nat_traversal_(allow_nat_traversal),
      state_(kInitial),
       stopping_recorders_(0),
       protocol_config_(protocol::CandidateSessionConfig::CreateDefault()),
       is_curtained_(false),
       is_it2me_(false) {
  DCHECK(desktop_environment_);
  desktop_environment_->set_host(this);
}
