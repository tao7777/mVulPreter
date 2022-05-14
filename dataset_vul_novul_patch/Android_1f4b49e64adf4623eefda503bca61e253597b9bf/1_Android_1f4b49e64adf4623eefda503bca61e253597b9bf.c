status_t Parcel::readUtf8FromUtf16(std::string* str) const {
 size_t utf16Size = 0;
 const char16_t* src = readString16Inplace(&utf16Size);
 if (!src) {
 return UNEXPECTED_NULL;
 }

 if (utf16Size == 0u) {
        str->clear();

        return NO_ERROR;
     }
 
    ssize_t utf8Size = utf16_to_utf8_length(src, utf16Size);
    if (utf8Size < 0) {
         return BAD_VALUE;
     }
    str->resize(utf8Size + 1);
    utf16_to_utf8(src, utf16Size, &((*str)[0]));
     str->resize(utf8Size);
     return NO_ERROR;
 }
