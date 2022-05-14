    void registerURL(const char* url, const char* file, const char* mimeType)
    {
        registerMockedURLLoad(KURL(m_baseUrl, url), WebString::fromUTF8(file), m_folder, WebString::fromUTF8(mimeType));
    }
