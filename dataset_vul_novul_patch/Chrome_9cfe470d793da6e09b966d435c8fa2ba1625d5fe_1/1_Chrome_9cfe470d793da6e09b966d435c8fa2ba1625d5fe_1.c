base::Optional<AuthenticatorGetInfoResponse> ReadCTAPGetInfoResponse(
    base::span<const uint8_t> buffer) {
  if (buffer.size() <= kResponseCodeLength ||
      GetResponseCode(buffer) != CtapDeviceResponseCode::kSuccess)
    return base::nullopt;

  base::Optional<CBOR> decoded_response =
      cbor::CBORReader::Read(buffer.subspan(1));

  if (!decoded_response || !decoded_response->is_map())
    return base::nullopt;

  const auto& response_map = decoded_response->GetMap();

  auto it = response_map.find(CBOR(1));
  if (it == response_map.end() || !it->second.is_array() ||
      it->second.GetArray().size() > 2) {
    return base::nullopt;
  }

  base::flat_set<ProtocolVersion> protocol_versions;
  for (const auto& version : it->second.GetArray()) {
    if (!version.is_string())
      return base::nullopt;

    auto protocol = ConvertStringToProtocolVersion(version.GetString());
    if (protocol == ProtocolVersion::kUnknown) {
      VLOG(2) << "Unexpected protocol version received.";
      continue;
    }

    if (!protocol_versions.insert(protocol).second)
      return base::nullopt;
  }

  if (protocol_versions.empty())
    return base::nullopt;

  it = response_map.find(CBOR(3));
  if (it == response_map.end() || !it->second.is_bytestring() ||
      it->second.GetBytestring().size() != kAaguidLength) {
     return base::nullopt;
   }
 
  AuthenticatorGetInfoResponse response(std::move(protocol_versions),
                                        it->second.GetBytestring());
 
   it = response_map.find(CBOR(2));
   if (it != response_map.end()) {
    if (!it->second.is_array())
      return base::nullopt;

    std::vector<std::string> extensions;
    for (const auto& extension : it->second.GetArray()) {
      if (!extension.is_string())
        return base::nullopt;

      extensions.push_back(extension.GetString());
    }
    response.SetExtensions(std::move(extensions));
  }

  AuthenticatorSupportedOptions options;
  it = response_map.find(CBOR(4));
  if (it != response_map.end()) {
    if (!it->second.is_map())
      return base::nullopt;

    const auto& option_map = it->second.GetMap();
    auto option_map_it = option_map.find(CBOR(kPlatformDeviceMapKey));
    if (option_map_it != option_map.end()) {
      if (!option_map_it->second.is_bool())
        return base::nullopt;

      options.SetIsPlatformDevice(option_map_it->second.GetBool());
    }

    option_map_it = option_map.find(CBOR(kResidentKeyMapKey));
    if (option_map_it != option_map.end()) {
      if (!option_map_it->second.is_bool())
        return base::nullopt;

      options.SetSupportsResidentKey(option_map_it->second.GetBool());
    }

    option_map_it = option_map.find(CBOR(kUserPresenceMapKey));
    if (option_map_it != option_map.end()) {
      if (!option_map_it->second.is_bool())
        return base::nullopt;

      options.SetUserPresenceRequired(option_map_it->second.GetBool());
    }

    option_map_it = option_map.find(CBOR(kUserVerificationMapKey));
    if (option_map_it != option_map.end()) {
      if (!option_map_it->second.is_bool())
        return base::nullopt;

      if (option_map_it->second.GetBool()) {
        options.SetUserVerificationAvailability(
            AuthenticatorSupportedOptions::UserVerificationAvailability::
                kSupportedAndConfigured);
      } else {
        options.SetUserVerificationAvailability(
            AuthenticatorSupportedOptions::UserVerificationAvailability::
                kSupportedButNotConfigured);
      }
    }

    option_map_it = option_map.find(CBOR(kClientPinMapKey));
    if (option_map_it != option_map.end()) {
      if (!option_map_it->second.is_bool())
        return base::nullopt;

      if (option_map_it->second.GetBool()) {
        options.SetClientPinAvailability(
            AuthenticatorSupportedOptions::ClientPinAvailability::
                kSupportedAndPinSet);
      } else {
        options.SetClientPinAvailability(
            AuthenticatorSupportedOptions::ClientPinAvailability::
                kSupportedButPinNotSet);
      }
    }
    response.SetOptions(std::move(options));
  }

  it = response_map.find(CBOR(5));
  if (it != response_map.end()) {
    if (!it->second.is_unsigned())
      return base::nullopt;

    response.SetMaxMsgSize(it->second.GetUnsigned());
  }

  it = response_map.find(CBOR(6));
  if (it != response_map.end()) {
    if (!it->second.is_array())
      return base::nullopt;

    std::vector<uint8_t> supported_pin_protocols;
    for (const auto& protocol : it->second.GetArray()) {
      if (!protocol.is_unsigned())
        return base::nullopt;

      supported_pin_protocols.push_back(protocol.GetUnsigned());
    }
    response.SetPinProtocols(std::move(supported_pin_protocols));
  }

  return base::Optional<AuthenticatorGetInfoResponse>(std::move(response));
}
