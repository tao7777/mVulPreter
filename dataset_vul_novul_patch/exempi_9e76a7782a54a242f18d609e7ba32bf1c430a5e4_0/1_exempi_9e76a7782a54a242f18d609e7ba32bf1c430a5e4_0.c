 void VP8XChunk::width(XMP_Uns32 val)
 {
    PutLE24(&this->data[4], val - 1);
 }
