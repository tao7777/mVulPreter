OperationID FileSystemOperationRunner::BeginOperation(
     std::unique_ptr<FileSystemOperation> operation) {
   OperationID id = next_operation_id_++;
 
  operations_[id] = std::move(operation);
   return id;
 }
