ObjectIdPayloadMap ObjectIdsAndPayloadToMap(const ObjectIdSet& ids,
  int GetInvalidationCount(const ObjectId& id) const {
    ObjectIdCountMap::const_iterator it = invalidation_counts_.find(id);
    return (it == invalidation_counts_.end()) ? 0 : it->second;
   }
