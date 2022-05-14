   void Add(int original_content_length, int received_content_length) {
    AddInt64ToListPref(
        kNumDaysInHistory - 1, original_content_length, original_update_.Get());
    AddInt64ToListPref(
        kNumDaysInHistory - 1, received_content_length, received_update_.Get());
   }
