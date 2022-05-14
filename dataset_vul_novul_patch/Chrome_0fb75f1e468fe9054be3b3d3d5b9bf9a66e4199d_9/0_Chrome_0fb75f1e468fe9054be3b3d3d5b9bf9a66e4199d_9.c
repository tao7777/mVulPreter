 LeafUnwindBlacklist* LeafUnwindBlacklist::GetInstance() {
  // Leaky for performance reasons.
   return Singleton<LeafUnwindBlacklist,
                    LeakySingletonTraits<LeafUnwindBlacklist>>::get();
 }
