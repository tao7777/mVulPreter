  void SaveTestFileSystem() {
    GDataRootDirectoryProto root;
    GDataDirectoryProto* root_dir = root.mutable_gdata_directory();
     GDataEntryProto* file_base = root_dir->mutable_gdata_entry();
     PlatformFileInfoProto* platform_info = file_base->mutable_file_info();
     file_base->set_title("drive");
    file_base->set_resource_id(kGDataRootDirectoryResourceId);
     platform_info->set_is_directory(true);
 
    GDataFileProto* file = root_dir->add_child_files();
    file_base = file->mutable_gdata_entry();
    platform_info = file_base->mutable_file_info();
    file_base->set_title("File1");
    platform_info->set_is_directory(false);
    platform_info->set_size(1048576);

    GDataDirectoryProto* dir1 = root_dir->add_child_directories();
    file_base = dir1->mutable_gdata_entry();
    platform_info = file_base->mutable_file_info();
    file_base->set_title("Dir1");
    platform_info->set_is_directory(true);

    file = dir1->add_child_files();
    file_base = file->mutable_gdata_entry();
    platform_info = file_base->mutable_file_info();
    file_base->set_title("File2");
    platform_info->set_is_directory(false);
    platform_info->set_size(555);

    GDataDirectoryProto* dir2 = dir1->add_child_directories();
    file_base = dir2->mutable_gdata_entry();
    platform_info = file_base->mutable_file_info();
    file_base->set_title("SubDir2");
    platform_info->set_is_directory(true);

    file = dir2->add_child_files();
    file_base = file->mutable_gdata_entry();
    platform_info = file_base->mutable_file_info();
    file_base->set_title("File3");
    platform_info->set_is_directory(false);
    platform_info->set_size(12345);

    std::string serialized_proto;
    ASSERT_TRUE(root.SerializeToString(&serialized_proto));
    ASSERT_TRUE(!serialized_proto.empty());

    FilePath cache_dir_path = profile_->GetPath().Append(
        FILE_PATH_LITERAL("GCache/v1/meta/"));
    ASSERT_TRUE(file_util::CreateDirectory(cache_dir_path));
    const int file_size = static_cast<int>(serialized_proto.length());
    ASSERT_EQ(file_util::WriteFile(cache_dir_path.Append("file_system.pb"),
        serialized_proto.data(), file_size), file_size);
  }
