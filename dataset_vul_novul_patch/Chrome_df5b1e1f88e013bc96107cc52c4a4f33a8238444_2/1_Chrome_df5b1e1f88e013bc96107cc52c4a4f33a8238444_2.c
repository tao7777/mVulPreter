void BackendIO::ExecuteBackendOperation() {
  switch (operation_) {
    case OP_INIT:
      result_ = backend_->SyncInit();
      break;
    case OP_OPEN: {
      scoped_refptr<EntryImpl> entry;
      result_ = backend_->SyncOpenEntry(key_, &entry);
      *entry_ptr_ = LeakEntryImpl(std::move(entry));
      break;
    }
    case OP_CREATE: {
      scoped_refptr<EntryImpl> entry;
      result_ = backend_->SyncCreateEntry(key_, &entry);
      *entry_ptr_ = LeakEntryImpl(std::move(entry));
      break;
    }
    case OP_DOOM:
      result_ = backend_->SyncDoomEntry(key_);
      break;
    case OP_DOOM_ALL:
      result_ = backend_->SyncDoomAllEntries();
      break;
    case OP_DOOM_BETWEEN:
      result_ = backend_->SyncDoomEntriesBetween(initial_time_, end_time_);
      break;
    case OP_DOOM_SINCE:
      result_ = backend_->SyncDoomEntriesSince(initial_time_);
      break;
    case OP_SIZE_ALL:
      result_ = backend_->SyncCalculateSizeOfAllEntries();
      break;
    case OP_OPEN_NEXT: {
      scoped_refptr<EntryImpl> entry;
      result_ = backend_->SyncOpenNextEntry(iterator_, &entry);
      *entry_ptr_ = LeakEntryImpl(std::move(entry));
      break;
    }
    case OP_END_ENUMERATION:
      backend_->SyncEndEnumeration(std::move(scoped_iterator_));
      result_ = net::OK;
      break;
    case OP_ON_EXTERNAL_CACHE_HIT:
      backend_->SyncOnExternalCacheHit(key_);
      result_ = net::OK;
      break;
    case OP_CLOSE_ENTRY:
      entry_->Release();
      result_ = net::OK;
      break;
    case OP_DOOM_ENTRY:
      entry_->DoomImpl();
      result_ = net::OK;
      break;
    case OP_FLUSH_QUEUE:
      result_ = net::OK;
      break;
    case OP_RUN_TASK:
      task_.Run();
      result_ = net::OK;
      break;
    default:
      NOTREACHED() << "Invalid Operation";
      result_ = net::ERR_UNEXPECTED;
   }
   DCHECK_NE(net::ERR_IO_PENDING, result_);
   NotifyController();
 }
