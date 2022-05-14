 Resource::Resource(PluginInstance* instance)
     : resource_id_(0), instance_(instance) {
  ResourceTracker::Get()->ResourceCreated(this, instance_);
 }
