 bool SRTPromptNeedsElevationIcon() {
  return !base::StartsWith(
      base::FieldTrialList::FindFullName(kSRTElevationTrial),
      kSRTElevationAsNeededGroup, base::CompareCase::SENSITIVE);
}

bool IsSwReporterEnabled() {
  return !base::StartsWith(
      base::FieldTrialList::FindFullName(kSRTReporterTrial),
      kSRTReporterOffGroup, base::CompareCase::SENSITIVE);
 }
