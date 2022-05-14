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
 
#if PLATFORM(MAC)
    // FIXME: Move this to ArgumentCodersCFMac.mm and change this file back to be C++
    // instead of Objective-C++.
    if (!CFStringGetLength(string.get())) {
        // CFURL can't hold an empty URL, unlike NSURL.
        result = reinterpret_cast<CFURLRef>([NSURL URLWithString:@""]);
        return true;
    }
#endif
                    
     CFURLRef url = CFURLCreateWithString(0, string.get(), baseURL.get());
     if (!url)
         return false;

    result.adoptCF(url);
    return true;
}
