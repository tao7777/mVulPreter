 bool IsInSRTPromptFieldTrialGroups() {
  return base::FieldTrialList::FindFullName(kSRTPromptTrial) !=
         kSRTPromptOffGroup;
 }
