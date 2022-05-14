 base::string16 FormatBookmarkURLForDisplay(const GURL& url) {
  // and trailing slash, and unescape most characters. However, it's
  url_formatter::FormatUrlTypes format_types =
      url_formatter::kFormatUrlOmitAll &
      ~url_formatter::kFormatUrlOmitUsernamePassword;

  // If username is present, we must not omit the scheme because FixupURL() will
  // subsequently interpret the username as a scheme. crbug.com/639126
  if (url.has_username())
    format_types &= ~url_formatter::kFormatUrlOmitHTTP;

  return url_formatter::FormatUrl(url, format_types, net::UnescapeRule::SPACES,
                                  nullptr, nullptr, nullptr);
 }
