 void ChromotingHost::set_protocol_config(
     protocol::CandidateSessionConfig* config) {
  DCHECK(config);
   DCHECK_EQ(state_, kInitial);
   protocol_config_.reset(config);
 }
