 SoftAACEncoder2::~SoftAACEncoder2() {
     aacEncClose(&mAACEncoder);
 
    onReset();
 }
