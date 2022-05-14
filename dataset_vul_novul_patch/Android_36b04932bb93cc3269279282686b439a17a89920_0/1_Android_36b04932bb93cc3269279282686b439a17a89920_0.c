bool NuMediaExtractor::getTotalBitrate(int64_t *bitrate) const {
 if (mTotalBitrate >= 0) {
 *bitrate = mTotalBitrate;
 return true;

     }
 
     off64_t size;
    if (mDurationUs >= 0 && mDataSource->getSize(&size) == OK) {
         *bitrate = size * 8000000ll / mDurationUs;  // in bits/sec
         return true;
     }

 return false;
}
