std::vector<uint8_t> NtlmClient::GenerateAuthenticateMessage(
    const base::string16& domain,
    const base::string16& username,
    const base::string16& password,
    const std::string& hostname,
    const std::string& channel_bindings,
    const std::string& spn,
    uint64_t client_time,
    base::span<const uint8_t, kChallengeLen> client_challenge,
    base::span<const uint8_t> server_challenge_message) const {
  if (hostname.length() > kMaxFqdnLen || domain.length() > kMaxFqdnLen ||
      username.length() > kMaxUsernameLen ||
      password.length() > kMaxPasswordLen) {
    return {};
  }

  NegotiateFlags challenge_flags;
  uint8_t server_challenge[kChallengeLen];
  uint8_t lm_response[kResponseLenV1];
  uint8_t ntlm_response[kResponseLenV1];

  std::vector<uint8_t> updated_target_info;
  std::vector<uint8_t> v2_proof_input;
  uint8_t v2_proof[kNtlmProofLenV2];
  uint8_t v2_session_key[kSessionKeyLenV2];

  if (IsNtlmV2()) {
    std::vector<AvPair> av_pairs;
    if (!ParseChallengeMessageV2(server_challenge_message, &challenge_flags,
                                 server_challenge, &av_pairs)) {
      return {};
    }

    uint64_t timestamp;
    updated_target_info =
        GenerateUpdatedTargetInfo(IsMicEnabled(), IsEpaEnabled(),
                                  channel_bindings, spn, av_pairs, &timestamp);

    memset(lm_response, 0, kResponseLenV1);
    if (timestamp == UINT64_MAX) {
      timestamp = client_time;
    }

     uint8_t v2_hash[kNtlmHashLen];
     GenerateNtlmHashV2(domain, username, password, v2_hash);
     v2_proof_input = GenerateProofInputV2(timestamp, client_challenge);
    GenerateNtlmProofV2(v2_hash, server_challenge, v2_proof_input,
                         updated_target_info, v2_proof);
     GenerateSessionBaseKeyV2(v2_hash, v2_proof, v2_session_key);
   } else {
    if (!ParseChallengeMessage(server_challenge_message, &challenge_flags,
                               server_challenge)) {
      return {};
    }

    GenerateResponsesV1WithSessionSecurity(password, server_challenge,
                                           client_challenge, lm_response,
                                           ntlm_response);
  }

  NegotiateFlags authenticate_flags = (challenge_flags & negotiate_flags_) |
                                      NegotiateFlags::kExtendedSessionSecurity;

  bool is_unicode = (authenticate_flags & NegotiateFlags::kUnicode) ==
                    NegotiateFlags::kUnicode;

  SecurityBuffer lm_info;
  SecurityBuffer ntlm_info;
  SecurityBuffer domain_info;
  SecurityBuffer username_info;
  SecurityBuffer hostname_info;
  SecurityBuffer session_key_info;
  size_t authenticate_message_len;

  CalculatePayloadLayout(is_unicode, domain, username, hostname,
                         updated_target_info.size(), &lm_info, &ntlm_info,
                         &domain_info, &username_info, &hostname_info,
                         &session_key_info, &authenticate_message_len);

  NtlmBufferWriter authenticate_writer(authenticate_message_len);
  bool writer_result = WriteAuthenticateMessage(
      &authenticate_writer, lm_info, ntlm_info, domain_info, username_info,
      hostname_info, session_key_info, authenticate_flags);
  DCHECK(writer_result);

  if (IsNtlmV2()) {
    writer_result = authenticate_writer.WriteZeros(kVersionFieldLen) &&
                    authenticate_writer.WriteZeros(kMicLenV2);

    DCHECK(writer_result);
  }

  DCHECK(authenticate_writer.GetCursor() == GetAuthenticateHeaderLength());
  DCHECK(GetAuthenticateHeaderLength() == lm_info.offset);

  if (IsNtlmV2()) {
    writer_result =
        WriteResponsePayloadsV2(&authenticate_writer, lm_response, v2_proof,
                                v2_proof_input, updated_target_info);
  } else {
    DCHECK_EQ(kResponseLenV1, lm_info.length);
    DCHECK_EQ(kResponseLenV1, ntlm_info.length);
    writer_result =
        WriteResponsePayloads(&authenticate_writer, lm_response, ntlm_response);
  }

  DCHECK(writer_result);
  DCHECK_EQ(authenticate_writer.GetCursor(), domain_info.offset);

  writer_result = WriteStringPayloads(&authenticate_writer, is_unicode, domain,
                                      username, hostname);
  DCHECK(writer_result);
  DCHECK(authenticate_writer.IsEndOfBuffer());
  DCHECK_EQ(authenticate_message_len, authenticate_writer.GetLength());

  std::vector<uint8_t> auth_msg = authenticate_writer.Pass();

  if (IsMicEnabled()) {
    DCHECK_LT(kMicOffsetV2 + kMicLenV2, authenticate_message_len);

    base::span<uint8_t, kMicLenV2> mic(
        const_cast<uint8_t*>(auth_msg.data()) + kMicOffsetV2, kMicLenV2);
    GenerateMicV2(v2_session_key, negotiate_message_, server_challenge_message,
                  auth_msg, mic);
  }

  return auth_msg;
}
