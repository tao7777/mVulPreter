 ssize_t MPEG4DataSource::readAt(off64_t offset, void *data, size_t size) {
     Mutex::Autolock autoLock(mLock);
 
    if (isInRange(mCachedOffset, mCachedSize, offset, size)) {
         memcpy(data, &mCache[offset - mCachedOffset], size);
         return size;
     }

 return mSource->readAt(offset, data, size);
}
