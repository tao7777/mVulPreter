void OMX::binderDied(const wp<IBinder> &the_late_who) {
 OMXNodeInstance *instance;

 {

         Mutex::Autolock autoLock(mLock);
 
         ssize_t index = mLiveNodes.indexOfKey(the_late_who);

        if (index < 0) {
            ALOGE("b/27597103, nonexistent observer on binderDied");
            android_errorWriteLog(0x534e4554, "27597103");
            return;
        }
 
         instance = mLiveNodes.editValueAt(index);
         mLiveNodes.removeItemsAt(index);

        index = mDispatchers.indexOfKey(instance->nodeID());
        CHECK(index >= 0);
        mDispatchers.removeItemsAt(index);

        invalidateNodeID_l(instance->nodeID());
 }

    instance->onObserverDied(mMaster);
}
