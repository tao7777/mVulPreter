nsPluginInstance::setupCookies(const std::string& pageurl)
{
    std::string::size_type pos;
    pos = pageurl.find("/", pageurl.find("//", 0) + 2) + 1;
    std::string url = pageurl.substr(0, pos);

    std::string ncookie;
 
    char *cookie = 0;
    uint32_t length = 0;

    NPError rv = NPERR_GENERIC_ERROR;
#if NPAPI_VERSION != 190
    if (NPNFuncs.getvalueforurl) {
        rv = NPN_GetValueForURL(_instance, NPNURLVCookie, url.c_str(),
                                &cookie, &length);
    } else {
        LOG_ONCE( gnash::log_debug("Browser doesn't support getvalueforurl") );
    }
#endif

    if (rv == NPERR_GENERIC_ERROR) {
        log_debug("Trying window.document.cookie for cookies");
        ncookie = getDocumentProp("cookie");
    }

    if (cookie) {
        ncookie.assign(cookie, length);
        NPN_MemFree(cookie);
    }

    if (ncookie.empty()) {
        gnash::log_debug("No stored Cookie for %s", url);
        return;
    }

    gnash::log_debug("The Cookie for %s is %s", url, ncookie);
    std::ofstream cookiefile;
    std::stringstream ss;
     ss << "/tmp/gnash-cookies." << getpid();
 
     cookiefile.open(ss.str().c_str(), std::ios::out | std::ios::trunc);
    chmod (ss.str().c_str(), 0600);
 
  
    typedef boost::char_separator<char> char_sep;
    typedef boost::tokenizer<char_sep> tokenizer;
    tokenizer tok(ncookie, char_sep(";"));

    for (tokenizer::iterator it=tok.begin(); it != tok.end(); ++it) {
        cookiefile << "Set-Cookie: " << *it << std::endl;
    }
 
    cookiefile.close();
  
    if (setenv("GNASH_COOKIES_IN", ss.str().c_str(), 1) < 0) {
        gnash::log_error(
            "Couldn't set environment variable GNASH_COOKIES_IN to %s",
            ncookie);
    }
}
