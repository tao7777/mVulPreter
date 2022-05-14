status_t ESDS::parseESDescriptor(size_t offset, size_t size) {
 if (size < 3) {
 return ERROR_MALFORMED;
 }

    offset += 2; // skip ES_ID
    size -= 2;

 unsigned streamDependenceFlag = mData[offset] & 0x80;
 unsigned URL_Flag = mData[offset] & 0x40;
 unsigned OCRstreamFlag = mData[offset] & 0x20;

 ++offset;

     --size;
 
     if (streamDependenceFlag) {
        if (size < 2)
            return ERROR_MALFORMED;
         offset += 2;
         size -= 2;
     }

 if (URL_Flag) {
 if (offset >= size) {

             return ERROR_MALFORMED;
         }
         unsigned URLlength = mData[offset];
        if (URLlength >= size)
            return ERROR_MALFORMED;
         offset += URLlength + 1;
         size -= URLlength + 1;
     }
 
     if (OCRstreamFlag) {
        if (size < 2)
            return ERROR_MALFORMED;
         offset += 2;
         size -= 2;
 
 if ((offset >= size || mData[offset] != kTag_DecoderConfigDescriptor)
 && offset - 2 < size
 && mData[offset - 2] == kTag_DecoderConfigDescriptor) {
            offset -= 2;
            size += 2;

            ALOGW("Found malformed 'esds' atom, ignoring missing OCR_ES_Id.");
 }
 }

 if (offset >= size) {
 return ERROR_MALFORMED;
 }

 uint8_t tag;
 size_t sub_offset, sub_size;
 status_t err = skipDescriptorHeader(
            offset, size, &tag, &sub_offset, &sub_size);

 if (err != OK) {
 return err;
 }

 if (tag != kTag_DecoderConfigDescriptor) {
 return ERROR_MALFORMED;
 }

    err = parseDecoderConfigDescriptor(sub_offset, sub_size);

 return err;
}
