AudioOutputStream* AudioManagerBase::MakeAudioOutputStreamProxy(
    const AudioParameters& params) {
  DCHECK(GetMessageLoop()->BelongsToCurrentThread());

  scoped_refptr<AudioOutputDispatcher>& dispatcher =
      output_dispatchers_[params];
   if (!dispatcher) {
     base::TimeDelta close_delay =
         base::TimeDelta::FromSeconds(kStreamCloseDelaySeconds);
     const CommandLine* cmd_line = CommandLine::ForCurrentProcess();
    // TODO(dalecurtis): Browser side mixing has a couple issues that must be
    // fixed before it can be turned on by default: http://crbug.com/138098 and
    // http://crbug.com/140247
    if (cmd_line->HasSwitch(switches::kEnableAudioMixer)) {
       dispatcher = new AudioOutputMixer(this, params, close_delay);
    } else {
       dispatcher = new AudioOutputDispatcherImpl(this, params, close_delay);
     }
   }
  return new AudioOutputProxy(dispatcher);
}
