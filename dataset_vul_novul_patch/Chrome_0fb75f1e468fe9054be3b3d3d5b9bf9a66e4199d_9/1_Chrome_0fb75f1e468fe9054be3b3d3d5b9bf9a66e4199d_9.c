 LeafUnwindBlacklist* LeafUnwindBlacklist::GetInstance() {
   return Singleton<LeafUnwindBlacklist,
                    LeakySingletonTraits<LeafUnwindBlacklist>>::get();
 }
