 void VP8XChunk::height(XMP_Uns32 val)
 {
    PutLE24(&this->data[7], val > 0 ? val - 1 : 0);
 }
