     void registerURL(const char* file, const char* mimeType)
     {
        registerMockedURLLoad(KURL(m_baseUrl, file), WebString::fromUTF8(file), m_folder, WebString::fromUTF8(mimeType));
     }
