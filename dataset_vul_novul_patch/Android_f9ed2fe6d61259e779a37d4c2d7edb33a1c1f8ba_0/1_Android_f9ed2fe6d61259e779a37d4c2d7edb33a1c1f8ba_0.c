void OMX::binderDied(const wp<IBinder> &the_late_who) {
 OMXNodeInstance *instance;

 {

         Mutex::Autolock autoLock(mLock);
 
         ssize_t index = mLiveNodes.indexOfKey(the_late_who);
        CHECK(index >= 0);
 
         instance = mLiveNodes.editValueAt(index);
         mLiveNodes.removeItemsAt(index);

        index = mDispatchers.indexOfKey(instance->nodeID());
        CHECK(index >= 0);
        mDispatchers.removeItemsAt(index);

        invalidateNodeID_l(instance->nodeID());
 }

    instance->onObserverDied(mMaster);
}
