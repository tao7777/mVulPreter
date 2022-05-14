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
 
    // Allow for closing '\0'
    ssize_t utf8Size = utf16_to_utf8_length(src, utf16Size) + 1;
    if (utf8Size < 1) {
         return BAD_VALUE;
     }
    // spare byte around for the trailing null, we still pass the size including the trailing null
     str->resize(utf8Size);
    utf16_to_utf8(src, utf16Size, &((*str)[0]), utf8Size);
    str->resize(utf8Size - 1);
     return NO_ERROR;
 }
