CString fileSystemRepresentation(const String&)
CString fileSystemRepresentation(const String& path)
 {
    const UChar* characters = path.characters();
    int size = WideCharToMultiByte(CP_ACP, 0, characters, path.length(), 0, 0, 0, 0) - 1;

    char* buffer;
    CString string = CString::newUninitialized(size, buffer);

    WideCharToMultiByte(CP_ACP, 0, characters, path.length(), buffer, size, 0, 0);

    return string;
 }
