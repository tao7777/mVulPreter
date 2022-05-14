 base::string16 FormatBookmarkURLForDisplay(const GURL& url) {
  return url_formatter::FormatUrl(
      url, url_formatter::kFormatUrlOmitAll &
               ~url_formatter::kFormatUrlOmitUsernamePassword,
      net::UnescapeRule::SPACES, nullptr, nullptr, nullptr);
 }
