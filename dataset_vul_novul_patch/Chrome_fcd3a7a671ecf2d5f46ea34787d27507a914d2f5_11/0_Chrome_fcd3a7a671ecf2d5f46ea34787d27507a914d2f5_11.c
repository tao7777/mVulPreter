   explicit SyncInternal(const std::string& name)
       : name_(name),
         weak_ptr_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)),
         registrar_(NULL),
         change_delegate_(NULL),
         initialized_(false),
        testing_mode_(NON_TEST),
        observing_ip_address_changes_(false),
        traffic_recorder_(kMaxMessagesToRecord, kMaxMessageSizeToRecord),
        encryptor_(NULL),
        unrecoverable_error_handler_(NULL),
        report_unrecoverable_error_function_(NULL),
        created_on_loop_(MessageLoop::current()),
        nigori_overwrite_count_(0) {
    for (int i = syncable::FIRST_REAL_MODEL_TYPE;
         i < syncable::MODEL_TYPE_COUNT; ++i) {
      notification_info_map_.insert(
          std::make_pair(syncable::ModelTypeFromInt(i), NotificationInfo()));
    }

    BindJsMessageHandler(
        "getNotificationState",
        &SyncManager::SyncInternal::GetNotificationState);
    BindJsMessageHandler(
        "getNotificationInfo",
        &SyncManager::SyncInternal::GetNotificationInfo);
    BindJsMessageHandler(
        "getRootNodeDetails",
        &SyncManager::SyncInternal::GetRootNodeDetails);
    BindJsMessageHandler(
        "getNodeSummariesById",
        &SyncManager::SyncInternal::GetNodeSummariesById);
    BindJsMessageHandler(
        "getNodeDetailsById",
        &SyncManager::SyncInternal::GetNodeDetailsById);
    BindJsMessageHandler(
        "getAllNodes",
        &SyncManager::SyncInternal::GetAllNodes);
    BindJsMessageHandler(
        "getChildNodeIds",
        &SyncManager::SyncInternal::GetChildNodeIds);
    BindJsMessageHandler(
        "getClientServerTraffic",
        &SyncManager::SyncInternal::GetClientServerTraffic);
  }
