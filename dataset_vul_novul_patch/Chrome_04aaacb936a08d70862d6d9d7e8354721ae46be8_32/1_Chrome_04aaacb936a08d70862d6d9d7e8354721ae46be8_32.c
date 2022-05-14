std::unique_ptr<SymmetricKey>* GetPaddingKey() {
   static base::NoDestructor<std::unique_ptr<SymmetricKey>> s_padding_key([] {
     return SymmetricKey::GenerateRandomKey(kPaddingKeyAlgorithm, 128);
   }());
  return s_padding_key.get();
}
