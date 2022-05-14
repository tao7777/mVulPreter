 off_t HFSForkReadStream::Seek(off_t offset, int whence) {
   DCHECK_EQ(SEEK_SET, whence);
   DCHECK_GE(offset, 0);
  DCHECK(offset == 0 || static_cast<uint64_t>(offset) < fork_.logicalSize);
   size_t target_block = offset / hfs_->block_size();
   size_t block_count = 0;
   for (size_t i = 0; i < arraysize(fork_.extents); ++i) {
    const HFSPlusExtentDescriptor* extent = &fork_.extents[i];

    if (extent->startBlock == 0 && extent->blockCount == 0)
      break;

    base::CheckedNumeric<size_t> new_block_count(block_count);
    new_block_count += extent->blockCount;
    if (!new_block_count.IsValid()) {
      DLOG(ERROR) << "Seek offset block count overflows";
      return false;
    }

    if (target_block < new_block_count.ValueOrDie()) {
      if (current_extent_ != i) {
        read_current_extent_ = false;
        current_extent_ = i;
      }
      auto iterator_block_offset =
          base::CheckedNumeric<size_t>(block_count) * hfs_->block_size();
      if (!iterator_block_offset.IsValid()) {
        DLOG(ERROR) << "Seek block offset overflows";
        return false;
      }
      fork_logical_offset_ = offset;
      return offset;
    }

    block_count = new_block_count.ValueOrDie();
  }
  return -1;
}
