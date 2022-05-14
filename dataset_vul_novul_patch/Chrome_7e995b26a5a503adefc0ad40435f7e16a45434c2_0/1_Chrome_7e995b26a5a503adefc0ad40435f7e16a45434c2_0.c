  explicit MountState(DriveFsHost* host)
      : host_(host),
        mojo_connection_delegate_(
            host_->delegate_->CreateMojoConnectionDelegate()),
        pending_token_(base::UnguessableToken::Create()),
        binding_(this) {
    source_path_ = base::StrCat({kMountScheme, pending_token_.ToString()});
    std::string datadir_option = base::StrCat(
        {"datadir=",
          host_->profile_path_.Append(kDataPath)
              .Append(host_->delegate_->GetAccountId().GetAccountIdKey())
              .value()});
    chromeos::disks::DiskMountManager::GetInstance()->MountPath(
        source_path_, "",
        base::StrCat(
            {"drivefs-", host_->delegate_->GetAccountId().GetAccountIdKey()}),
        {datadir_option}, chromeos::MOUNT_TYPE_NETWORK_STORAGE,
        chromeos::MOUNT_ACCESS_MODE_READ_WRITE);
     auto bootstrap =
         mojo::MakeProxy(mojo_connection_delegate_->InitializeMojoConnection());
     mojom::DriveFsDelegatePtr delegate;
    binding_.Bind(mojo::MakeRequest(&delegate));
    bootstrap->Init(
        {base::in_place, host_->delegate_->GetAccountId().GetUserEmail()},
        mojo::MakeRequest(&drivefs_), std::move(delegate));

    PendingConnectionManager::Get().ExpectOpenIpcChannel(
         pending_token_,
         base::BindOnce(&DriveFsHost::MountState::AcceptMojoConnection,
                        base::Unretained(this)));
   }
