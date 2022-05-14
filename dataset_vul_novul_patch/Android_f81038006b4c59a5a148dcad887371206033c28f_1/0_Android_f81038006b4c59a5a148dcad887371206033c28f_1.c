sp<MediaSource> MPEG4Extractor::getTrack(size_t index) {
 status_t err;
 if ((err = readMetaData()) != OK) {
 return NULL;
 }

 Track *track = mFirstTrack;
 while (index > 0) {
 if (track == NULL) {
 return NULL;
 }

        track = track->next;
 --index;
 }

 if (track == NULL) {
 return NULL;
 }


 Trex *trex = NULL;
 int32_t trackId;
 if (track->meta->findInt32(kKeyTrackID, &trackId)) {
 for (size_t i = 0; i < mTrex.size(); i++) {
 Trex *t = &mTrex.editItemAt(index);
 if (t->track_ID == (uint32_t) trackId) {
                trex = t;

                 break;
             }
         }
    } else {
        ALOGE("b/21657957");
        return NULL;
     }
 
     ALOGV("getTrack called, pssh: %zu", mPssh.size());

 return new MPEG4Source(this,
            track->meta, mDataSource, track->timescale, track->sampleTable,
            mSidxEntries, trex, mMoofOffset);
}
