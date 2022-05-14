 SoftAACEncoder::~SoftAACEncoder() {
    delete[] mInputFrame;
    mInputFrame = NULL;
 
     if (mEncoderHandle) {
         CHECK_EQ(VO_ERR_NONE, mApiHandle->Uninit(mEncoderHandle));
        mEncoderHandle = NULL;
 }

 delete mApiHandle;
    mApiHandle = NULL;

 delete mMemOperator;
    mMemOperator = NULL;
}
