void MojoVideoEncodeAccelerator::UseOutputBitstreamBuffer(
    const BitstreamBuffer& buffer) {
  DVLOG(2) << __func__ << " buffer.id()= " << buffer.id()
            << " buffer.size()= " << buffer.size() << "B";
   DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
 

  // TODO(https://crbug.com/793446): Only wrap read-only handles here and change
  // the protection status to kReadOnly.
   mojo::ScopedSharedBufferHandle buffer_handle = mojo::WrapSharedMemoryHandle(
      buffer.handle().Duplicate(), buffer.size(),
      mojo::UnwrappedSharedMemoryHandleProtection::kReadWrite);
 
   vea_->UseOutputBitstreamBuffer(buffer.id(), std::move(buffer_handle));
 }
