void MidiManagerUsb::DispatchSendMidiData(MidiManagerClient* client,
                                           uint32_t port_index,
                                           const std::vector<uint8>& data,
                                           double timestamp) {
  if (port_index >= output_streams_.size()) {
    // |port_index| is provided by a renderer so we can't believe that it is
    // in the valid range.
    // TODO(toyoshim): Move this check to MidiHost and kill the renderer when
    // it fails.
    return;
  }
   output_streams_[port_index]->Send(data);
   client->AccumulateMidiBytesSent(data.size());
 }
