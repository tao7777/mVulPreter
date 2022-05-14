ObjectIdSet MakeSetFromId(const invalidation::ObjectId& id) {
  ObjectIdSet ids;
  ids.insert(id);
  return ids;
}
