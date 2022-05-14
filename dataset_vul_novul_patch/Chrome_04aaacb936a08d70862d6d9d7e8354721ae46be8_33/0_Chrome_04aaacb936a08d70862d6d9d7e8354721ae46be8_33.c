 void ResetPaddingKeyForTesting() {
  *GetPaddingKeyInternal() =
      SymmetricKey::GenerateRandomKey(kPaddingKeyAlgorithm, 128);
 }
