bool xmp_init()
{
     RESET_ERROR;
     try {
         bool result = SXMPFiles::Initialize(kXMPFiles_IgnoreLocalText);
         SXMPMeta::SetDefaultErrorCallback(&_xmp_error_callback, nullptr, 1);
         return result;
        SXMPMeta::SetDefaultErrorCallback(&_xmp_error_callback, nullptr, 1);
        return result;
    }
    catch (const XMP_Error &e) {
        set_error(e);
    }
    return false;
}
