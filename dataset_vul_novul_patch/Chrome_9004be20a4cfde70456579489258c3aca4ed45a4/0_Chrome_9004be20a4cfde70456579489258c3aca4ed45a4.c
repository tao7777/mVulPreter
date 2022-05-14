  void OnReadAllMetadata(
      const SessionStore::SessionInfo& session_info,
      SessionStore::FactoryCompletionCallback callback,
      std::unique_ptr<ModelTypeStore> store,
       std::unique_ptr<ModelTypeStore::RecordList> record_list,
       const base::Optional<syncer::ModelError>& error,
       std::unique_ptr<syncer::MetadataBatch> metadata_batch) {
    // Remove after fixing https://crbug.com/902203.
    TRACE_EVENT0("browser", "FactoryImpl::OnReadAllMetadata");
     if (error) {
       std::move(callback).Run(error, /*store=*/nullptr,
                               /*metadata_batch=*/nullptr);
      return;
    }

    std::map<std::string, sync_pb::SessionSpecifics> initial_data;
    for (ModelTypeStore::Record& record : *record_list) {
      const std::string& storage_key = record.id;
      SessionSpecifics specifics;
      if (storage_key.empty() ||
          !specifics.ParseFromString(std::move(record.value))) {
        DVLOG(1) << "Ignoring corrupt database entry with key: " << storage_key;
        continue;
      }
      initial_data[storage_key].Swap(&specifics);
    }

    auto session_store = std::make_unique<SessionStore>(
        sessions_client_, session_info, std::move(store),
        std::move(initial_data), metadata_batch->GetAllMetadata(),
        restored_foreign_tab_callback_);

    std::move(callback).Run(/*error=*/base::nullopt, std::move(session_store),
                            std::move(metadata_batch));
  }
