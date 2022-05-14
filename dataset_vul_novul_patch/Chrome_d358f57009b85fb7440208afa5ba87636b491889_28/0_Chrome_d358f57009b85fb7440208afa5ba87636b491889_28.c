  virtual void SetUp() {
    FakeDBusThreadManager* fake_dbus_thread_manager = new FakeDBusThreadManager;
    fake_bluetooth_profile_manager_client_ =
        new FakeBluetoothProfileManagerClient;
     fake_dbus_thread_manager->SetBluetoothProfileManagerClient(
         scoped_ptr<BluetoothProfileManagerClient>(
             fake_bluetooth_profile_manager_client_));
    fake_dbus_thread_manager->SetBluetoothAgentManagerClient(
        scoped_ptr<BluetoothAgentManagerClient>(
            new FakeBluetoothAgentManagerClient));
     fake_dbus_thread_manager->SetBluetoothAdapterClient(
         scoped_ptr<BluetoothAdapterClient>(new FakeBluetoothAdapterClient));
     fake_dbus_thread_manager->SetBluetoothDeviceClient(
        scoped_ptr<BluetoothDeviceClient>(new FakeBluetoothDeviceClient));
    fake_dbus_thread_manager->SetBluetoothInputClient(
        scoped_ptr<BluetoothInputClient>(new FakeBluetoothInputClient));
    DBusThreadManager::InitializeForTesting(fake_dbus_thread_manager);

    device::BluetoothAdapterFactory::GetAdapter(
        base::Bind(&BluetoothProfileChromeOSTest::AdapterCallback,
                   base::Unretained(this)));
    ASSERT_TRUE(adapter_.get() != NULL);
    ASSERT_TRUE(adapter_->IsInitialized());
    ASSERT_TRUE(adapter_->IsPresent());

    adapter_->SetPowered(
        true,
        base::Bind(&base::DoNothing),
        base::Bind(&base::DoNothing));
    ASSERT_TRUE(adapter_->IsPowered());
  }
