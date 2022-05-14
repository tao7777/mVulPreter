FLAC__StreamDecoderWriteStatus FLACParser::writeCallback(
 const FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
 if (mWriteRequested) {

         mWriteRequested = false;
         mWriteHeader = frame->header;
        mWriteBuffer = buffer;
         mWriteCompleted = true;
         return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
     } else {
        ALOGE("FLACParser::writeCallback unexpected");
 return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
 }
}
