status_t MediaHTTP::connect(
 const char *uri,
 const KeyedVector<String8, String8> *headers,
 off64_t /* offset */) {
 if (mInitCheck != OK) {
 return mInitCheck;
 }

 KeyedVector<String8, String8> extHeaders;
 if (headers != NULL) {
        extHeaders = *headers;
 }

 if (extHeaders.indexOfKey(String8("User-Agent")) < 0) {

         extHeaders.add(String8("User-Agent"), String8(MakeUserAgent().c_str()));
     }
 
    mLastURI = uri;
    // reconnect() calls with uri == old mLastURI.c_str(), which gets zapped
    // as part of the above assignment. Ensure no accidental later use.
    uri = NULL;

    bool success = mHTTPConnection->connect(mLastURI.c_str(), &extHeaders);
 
     mLastHeaders = extHeaders;
 
     mCachedSizeValid = false;
 
     if (success) {
        AString sanitized = uriDebugString(mLastURI);
         mName = String8::format("MediaHTTP(%s)", sanitized.c_str());
     }
 
 return success ? OK : UNKNOWN_ERROR;
}
