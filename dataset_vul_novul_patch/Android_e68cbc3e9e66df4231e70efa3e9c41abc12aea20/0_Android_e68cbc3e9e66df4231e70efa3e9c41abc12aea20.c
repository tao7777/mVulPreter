status_t Parcel::appendFrom(const Parcel *parcel, size_t offset, size_t len)
{
 const sp<ProcessState> proc(ProcessState::self());
 status_t err;
 const uint8_t *data = parcel->mData;
 const binder_size_t *objects = parcel->mObjects;
 size_t size = parcel->mObjectsSize;
 int startPos = mDataPos;
 int firstIndex = -1, lastIndex = -2;

 if (len == 0) {
 return NO_ERROR;
 }

 if ((offset > parcel->mDataSize)
 || (len > parcel->mDataSize)
 || (offset + len > parcel->mDataSize)) {
 return BAD_VALUE;
 }


     for (int i = 0; i < (int) size; i++) {
         size_t off = objects[i];
        if ((off >= offset) && (off + sizeof(flat_binder_object) <= offset + len)) {
             if (firstIndex == -1) {
                 firstIndex = i;
             }
            lastIndex = i;
 }
 }
 int numObjects = lastIndex - firstIndex + 1;

 if ((mDataSize+len) > mDataCapacity) {
        err = growData(len);
 if (err != NO_ERROR) {
 return err;
 }
 }

    memcpy(mData + mDataPos, data + offset, len);
    mDataPos += len;
    mDataSize += len;

    err = NO_ERROR;

 if (numObjects > 0) {
 if (mObjectsCapacity < mObjectsSize + numObjects) {
 int newSize = ((mObjectsSize + numObjects)*3)/2;
 binder_size_t *objects =
 (binder_size_t*)realloc(mObjects, newSize*sizeof(binder_size_t));
 if (objects == (binder_size_t*)0) {
 return NO_MEMORY;
 }
            mObjects = objects;
            mObjectsCapacity = newSize;
 }

 int idx = mObjectsSize;
 for (int i = firstIndex; i <= lastIndex; i++) {
 size_t off = objects[i] - offset + startPos;
            mObjects[idx++] = off;
            mObjectsSize++;

            flat_binder_object* flat
 = reinterpret_cast<flat_binder_object*>(mData + off);
            acquire_object(proc, *flat, this);

 if (flat->type == BINDER_TYPE_FD) {
                flat->handle = dup(flat->handle);
                flat->cookie = 1;
                mHasFds = mFdsKnown = true;
 if (!mAllowFds) {
                    err = FDS_NOT_ALLOWED;
 }
 }
 }
 }

 return err;
}
