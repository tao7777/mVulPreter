bool TypedUrlModelAssociator::AssociateModels() {
  VLOG(1) << "Associating TypedUrl Models";
  DCHECK(expected_loop_ == MessageLoop::current());

  std::vector<history::URLRow> typed_urls;
  if (!history_backend_->GetAllTypedURLs(&typed_urls)) {
    LOG(ERROR) << "Could not get the typed_url entries.";
    return false;
  }

  std::map<history::URLID, history::VisitVector> visit_vectors;
  for (std::vector<history::URLRow>::iterator ix = typed_urls.begin();
       ix != typed_urls.end(); ++ix) {
    if (!history_backend_->GetVisitsForURL(ix->id(),
                                           &(visit_vectors[ix->id()]))) {
      LOG(ERROR) << "Could not get the url's visits.";
      return false;
    }
    if (visit_vectors[ix->id()].empty()) {
      history::VisitRow visit(
          ix->id(), ix->last_visit(), 0, PageTransition::TYPED, 0);
      visit_vectors[ix->id()].push_back(visit);
    }
  }

  TypedUrlTitleVector titles;
  TypedUrlVector new_urls;
  TypedUrlVisitVector new_visits;
  TypedUrlUpdateVector updated_urls;

  {
    sync_api::WriteTransaction trans(sync_service_->GetUserShare());
    sync_api::ReadNode typed_url_root(&trans);
    if (!typed_url_root.InitByTagLookup(kTypedUrlTag)) {
      LOG(ERROR) << "Server did not create the top-level typed_url node. We "
                 << "might be running against an out-of-date server.";
      return false;
    }

    std::set<std::string> current_urls;
    for (std::vector<history::URLRow>::iterator ix = typed_urls.begin();
         ix != typed_urls.end(); ++ix) {
      std::string tag = ix->url().spec();

      history::VisitVector& visits = visit_vectors[ix->id()];

      sync_api::ReadNode node(&trans);
      if (node.InitByClientTagLookup(syncable::TYPED_URLS, tag)) {
        const sync_pb::TypedUrlSpecifics& typed_url(
            node.GetTypedUrlSpecifics());
        DCHECK_EQ(tag, typed_url.url());

        history::URLRow new_url(*ix);

        std::vector<history::VisitInfo> added_visits;
        int difference = MergeUrls(typed_url, *ix, &visits, &new_url,
                                   &added_visits);
        if (difference & DIFF_UPDATE_NODE) {
          sync_api::WriteNode write_node(&trans);
          if (!write_node.InitByClientTagLookup(syncable::TYPED_URLS, tag)) {
            LOG(ERROR) << "Failed to edit typed_url sync node.";
            return false;
          }
          if (typed_url.visits_size() > 0) {
            base::Time earliest_visit =
                base::Time::FromInternalValue(typed_url.visits(0));
            for (history::VisitVector::iterator it = visits.begin();
                 it != visits.end() && it->visit_time < earliest_visit; ) {
              it = visits.erase(it);
            }
             DCHECK(visits.size() > 0);
           }
           WriteToSyncNode(new_url, visits, &write_node);
         }
        if (difference & DIFF_LOCAL_TITLE_CHANGED) {
          titles.push_back(std::pair<GURL, string16>(new_url.url(),
                                                     new_url.title()));
        }
        if (difference & DIFF_LOCAL_ROW_CHANGED) {
          updated_urls.push_back(
              std::pair<history::URLID, history::URLRow>(ix->id(), new_url));
        }
        if (difference & DIFF_LOCAL_VISITS_ADDED) {
          new_visits.push_back(
              std::pair<GURL, std::vector<history::VisitInfo> >(ix->url(),
                                                                added_visits));
        }

        Associate(&tag, node.GetId());
      } else {
        sync_api::WriteNode node(&trans);
        if (!node.InitUniqueByCreation(syncable::TYPED_URLS,
                                       typed_url_root, tag)) {
          LOG(ERROR) << "Failed to create typed_url sync node.";
          return false;
        }

        node.SetTitle(UTF8ToWide(tag));
        WriteToSyncNode(*ix, visits, &node);

        Associate(&tag, node.GetId());
      }

      current_urls.insert(tag);
    }

    int64 sync_child_id = typed_url_root.GetFirstChildId();
    while (sync_child_id != sync_api::kInvalidId) {
      sync_api::ReadNode sync_child_node(&trans);
      if (!sync_child_node.InitByIdLookup(sync_child_id)) {
        LOG(ERROR) << "Failed to fetch child node.";
        return false;
      }
       const sync_pb::TypedUrlSpecifics& typed_url(
         sync_child_node.GetTypedUrlSpecifics());
 
      sync_child_id = sync_child_node.GetSuccessorId();

      // Ignore old sync nodes that don't have any transition data stored with
      // them.
      if (typed_url.visit_transitions_size() == 0) {
        VLOG(1) << "Ignoring obsolete sync node with no visit transition info.";
        continue;
      }

       if (current_urls.find(typed_url.url()) == current_urls.end()) {
         new_visits.push_back(
             std::pair<GURL, std::vector<history::VisitInfo> >(
                GURL(typed_url.url()),
                std::vector<history::VisitInfo>()));
        std::vector<history::VisitInfo>& visits = new_visits.back().second;
        history::URLRow new_url(GURL(typed_url.url()));
        TypedUrlModelAssociator::UpdateURLRowFromTypedUrlSpecifics(
            typed_url, &new_url);

        for (int c = 0; c < typed_url.visits_size(); ++c) {
          DCHECK(c == 0 || typed_url.visits(c) > typed_url.visits(c - 1));
          DCHECK_LE(typed_url.visit_transitions(c),
                    static_cast<int>(PageTransition::LAST_CORE));
          visits.push_back(history::VisitInfo(
              base::Time::FromInternalValue(typed_url.visits(c)),
              static_cast<PageTransition::Type>(
                  typed_url.visit_transitions(c))));
        }

         Associate(&typed_url.url(), sync_child_node.GetId());
         new_urls.push_back(new_url);
       }
     }
   }
 
  return WriteToHistoryBackend(&titles, &new_urls, &updated_urls,
                               &new_visits, NULL);
}
