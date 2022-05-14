AudioOutputStream* AudioManagerBase::MakeAudioOutputStreamProxy(
    const AudioParameters& params) {
  DCHECK(GetMessageLoop()->BelongsToCurrentThread());

  scoped_refptr<AudioOutputDispatcher>& dispatcher =
      output_dispatchers_[params];
   if (!dispatcher) {
     base::TimeDelta close_delay =
         base::TimeDelta::FromSeconds(kStreamCloseDelaySeconds);
#if defined(OS_WIN) || defined(OS_MACOSX)
     const CommandLine* cmd_line = CommandLine::ForCurrentProcess();
    if (!cmd_line->HasSwitch(switches::kDisableAudioMixer)) {
       dispatcher = new AudioOutputMixer(this, params, close_delay);
    } else
#endif
    {
       dispatcher = new AudioOutputDispatcherImpl(this, params, close_delay);
     }
   }
  return new AudioOutputProxy(dispatcher);
}
