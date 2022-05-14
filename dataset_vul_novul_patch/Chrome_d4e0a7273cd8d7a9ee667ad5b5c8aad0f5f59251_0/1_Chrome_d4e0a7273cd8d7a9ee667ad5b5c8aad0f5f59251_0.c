void FileManagerBrowserTestBase::SetUpOnMainThread() {
  sync_file_system::SyncFileSystemServiceFactory::GetInstance()
      ->set_mock_remote_file_service(
          std::make_unique<::testing::NiceMock<
              sync_file_system::MockRemoteFileSyncService>>());

  extensions::ExtensionApiTest::SetUpOnMainThread();
  CHECK(profile());
  CHECK_EQ(!!browser(), GetRequiresStartupBrowser());

  if (DoesTestStartWithNoVolumesMounted()) {
    VolumeManager::Get(profile())->RemoveDownloadsDirectoryForTesting();
  } else {
    CHECK(local_volume_->Mount(profile()));
  }

  if (!IsGuestModeTest()) {
    CHECK(embedded_test_server()->Start());
    drive_volume_ = drive_volumes_[profile()->GetOriginalProfile()].get();
    if (!DoesTestStartWithNoVolumesMounted()) {
      test_util::WaitUntilDriveMountPointIsAdded(profile());
    }

    crostini_volume_ = std::make_unique<CrostiniTestVolume>();
    profile()->GetPrefs()->SetBoolean(crostini::prefs::kCrostiniEnabled, true);
    crostini::CrostiniManager* crostini_manager =
        crostini::CrostiniManager::GetForProfile(
            profile()->GetOriginalProfile());
    crostini_manager->set_skip_restart_for_testing();
    crostini_manager->AddRunningVmForTesting(crostini::kCrostiniDefaultVmName);
    crostini_manager->AddRunningContainerForTesting(
        crostini::kCrostiniDefaultVmName,
        crostini::ContainerInfo(crostini::kCrostiniDefaultContainerName,
                                "testuser", "/home/testuser"));
    chromeos::DBusThreadManager* dbus_thread_manager =
        chromeos::DBusThreadManager::Get();
    static_cast<chromeos::FakeCrosDisksClient*>(
        dbus_thread_manager->GetCrosDisksClient())
        ->AddCustomMountPointCallback(
            base::BindRepeating(&FileManagerBrowserTestBase::MaybeMountCrostini,
                                base::Unretained(this)));

    if (arc::IsArcAvailable()) {
      arc_file_system_instance_ =
          std::make_unique<arc::FakeFileSystemInstance>();
      arc::ArcServiceManager::Get()
          ->arc_bridge_service()
          ->file_system()
          ->SetInstance(arc_file_system_instance_.get());
      arc::WaitForInstanceReady(
          arc::ArcServiceManager::Get()->arc_bridge_service()->file_system());
      ASSERT_TRUE(arc_file_system_instance_->InitCalled());

      if (IsDocumentsProviderTest()) {
        documents_provider_volume_ =
            std::make_unique<DocumentsProviderTestVolume>(
                arc_file_system_instance_.get(), "com.example.documents",
                "root");
        if (!DoesTestStartWithNoVolumesMounted()) {
          documents_provider_volume_->Mount(profile());
        }
      }
    } else {
      android_files_volume_ = std::make_unique<AndroidFilesTestVolume>();
      if (!DoesTestStartWithNoVolumesMounted()) {
        android_files_volume_->Mount(profile());
      }
    }

    if (!IsIncognitoModeTest()) {
      file_tasks_observer_ =
          std::make_unique<testing::StrictMock<MockFileTasksObserver>>(
              profile());
    } else {
      EXPECT_FALSE(file_tasks::FileTasksNotifier::GetForProfile(profile()));
    }
  }

   display_service_ =
       std::make_unique<NotificationDisplayServiceTester>(profile());
 
  if (IsOfflineTest()) {
    ExtensionFunctionRegistry::GetInstance().OverrideFunctionForTesting(
        "fileManagerPrivate.getDriveConnectionState",
        &NewExtensionFunction<OfflineGetDriveConnectionState>);
  }
   content::NetworkConnectionChangeSimulator network_change_simulator;
   network_change_simulator.SetConnectionType(
       IsOfflineTest() ? network::mojom::ConnectionType::CONNECTION_NONE
                      : network::mojom::ConnectionType::CONNECTION_ETHERNET);

  test::AddDefaultComponentExtensionsOnMainThread(profile());

  if (IsTabletModeTest()) {
    EnableVirtualKeyboard();
  }

  select_factory_ = new SelectFileDialogExtensionTestFactory();
  ui::SelectFileDialog::SetFactory(select_factory_);
}
