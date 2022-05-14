 void ResetPaddingKeyForTesting() {
  *GetPaddingKey() = SymmetricKey::GenerateRandomKey(kPaddingKeyAlgorithm, 128);
 }
