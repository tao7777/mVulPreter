std::string TemplateURLRef::HandleReplacements(
    const SearchTermsArgs& search_terms_args,
    const SearchTermsData& search_terms_data,
    PostContent* post_content) const {
  if (replacements_.empty()) {
    if (!post_params_.empty())
      EncodeFormData(post_params_, post_content);
    return parsed_url_;
  }

  bool is_in_query = true;
  for (Replacements::iterator i = replacements_.begin();
       i != replacements_.end(); ++i) {
    if (i->type == SEARCH_TERMS) {
      base::string16::size_type query_start = parsed_url_.find('?');
      is_in_query = query_start != base::string16::npos &&
          (static_cast<base::string16::size_type>(i->index) > query_start);
      break;
    }
  }

  std::string input_encoding;
  base::string16 encoded_terms;
  base::string16 encoded_original_query;
  owner_->EncodeSearchTerms(search_terms_args, is_in_query, &input_encoding,
                            &encoded_terms, &encoded_original_query);

  std::string url = parsed_url_;

  for (Replacements::reverse_iterator i = replacements_.rbegin();
       i != replacements_.rend(); ++i) {
    switch (i->type) {
      case ENCODING:
        HandleReplacement(std::string(), input_encoding, *i, &url);
        break;

      case GOOGLE_ASSISTED_QUERY_STATS:
        DCHECK(!i->is_post_param);
        if (!search_terms_args.assisted_query_stats.empty()) {
          SearchTermsArgs search_terms_args_without_aqs(search_terms_args);
          search_terms_args_without_aqs.assisted_query_stats.clear();
          GURL base_url(ReplaceSearchTerms(
              search_terms_args_without_aqs, search_terms_data, NULL));
          if (base_url.SchemeIsCryptographic()) {
            HandleReplacement(
                "aqs", search_terms_args.assisted_query_stats, *i, &url);
          }
        }
        break;

      case GOOGLE_BASE_URL:
        DCHECK(!i->is_post_param);
        HandleReplacement(
            std::string(), search_terms_data.GoogleBaseURLValue(), *i, &url);
        break;

      case GOOGLE_BASE_SUGGEST_URL:
        DCHECK(!i->is_post_param);
        HandleReplacement(
            std::string(), search_terms_data.GoogleBaseSuggestURLValue(), *i,
            &url);
        break;

      case GOOGLE_CURRENT_PAGE_URL:
        DCHECK(!i->is_post_param);
        if (!search_terms_args.current_page_url.empty()) {
          const std::string& escaped_current_page_url =
              net::EscapeQueryParamValue(search_terms_args.current_page_url,
                                         true);
          HandleReplacement("url", escaped_current_page_url, *i, &url);
        }
        break;

      case GOOGLE_CURSOR_POSITION:
        DCHECK(!i->is_post_param);
        if (search_terms_args.cursor_position != base::string16::npos)
          HandleReplacement(
              "cp",
              base::StringPrintf("%" PRIuS, search_terms_args.cursor_position),
              *i,
              &url);
        break;

      case GOOGLE_FORCE_INSTANT_RESULTS:
        DCHECK(!i->is_post_param);
        HandleReplacement(std::string(),
                          search_terms_data.ForceInstantResultsParam(
                              search_terms_args.force_instant_results),
                          *i,
                          &url);
        break;

      case GOOGLE_INPUT_TYPE:
        DCHECK(!i->is_post_param);
        HandleReplacement(
            "oit", base::IntToString(search_terms_args.input_type), *i, &url);
        break;

      case GOOGLE_INSTANT_EXTENDED_ENABLED:
        DCHECK(!i->is_post_param);
        HandleReplacement(std::string(),
                          search_terms_data.InstantExtendedEnabledParam(
                              type_ == SEARCH),
                          *i,
                          &url);
        break;

      case GOOGLE_CONTEXTUAL_SEARCH_VERSION:
        if (search_terms_args.contextual_search_params.version >= 0) {
          HandleReplacement(
              "ctxs",
              base::IntToString(
                  search_terms_args.contextual_search_params.version),
              *i,
              &url);
        }
        break;

      case GOOGLE_CONTEXTUAL_SEARCH_CONTEXT_DATA: {
        DCHECK(!i->is_post_param);
        std::string context_data;

        const SearchTermsArgs::ContextualSearchParams& params =
            search_terms_args.contextual_search_params;

        if (params.start != std::string::npos) {
          context_data.append("ctxs_start=" +
                              base::SizeTToString(params.start) + "&");
        }

        if (params.end != std::string::npos) {
          context_data.append("ctxs_end=" +
                              base::SizeTToString(params.end) + "&");
        }

        if (!params.selection.empty())
          context_data.append("q=" + params.selection + "&");

        if (!params.content.empty())
          context_data.append("ctxs_content=" + params.content + "&");

        if (!params.base_page_url.empty())
          context_data.append("ctxsl_url=" + params.base_page_url + "&");

        if (!params.encoding.empty()) {
          context_data.append("ctxs_encoding=" + params.encoding + "&");
        }

         context_data.append("ctxsl_coca=" +
                            base::IntToString(
                                params.contextual_cards_version));
 
         HandleReplacement(std::string(), context_data, *i, &url);
         break;
      }

      case GOOGLE_ORIGINAL_QUERY_FOR_SUGGESTION:
        DCHECK(!i->is_post_param);
        if (search_terms_args.accepted_suggestion >= 0 ||
            !search_terms_args.assisted_query_stats.empty()) {
          HandleReplacement(
              "oq", base::UTF16ToUTF8(encoded_original_query), *i, &url);
        }
        break;

      case GOOGLE_PAGE_CLASSIFICATION:
        if (search_terms_args.page_classification !=
            metrics::OmniboxEventProto::INVALID_SPEC) {
          HandleReplacement(
              "pgcl", base::IntToString(search_terms_args.page_classification),
              *i, &url);
        }
        break;

      case GOOGLE_PREFETCH_QUERY: {
        const std::string& query = search_terms_args.prefetch_query;
        const std::string& type = search_terms_args.prefetch_query_type;
        if (!query.empty() && !type.empty()) {
          HandleReplacement(
              std::string(), "pfq=" + query + "&qha=" + type + "&", *i, &url);
        }
        break;
      }

      case GOOGLE_RLZ: {
        DCHECK(!i->is_post_param);
        base::string16 rlz_string = search_terms_data.GetRlzParameterValue(
            search_terms_args.from_app_list);
        if (!rlz_string.empty()) {
          HandleReplacement("rlz", base::UTF16ToUTF8(rlz_string), *i, &url);
        }
        break;
      }

      case GOOGLE_SEARCH_CLIENT: {
        DCHECK(!i->is_post_param);
        std::string client = search_terms_data.GetSearchClient();
        if (!client.empty())
          HandleReplacement("client", client, *i, &url);
        break;
      }

      case GOOGLE_SEARCH_FIELDTRIAL_GROUP:
        break;

      case GOOGLE_SEARCH_VERSION:
        HandleReplacement("gs_rn", "42", *i, &url);
        break;

      case GOOGLE_SESSION_TOKEN: {
        std::string token = search_terms_args.session_token;
        if (!token.empty())
          HandleReplacement("psi", token, *i, &url);
        break;
      }

      case GOOGLE_SUGGEST_CLIENT:
        HandleReplacement(
            std::string(), search_terms_data.GetSuggestClient(), *i, &url);
        break;

      case GOOGLE_SUGGEST_REQUEST_ID:
        HandleReplacement(
            std::string(), search_terms_data.GetSuggestRequestIdentifier(), *i,
            &url);
        break;

      case GOOGLE_UNESCAPED_SEARCH_TERMS: {
        std::string unescaped_terms;
        base::UTF16ToCodepage(search_terms_args.search_terms,
                              input_encoding.c_str(),
                              base::OnStringConversionError::SKIP,
                              &unescaped_terms);
        HandleReplacement(std::string(), unescaped_terms, *i, &url);
        break;
      }

      case LANGUAGE:
        HandleReplacement(
            std::string(), search_terms_data.GetApplicationLocale(), *i, &url);
        break;

      case SEARCH_TERMS:
        HandleReplacement(
            std::string(), base::UTF16ToUTF8(encoded_terms), *i, &url);
        break;

      case GOOGLE_IMAGE_THUMBNAIL:
        HandleReplacement(
            std::string(), search_terms_args.image_thumbnail_content, *i, &url);
        post_params_[i->index].content_type = "image/jpeg";
        break;

      case GOOGLE_IMAGE_URL:
        if (search_terms_args.image_url.is_valid()) {
          HandleReplacement(
              std::string(), search_terms_args.image_url.spec(), *i, &url);
        }
        break;

      case GOOGLE_IMAGE_ORIGINAL_WIDTH:
        if (!search_terms_args.image_original_size.IsEmpty()) {
          HandleReplacement(
              std::string(),
              base::IntToString(search_terms_args.image_original_size.width()),
              *i, &url);
        }
        break;

      case GOOGLE_IMAGE_ORIGINAL_HEIGHT:
        if (!search_terms_args.image_original_size.IsEmpty()) {
          HandleReplacement(
              std::string(),
              base::IntToString(search_terms_args.image_original_size.height()),
              *i, &url);
        }
        break;

      case GOOGLE_IMAGE_SEARCH_SOURCE:
        HandleReplacement(
            std::string(), search_terms_data.GoogleImageSearchSource(), *i,
            &url);
        break;

      case GOOGLE_IOS_SEARCH_LANGUAGE:
#if defined(OS_IOS)
        HandleReplacement("hl", search_terms_data.GetApplicationLocale(), *i,
                          &url);
#endif
        break;

      default:
        NOTREACHED();
        break;
    }
  }

  if (!post_params_.empty())
    EncodeFormData(post_params_, post_content);

  return url;
}
