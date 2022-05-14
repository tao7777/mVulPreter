scoped_ptr<GDataEntry> GDataDirectoryService::FromProtoString(
    const std::string& serialized_proto) {
  GDataEntryProto entry_proto;
  if (!entry_proto.ParseFromString(serialized_proto))
    return scoped_ptr<GDataEntry>();
 
   scoped_ptr<GDataEntry> entry;
   if (entry_proto.file_info().is_directory()) {
    entry.reset(CreateGDataDirectory());
     if (!entry->FromProto(entry_proto)) {
       NOTREACHED() << "FromProto (directory) failed";
       entry.reset();
     }
   } else {
    scoped_ptr<GDataFile> file(CreateGDataFile());
     if (file->FromProto(entry_proto)) {
       entry.reset(file.release());
    } else {
      NOTREACHED() << "FromProto (file) failed";
    }
  }
  return entry.Pass();
}
