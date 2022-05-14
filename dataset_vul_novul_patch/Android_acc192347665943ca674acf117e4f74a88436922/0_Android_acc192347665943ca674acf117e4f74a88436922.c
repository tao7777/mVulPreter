FLACParser::FLACParser(
 const sp<DataSource> &dataSource,
 const sp<MetaData> &fileMetadata,
 const sp<MetaData> &trackMetadata)
 : mDataSource(dataSource),
      mFileMetadata(fileMetadata),
      mTrackMetadata(trackMetadata),
      mInitCheck(false),
      mMaxBufferSize(0),
      mGroup(NULL),
      mCopy(copyTrespass),
      mDecoder(NULL),
      mCurrentPos(0LL),
      mEOF(false),

       mStreamInfoValid(false),
       mWriteRequested(false),
       mWriteCompleted(false),
       mErrorStatus((FLAC__StreamDecoderErrorStatus) -1)
 {
     ALOGV("FLACParser::FLACParser");
    memset(&mStreamInfo, 0, sizeof(mStreamInfo));
    memset(&mWriteHeader, 0, sizeof(mWriteHeader));
    mInitCheck = init();
}
