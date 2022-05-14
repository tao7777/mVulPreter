 bool SRTPromptNeedsElevationIcon() {
  return base::FieldTrialList::FindFullName(kSRTElevationTrial) !=
         kSRTElevationAsNeededGroup;
 }
