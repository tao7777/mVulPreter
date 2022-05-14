void UpdateTargetInfoAvPairs(bool is_mic_enabled,
                             bool is_epa_enabled,
                             const std::string& channel_bindings,
                             const std::string& spn,
                             std::vector<AvPair>* av_pairs,
                             uint64_t* server_timestamp,
                             size_t* target_info_len) {
  *server_timestamp = UINT64_MAX;
  *target_info_len = 0;

  bool need_flags_added = is_mic_enabled;
  for (AvPair& pair : *av_pairs) {
    *target_info_len += pair.avlen + kAvPairHeaderLen;
    switch (pair.avid) {
      case TargetInfoAvId::kFlags:
        if (is_mic_enabled) {
          pair.flags = pair.flags | TargetInfoAvFlags::kMicPresent;
        }

        need_flags_added = false;
        break;
      case TargetInfoAvId::kTimestamp:
        *server_timestamp = pair.timestamp;
        break;
      case TargetInfoAvId::kEol:
      case TargetInfoAvId::kChannelBindings:
      case TargetInfoAvId::kTargetName:
        NOTREACHED();
        break;
      default:
        break;
    }
  }

  if (need_flags_added) {
    DCHECK(is_mic_enabled);
    AvPair flags_pair(TargetInfoAvId::kFlags, sizeof(uint32_t));
    flags_pair.flags = TargetInfoAvFlags::kMicPresent;

    av_pairs->push_back(flags_pair);
    *target_info_len += kAvPairHeaderLen + flags_pair.avlen;
  }

  if (is_epa_enabled) {
    std::vector<uint8_t> channel_bindings_hash(kChannelBindingsHashLen, 0);
 
     if (!channel_bindings.empty()) {
      GenerateChannelBindingHashV2(channel_bindings, channel_bindings_hash);
     }
 
     av_pairs->emplace_back(TargetInfoAvId::kChannelBindings,
                           std::move(channel_bindings_hash));

    base::string16 spn16 = base::UTF8ToUTF16(spn);
    NtlmBufferWriter spn_writer(spn16.length() * 2);
    bool spn_writer_result =
        spn_writer.WriteUtf16String(spn16) && spn_writer.IsEndOfBuffer();
    DCHECK(spn_writer_result);

    av_pairs->emplace_back(TargetInfoAvId::kTargetName, spn_writer.Pass());

    *target_info_len +=
        (2 * kAvPairHeaderLen) + kChannelBindingsHashLen + (spn16.length() * 2);
  }

  *target_info_len += kAvPairHeaderLen;
}
