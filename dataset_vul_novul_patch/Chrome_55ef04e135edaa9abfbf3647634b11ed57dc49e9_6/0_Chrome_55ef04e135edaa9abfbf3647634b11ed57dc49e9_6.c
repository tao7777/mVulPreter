uint32 ResourceTracker::GetLiveObjectsForInstance(
    PP_Instance instance) const {
   InstanceMap::const_iterator found = instance_map_.find(instance);
   if (found == instance_map_.end())
     return 0;
  return static_cast<uint32>(found->second->ref_resources.size() +
                              found->second->object_vars.size());
 }
