OMXNodeInstance::OMXNodeInstance(
        OMX *owner, const sp<IOMXObserver> &observer, const char *name)
 : mOwner(owner),
      mNodeID(0),

       mHandle(NULL),
       mObserver(observer),
       mDying(false),
       mBufferIDCount(0)
 {
     mName = ADebug::GetDebugName(name);
    DEBUG = ADebug::GetDebugLevelFromProperty(name, "debug.stagefright.omx-debug");
    ALOGV("debug level for %s is %d", name, DEBUG);
    DEBUG_BUMP = DEBUG;
    mNumPortBuffers[0] = 0;
    mNumPortBuffers[1] = 0;
    mDebugLevelBumpPendingBuffers[0] = 0;
    mDebugLevelBumpPendingBuffers[1] = 0;
    mMetadataType[0] = kMetadataBufferTypeInvalid;
    mMetadataType[1] = kMetadataBufferTypeInvalid;
    mSecureBufferType[0] = kSecureBufferTypeUnknown;
    mSecureBufferType[1] = kSecureBufferTypeUnknown;
    mIsSecure = AString(name).endsWith(".secure");
}
