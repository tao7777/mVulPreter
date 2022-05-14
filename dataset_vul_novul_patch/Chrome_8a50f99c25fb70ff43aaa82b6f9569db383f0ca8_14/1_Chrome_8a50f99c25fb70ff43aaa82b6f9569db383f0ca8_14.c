ObjectIdPayloadMap ObjectIdsAndPayloadToMap(const ObjectIdSet& ids,
                                            const std::string& payload) {
  ObjectIdPayloadMap id_payloads;
  for (ObjectIdSet::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    id_payloads[*it] = payload;
   }
  return id_payloads;
}
