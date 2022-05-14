 std::unique_ptr<SymmetricKey> CopyDefaultPaddingKey() {
  return SymmetricKey::Import(kPaddingKeyAlgorithm,
                              (*GetPaddingKeyInternal())->key());
 }
