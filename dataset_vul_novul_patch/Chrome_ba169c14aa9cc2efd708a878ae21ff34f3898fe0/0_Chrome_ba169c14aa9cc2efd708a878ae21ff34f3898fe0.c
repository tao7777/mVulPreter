DOMStorageContextWrapper::DOMStorageContextWrapper(
scoped_refptr<DOMStorageContextWrapper> DOMStorageContextWrapper::Create(
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
 
  scoped_refptr<DOMStorageContextImpl> old_context_impl =
      base::MakeRefCounted<DOMStorageContextImpl>(
          data_path.empty() ? data_path
                            : data_path.AppendASCII(kSessionStorageDirectory),
          special_storage_policy,
          new DOMStorageWorkerPoolTaskRunner(std::move(primary_sequence),
                                             std::move(commit_sequence)));
  auto mojo_task_runner =
       base::CreateSingleThreadTaskRunnerWithTraits({BrowserThread::IO});
 
  base::FilePath legacy_localstorage_path =
      data_path.empty() ? data_path
                        : data_path.AppendASCII(kLocalStorageDirectory);
  base::FilePath new_localstorage_path =
      profile_path.empty()
          ? base::FilePath()
          : local_partition_path.AppendASCII(kLocalStorageDirectory);
  LocalStorageContextMojo* mojo_local_state = new LocalStorageContextMojo(
      mojo_task_runner, connector, old_context_impl->task_runner(),
      legacy_localstorage_path, new_localstorage_path, special_storage_policy);
  SessionStorageContextMojo* mojo_session_state = nullptr;
   if (base::FeatureList::IsEnabled(blink::features::kOnionSoupDOMStorage)) {
    mojo_session_state = new SessionStorageContextMojo(
        mojo_task_runner, connector,
 #if defined(OS_ANDROID)
        SessionStorageContextMojo::BackingMode::kClearDiskStateOnOpen,
#else
        profile_path.empty()
            ? SessionStorageContextMojo::BackingMode::kNoDisk
            : SessionStorageContextMojo::BackingMode::kRestoreDiskState,
#endif
         local_partition_path, std::string(kSessionStorageDirectory));
   }
 
  return base::WrapRefCounted(new DOMStorageContextWrapper(
      std::move(legacy_localstorage_path), std::move(old_context_impl),
      mojo_task_runner, mojo_local_state, mojo_session_state));
}

DOMStorageContextWrapper::DOMStorageContextWrapper(
    base::FilePath legacy_local_storage_path,
    scoped_refptr<DOMStorageContextImpl> context_impl,
    scoped_refptr<base::SequencedTaskRunner> mojo_task_runner,
    LocalStorageContextMojo* mojo_local_storage_context,
    SessionStorageContextMojo* mojo_session_storage_context)
    : mojo_state_(mojo_local_storage_context),
      mojo_session_state_(mojo_session_storage_context),
      mojo_task_runner_(std::move(mojo_task_runner)),
      legacy_localstorage_path_(std::move(legacy_local_storage_path)),
      context_(std::move(context_impl)) {
   memory_pressure_listener_.reset(new base::MemoryPressureListener(
       base::BindRepeating(&DOMStorageContextWrapper::OnMemoryPressure,
                           base::Unretained(this))));
}
