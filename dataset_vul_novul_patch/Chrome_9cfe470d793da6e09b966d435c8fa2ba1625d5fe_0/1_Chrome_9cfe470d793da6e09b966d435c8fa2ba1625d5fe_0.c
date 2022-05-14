bool FidoCableHandshakeHandler::ValidateAuthenticatorHandshakeMessage(
    base::span<const uint8_t> response) {
  crypto::HMAC hmac(crypto::HMAC::SHA256);
  if (!hmac.Init(handshake_key_))
    return false;

  if (response.size() != kCableAuthenticatorHandshakeMessageSize) {
    return false;
  }

  const auto authenticator_hello = response.first(
      kCableAuthenticatorHandshakeMessageSize - kCableHandshakeMacMessageSize);
  if (!hmac.VerifyTruncated(
          fido_parsing_utils::ConvertToStringPiece(authenticator_hello),
          fido_parsing_utils::ConvertToStringPiece(
              response.subspan(authenticator_hello.size())))) {
    return false;
  }

  const auto authenticator_hello_cbor =
      cbor::CBORReader::Read(authenticator_hello);
  if (!authenticator_hello_cbor || !authenticator_hello_cbor->is_map() ||
      authenticator_hello_cbor->GetMap().size() != 2) {
    return false;
  }

  const auto authenticator_hello_msg =
      authenticator_hello_cbor->GetMap().find(cbor::CBORValue(0));
  if (authenticator_hello_msg == authenticator_hello_cbor->GetMap().end() ||
      !authenticator_hello_msg->second.is_string() ||
      authenticator_hello_msg->second.GetString() !=
          kCableAuthenticatorHelloMessage) {
    return false;
  }

  const auto authenticator_random_nonce =
      authenticator_hello_cbor->GetMap().find(cbor::CBORValue(1));
  if (authenticator_random_nonce == authenticator_hello_cbor->GetMap().end() ||
      !authenticator_random_nonce->second.is_bytestring() ||
      authenticator_random_nonce->second.GetBytestring().size() != 16) {
    return false;
   }
 
   cable_device_->SetEncryptionData(
      GetEncryptionKeyAfterSuccessfulHandshake(
          authenticator_random_nonce->second.GetBytestring()),
       nonce_);
 
   return true;
}
