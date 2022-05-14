void NuPlayer::GenericSource::notifyPreparedAndCleanup(status_t err) {
 if (err != OK) {
        mMetaDataSize = -1ll;
        mContentType = "";
        mSniffedMIME = "";
 {
            sp<DataSource> dataSource = mDataSource;
            sp<NuCachedSource2> cachedSource = mCachedSource;
            sp<DataSource> httpSource = mHttpSource;

             {
                 Mutex::Autolock _l(mDisconnectLock);
                 mDataSource.clear();
                mDrmManagerClient = NULL;
                 mCachedSource.clear();
                 mHttpSource.clear();
             }
 }

        cancelPollBuffering();
 }
    notifyPrepared(err);
}
