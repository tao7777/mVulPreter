std::string utf16ToUtf8(const StringPiece16& utf16) {
 ssize_t utf8Length = utf16_to_utf8_length(utf16.data(), utf16.length());
 if (utf8Length <= 0) {
 return {};

     }
 
     std::string utf8;
     utf8.resize(utf8Length);
    utf16_to_utf8(utf16.data(), utf16.length(), &*utf8.begin());
     return utf8;
 }
