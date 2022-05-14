 void RegisterSwReporterComponent(ComponentUpdateService* cus) {
  if (!safe_browsing::IsSwReporterEnabled())
     return;
 
   base::string16 cleaner_key_name(kSoftwareRemovalToolRegistryKey);
  cleaner_key_name.append(1, L'\\').append(kCleanerSuffixRegistryKey);
  base::win::RegKey cleaner_key(
      HKEY_CURRENT_USER, cleaner_key_name.c_str(), KEY_ALL_ACCESS);
  if (cleaner_key.Valid()) {
    if (cleaner_key.HasValue(kStartTimeValueName)) {
      if (cleaner_key.HasValue(kVersionValueName)) {
        DWORD version;
        cleaner_key.ReadValueDW(kVersionValueName, &version);
        UMA_HISTOGRAM_SPARSE_SLOWLY("SoftwareReporter.Cleaner.Version",
                                    version);
        cleaner_key.DeleteValue(kVersionValueName);
      }
      int64 start_time_value;
      cleaner_key.ReadInt64(kStartTimeValueName, &start_time_value);

      bool completed = cleaner_key.HasValue(kEndTimeValueName);
      SRTHasCompleted(completed ? SRT_COMPLETED_YES : SRT_COMPLETED_NOT_YET);
      if (completed) {
        int64 end_time_value;
        cleaner_key.ReadInt64(kEndTimeValueName, &end_time_value);
        cleaner_key.DeleteValue(kEndTimeValueName);
        base::TimeDelta run_time(
            base::Time::FromInternalValue(end_time_value) -
            base::Time::FromInternalValue(start_time_value));
        UMA_HISTOGRAM_LONG_TIMES("SoftwareReporter.Cleaner.RunningTime",
                                 run_time);
      }
      DWORD exit_code = safe_browsing::kSwReporterNothingFound;
      if (cleaner_key.HasValue(kExitCodeValueName)) {
        cleaner_key.ReadValueDW(kExitCodeValueName, &exit_code);
        UMA_HISTOGRAM_SPARSE_SLOWLY("SoftwareReporter.Cleaner.ExitCode",
                                    exit_code);
        cleaner_key.DeleteValue(kExitCodeValueName);
      }
      cleaner_key.DeleteValue(kStartTimeValueName);

      if (exit_code == safe_browsing::kSwReporterPostRebootCleanupNeeded ||
          exit_code ==
              safe_browsing::kSwReporterDelayedPostRebootCleanupNeeded) {
        base::TimeDelta elapsed(
            base::Time::Now() -
            base::Time::FromInternalValue(start_time_value));
        DCHECK_GT(elapsed.InMilliseconds(), 0);
        UMA_HISTOGRAM_BOOLEAN(
            "SoftwareReporter.Cleaner.HasRebooted",
            static_cast<uint64>(elapsed.InMilliseconds()) > ::GetTickCount());
      }

      if (cleaner_key.HasValue(kUploadResultsValueName)) {
        base::string16 upload_results;
        cleaner_key.ReadValue(kUploadResultsValueName, &upload_results);
        ReportUploadsWithUma(upload_results);
      }
    } else {
      if (cleaner_key.HasValue(kEndTimeValueName)) {
        SRTHasCompleted(SRT_COMPLETED_LATER);
        cleaner_key.DeleteValue(kEndTimeValueName);
      }
    }
  }
  ReportFoundUwS();

  scoped_ptr<ComponentInstallerTraits> traits(new SwReporterInstallerTraits());
  DefaultComponentInstaller* installer =
      new DefaultComponentInstaller(traits.Pass());
  installer->Register(cus, base::Closure());
}
