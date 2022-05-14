bool GDataRootDirectory::ParseFromString(const std::string& serialized_proto) {
  scoped_ptr<GDataRootDirectoryProto> proto(
       new GDataRootDirectoryProto());
   bool ok = proto->ParseFromString(serialized_proto);
   if (ok) {
    const GDataEntryProto& entry_proto =
        proto->gdata_directory().gdata_entry();
    if (entry_proto.title() != "drive") {
      LOG(ERROR) << "Incompatible proto detected (bad title): "
                 << entry_proto.title();
      return false;
    }
    // The title field for the root directory was originally empty. Discard
    // the proto data if the older format is detected.
    if (entry_proto.resource_id() != kGDataRootDirectoryResourceId) {
      LOG(ERROR) << "Incompatible proto detected (bad resource ID): "
                 << entry_proto.resource_id();
       return false;
     }
 
    FromProto(*proto.get());
    set_origin(FROM_CACHE);
    set_refresh_time(base::Time::Now());
  }
  return ok;
}
