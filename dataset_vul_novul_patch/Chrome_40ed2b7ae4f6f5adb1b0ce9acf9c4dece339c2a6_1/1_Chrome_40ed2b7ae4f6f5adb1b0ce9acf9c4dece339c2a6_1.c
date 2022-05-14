bool GDataRootDirectory::ParseFromString(const std::string& serialized_proto) {
  scoped_ptr<GDataRootDirectoryProto> proto(
       new GDataRootDirectoryProto());
   bool ok = proto->ParseFromString(serialized_proto);
   if (ok) {
    const std::string& title = proto->gdata_directory().gdata_entry().title();
    if (title != "drive") {
      LOG(ERROR) << "Incompatible proto detected: " << title;
       return false;
     }
 
    FromProto(*proto.get());
    set_origin(FROM_CACHE);
    set_refresh_time(base::Time::Now());
  }
  return ok;
}
