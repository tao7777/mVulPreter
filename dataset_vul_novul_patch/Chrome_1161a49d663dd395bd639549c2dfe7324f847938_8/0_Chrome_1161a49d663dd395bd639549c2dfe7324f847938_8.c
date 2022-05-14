 bool OSExchangeDataProviderAuraX11::GetURLAndTitle(
    OSExchangeData::FilenameToURLPolicy policy,
     GURL* url,
     base::string16* title) const {
   std::vector< ::Atom> url_atoms = ui::GetURLAtomsFrom(&atom_cache_);
  std::vector< ::Atom> requested_types;
  ui::GetAtomIntersection(url_atoms, GetTargets(), &requested_types);

  ui::SelectionData data(format_map_.GetFirstOf(requested_types));
  if (data.IsValid()) {

    if (data.GetType() == atom_cache_.GetAtom(kMimeTypeMozillaURL)) {
      base::string16 unparsed;
      data.AssignTo(&unparsed);

      std::vector<base::string16> tokens;
      size_t num_tokens = Tokenize(unparsed, base::ASCIIToUTF16("\n"), &tokens);
      if (num_tokens > 0) {
        if (num_tokens > 1)
          *title = tokens[1];
        else
          *title = base::string16();

        *url = GURL(tokens[0]);
        return true;
      }
    } else if (data.GetType() == atom_cache_.GetAtom(
                   Clipboard::kMimeTypeURIList)) {
      std::vector<std::string> tokens = ui::ParseURIList(data);
       for (std::vector<std::string>::const_iterator it = tokens.begin();
            it != tokens.end(); ++it) {
         GURL test_url(*it);
        if (!test_url.SchemeIsFile() ||
            policy == OSExchangeData::CONVERT_FILENAMES) {
           *url = test_url;
           *title = base::string16();
           return true;
        }
      }
    }
  }

  return false;
}
