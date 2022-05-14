bool ParamTraits<AudioParameters>::Read(const Message* m,
                                        PickleIterator* iter,
                                        AudioParameters* r) {
  int format, channel_layout, sample_rate, bits_per_sample,
      frames_per_buffer, channels;

  if (!m->ReadInt(iter, &format) ||
      !m->ReadInt(iter, &channel_layout) ||
      !m->ReadInt(iter, &sample_rate) ||
      !m->ReadInt(iter, &bits_per_sample) ||
      !m->ReadInt(iter, &frames_per_buffer) ||
      !m->ReadInt(iter, &channels))
    return false;
   r->Reset(static_cast<AudioParameters::Format>(format),
            static_cast<ChannelLayout>(channel_layout),
            sample_rate, bits_per_sample, frames_per_buffer);
   return true;
 }
