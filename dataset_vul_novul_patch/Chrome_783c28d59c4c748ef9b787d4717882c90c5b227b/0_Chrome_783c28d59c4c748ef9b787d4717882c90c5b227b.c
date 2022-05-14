void ScriptProcessorHandler::Process(size_t frames_to_process) {

  AudioBus* input_bus = Input(0).Bus();
  AudioBus* output_bus = Output(0).Bus();

  unsigned double_buffer_index = this->DoubleBufferIndex();
  bool is_double_buffer_index_good =
      double_buffer_index < 2 && double_buffer_index < input_buffers_.size() &&
      double_buffer_index < output_buffers_.size();
  DCHECK(is_double_buffer_index_good);
  if (!is_double_buffer_index_good)
    return;

  AudioBuffer* input_buffer = input_buffers_[double_buffer_index].Get();
  AudioBuffer* output_buffer = output_buffers_[double_buffer_index].Get();

  unsigned number_of_input_channels = internal_input_bus_->NumberOfChannels();
  bool buffers_are_good =
      output_buffer && BufferSize() == output_buffer->length() &&
      buffer_read_write_index_ + frames_to_process <= BufferSize();

  if (internal_input_bus_->NumberOfChannels())
    buffers_are_good = buffers_are_good && input_buffer &&
                       BufferSize() == input_buffer->length();

  DCHECK(buffers_are_good);
  if (!buffers_are_good)
    return;

  bool is_frames_to_process_good = frames_to_process &&
                                   BufferSize() >= frames_to_process &&
                                   !(BufferSize() % frames_to_process);
  DCHECK(is_frames_to_process_good);
  if (!is_frames_to_process_good)
    return;

  unsigned number_of_output_channels = output_bus->NumberOfChannels();

  bool channels_are_good =
      (number_of_input_channels == number_of_input_channels_) &&
      (number_of_output_channels == number_of_output_channels_);
  DCHECK(channels_are_good);
  if (!channels_are_good)
    return;

  for (unsigned i = 0; i < number_of_input_channels; ++i)
    internal_input_bus_->SetChannelMemory(
        i,
        input_buffer->getChannelData(i).View()->Data() +
            buffer_read_write_index_,
        frames_to_process);

  if (number_of_input_channels)
    internal_input_bus_->CopyFrom(*input_bus);

  for (unsigned i = 0; i < number_of_output_channels; ++i) {
    memcpy(output_bus->Channel(i)->MutableData(),
           output_buffer->getChannelData(i).View()->Data() +
               buffer_read_write_index_,
           sizeof(float) * frames_to_process);
  }

  buffer_read_write_index_ =
      (buffer_read_write_index_ + frames_to_process) % BufferSize();

  if (!buffer_read_write_index_) {
    MutexTryLocker try_locker(process_event_lock_);
    if (!try_locker.Locked()) {
      output_buffer->Zero();
    } else if (Context()->GetExecutionContext()) {
      if (Context()->HasRealtimeConstraint()) {
         TaskRunnerHelper::Get(TaskType::kMediaElementEvent,
                               Context()->GetExecutionContext())
            ->PostTask(
                BLINK_FROM_HERE,
                CrossThreadBind(&ScriptProcessorHandler::FireProcessEvent,
                                WrapRefPtr(this), double_buffer_index_));
       } else {
        std::unique_ptr<WaitableEvent> waitable_event =
            WTF::MakeUnique<WaitableEvent>();
 
         TaskRunnerHelper::Get(TaskType::kMediaElementEvent,
                               Context()->GetExecutionContext())
            ->PostTask(
                BLINK_FROM_HERE,
                CrossThreadBind(&ScriptProcessorHandler::
                                    FireProcessEventForOfflineAudioContext,
                                WrapRefPtr(this), double_buffer_index_,
                                CrossThreadUnretained(waitable_event.get())));
 
        waitable_event->Wait();
      }
    }

    SwapBuffers();
  }
}
