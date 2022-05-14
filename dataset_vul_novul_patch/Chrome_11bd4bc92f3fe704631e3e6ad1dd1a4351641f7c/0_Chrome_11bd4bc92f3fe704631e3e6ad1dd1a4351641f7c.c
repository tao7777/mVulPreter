BlobStorageContext::BlobFlattener::BlobFlattener(
    const BlobDataBuilder& input_builder,
    BlobEntry* output_blob,
    BlobStorageRegistry* registry) {
  const std::string& uuid = input_builder.uuid_;
  std::set<std::string> dependent_blob_uuids;

  size_t num_files_with_unknown_size = 0;
  size_t num_building_dependent_blobs = 0;

  bool found_memory_transport = false;
  bool found_file_transport = false;

  base::CheckedNumeric<uint64_t> checked_total_size = 0;
  base::CheckedNumeric<uint64_t> checked_total_memory_size = 0;
  base::CheckedNumeric<uint64_t> checked_transport_quota_needed = 0;
  base::CheckedNumeric<uint64_t> checked_copy_quota_needed = 0;

  for (scoped_refptr<BlobDataItem> input_item : input_builder.items_) {
    const DataElement& input_element = input_item->data_element();
    DataElement::Type type = input_element.type();
    uint64_t length = input_element.length();

    RecordBlobItemSizeStats(input_element);

    if (IsBytes(type)) {
      DCHECK_NE(0 + DataElement::kUnknownSize, input_element.length());
      found_memory_transport = true;
      if (found_file_transport) {
        status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
        return;
      }
      contains_unpopulated_transport_items |=
          (type == DataElement::TYPE_BYTES_DESCRIPTION);
      checked_transport_quota_needed += length;
      checked_total_size += length;
      scoped_refptr<ShareableBlobDataItem> item = new ShareableBlobDataItem(
          std::move(input_item), ShareableBlobDataItem::QUOTA_NEEDED);
      pending_transport_items.push_back(item);
      transport_items.push_back(item.get());
      output_blob->AppendSharedBlobItem(std::move(item));
      continue;
    }
    if (type == DataElement::TYPE_BLOB) {
      BlobEntry* ref_entry = registry->GetEntry(input_element.blob_uuid());

      if (!ref_entry || input_element.blob_uuid() == uuid) {
        status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
        return;
      }

      if (BlobStatusIsError(ref_entry->status())) {
        status = BlobStatus::ERR_REFERENCED_BLOB_BROKEN;
        return;
      }

      if (ref_entry->total_size() == DataElement::kUnknownSize) {
        status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
        return;
      }

      if (dependent_blob_uuids.find(input_element.blob_uuid()) ==
          dependent_blob_uuids.end()) {
        dependent_blobs.push_back(
            std::make_pair(input_element.blob_uuid(), ref_entry));
        dependent_blob_uuids.insert(input_element.blob_uuid());
        if (BlobStatusIsPending(ref_entry->status())) {
          num_building_dependent_blobs++;
        }
      }

      length = length == DataElement::kUnknownSize ? ref_entry->total_size()
                                                   : input_element.length();
      checked_total_size += length;

      if (input_element.offset() == 0 && length == ref_entry->total_size()) {
        for (const auto& shareable_item : ref_entry->items()) {
          output_blob->AppendSharedBlobItem(shareable_item);
        }
        continue;
       }
 
      uint64_t end_byte;
      if (!base::CheckAdd(input_element.offset(), length)
               .AssignIfValid(&end_byte) ||
          end_byte > ref_entry->total_size()) {
         status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
         return;
       }

      BlobSlice slice(*ref_entry, input_element.offset(), length);

      if (!slice.copying_memory_size.IsValid() ||
          !slice.total_memory_size.IsValid()) {
        status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
        return;
      }
      checked_total_memory_size += slice.total_memory_size;

      if (slice.first_source_item) {
        copies.push_back(ItemCopyEntry(slice.first_source_item,
                                       slice.first_item_slice_offset,
                                       slice.dest_items.front()));
        pending_copy_items.push_back(slice.dest_items.front());
      }
      if (slice.last_source_item) {
        copies.push_back(
            ItemCopyEntry(slice.last_source_item, 0, slice.dest_items.back()));
        pending_copy_items.push_back(slice.dest_items.back());
      }
      checked_copy_quota_needed += slice.copying_memory_size;

      for (auto& shareable_item : slice.dest_items) {
        output_blob->AppendSharedBlobItem(std::move(shareable_item));
      }
      continue;
    }

    scoped_refptr<ShareableBlobDataItem> item;
    if (BlobDataBuilder::IsFutureFileItem(input_element)) {
      found_file_transport = true;
      if (found_memory_transport) {
        status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
        return;
      }
      contains_unpopulated_transport_items = true;
      item = new ShareableBlobDataItem(std::move(input_item),
                                       ShareableBlobDataItem::QUOTA_NEEDED);
      pending_transport_items.push_back(item);
      transport_items.push_back(item.get());
      checked_transport_quota_needed += length;
    } else {
      item = new ShareableBlobDataItem(
          std::move(input_item),
          ShareableBlobDataItem::POPULATED_WITHOUT_QUOTA);
    }
    if (length == DataElement::kUnknownSize)
      num_files_with_unknown_size++;

    checked_total_size += length;
    output_blob->AppendSharedBlobItem(std::move(item));
  }

  if (num_files_with_unknown_size > 1 && input_builder.items_.size() > 1) {
    status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
    return;
  }
  if (!checked_total_size.IsValid() || !checked_total_memory_size.IsValid() ||
      !checked_transport_quota_needed.IsValid() ||
      !checked_copy_quota_needed.IsValid()) {
    status = BlobStatus::ERR_INVALID_CONSTRUCTION_ARGUMENTS;
    return;
  }
  total_size = checked_total_size.ValueOrDie();
  total_memory_size = checked_total_memory_size.ValueOrDie();
  transport_quota_needed = checked_transport_quota_needed.ValueOrDie();
  copy_quota_needed = checked_copy_quota_needed.ValueOrDie();
  transport_quota_type = found_file_transport ? TransportQuotaType::FILE
                                              : TransportQuotaType::MEMORY;
  if (transport_quota_needed) {
    status = BlobStatus::PENDING_QUOTA;
  } else {
    status = BlobStatus::PENDING_INTERNALS;
  }
}
