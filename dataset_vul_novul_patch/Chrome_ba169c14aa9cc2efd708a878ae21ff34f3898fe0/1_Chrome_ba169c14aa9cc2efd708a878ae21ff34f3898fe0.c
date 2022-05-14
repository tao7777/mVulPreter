DOMStorageContextWrapper::DOMStorageContextWrapper(
     service_manager::Connector* connector,
     const base::FilePath& profile_path,
     const base::FilePath& local_partition_path,
    storage::SpecialStoragePolicy* special_storage_policy) {
  base::FilePath data_path;
  if (!profile_path.empty())
    data_path = profile_path.Append(local_partition_path);

  scoped_refptr<base::SequencedTaskRunner> primary_sequence =
      base::CreateSequencedTaskRunnerWithTraits(
          {base::MayBlock(), base::TaskPriority::USER_BLOCKING,
           base::TaskShutdownBehavior::BLOCK_SHUTDOWN});
  scoped_refptr<base::SequencedTaskRunner> commit_sequence =
      base::CreateSequencedTaskRunnerWithTraits(
           {base::MayBlock(), base::TaskPriority::BEST_EFFORT,
            base::TaskShutdownBehavior::BLOCK_SHUTDOWN});
 
  legacy_localstorage_path_ =
      data_path.empty() ? data_path
                        : data_path.AppendASCII(kLocalStorageDirectory);
  context_ = new DOMStorageContextImpl(
      data_path.empty() ? data_path
                        : data_path.AppendASCII(kSessionStorageDirectory),
      special_storage_policy,
      new DOMStorageWorkerPoolTaskRunner(std::move(primary_sequence),
                                         std::move(commit_sequence)));
  base::FilePath storage_dir;
  if (!profile_path.empty())
    storage_dir = local_partition_path.AppendASCII(kLocalStorageDirectory);
  mojo_task_runner_ =
       base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO});
  mojo_state_ = new LocalStorageContextMojo(
      mojo_task_runner_, connector, context_->task_runner(),
      legacy_localstorage_path_, storage_dir, special_storage_policy);
 
   if (base::FeatureList::IsEnabled(blink::features::kOnionSoupDOMStorage)) {
    mojo_session_state_ = new SessionStorageContextMojo(
        mojo_task_runner_, connector,
 #if defined(OS_ANDROID)
        SessionStorageContextMojo::BackingMode::kClearDiskStateOnOpen,
#else
        profile_path.empty()
            ? SessionStorageContextMojo::BackingMode::kNoDisk
            : SessionStorageContextMojo::BackingMode::kRestoreDiskState,
#endif
         local_partition_path, std::string(kSessionStorageDirectory));
   }
 
   memory_pressure_listener_.reset(new base::MemoryPressureListener(
       base::BindRepeating(&DOMStorageContextWrapper::OnMemoryPressure,
                           base::Unretained(this))));
}
