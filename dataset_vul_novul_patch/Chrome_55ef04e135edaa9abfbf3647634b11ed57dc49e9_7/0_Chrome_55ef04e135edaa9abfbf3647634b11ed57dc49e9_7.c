bool ResourceTracker::UnrefResource(PP_Resource res) {
  DLOG_IF(ERROR, !CheckIdType(res, PP_ID_TYPE_RESOURCE))
      << res << " is not a PP_Resource.";
  ResourceMap::iterator i = live_resources_.find(res);
  if (i != live_resources_.end()) {
    if (!--i->second.second) {
      Resource* to_release = i->second.first;
       PP_Instance instance = to_release->instance()->pp_instance();
      to_release->LastPluginRefWasDeleted();
 
      instance_map_[instance]->ref_resources.erase(res);
       live_resources_.erase(i);
     }
     return true;
  } else {
    return false;
  }
}
