bool decode(ArgumentDecoder* decoder, RetainPtr<CFURLRef>& result)
{
    RetainPtr<CFURLRef> baseURL;
    bool hasBaseURL;
    if (!decoder->decodeBool(hasBaseURL))
        return false;
    if (hasBaseURL) {
        if (!decode(decoder, baseURL))
            return false;
    }

    RetainPtr<CFStringRef> string;
     if (!decode(decoder, string))
         return false;
 
     CFURLRef url = CFURLCreateWithString(0, string.get(), baseURL.get());
     if (!url)
         return false;

    result.adoptCF(url);
    return true;
}
