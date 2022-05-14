 PP_Resource ResourceTracker::AddResource(Resource* resource) {
   if (last_resource_id_ ==
      (std::numeric_limits<PP_Resource>::max() >> kPPIdTypeBits))
    return 0;

  PP_Resource new_id = MakeTypedId(++last_resource_id_, PP_ID_TYPE_RESOURCE);
  live_resources_.insert(std::make_pair(new_id, std::make_pair(resource, 1)));

   PP_Instance pp_instance = resource->instance()->pp_instance();
   DCHECK(instance_map_.find(pp_instance) != instance_map_.end());
  instance_map_[pp_instance]->ref_resources.insert(new_id);
   return new_id;
 }
