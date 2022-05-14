void SetBuildInfoAnnotations(std::map<std::string, std::string>* annotations) {
  base::android::BuildInfo* info = base::android::BuildInfo::GetInstance();
 
   (*annotations)["android_build_id"] = info->android_build_id();
   (*annotations)["android_build_fp"] = info->android_build_fp();
  (*annotations)["sdk"] = base::StringPrintf("%d", info->sdk_int());
   (*annotations)["device"] = info->device();
   (*annotations)["model"] = info->model();
   (*annotations)["brand"] = info->brand();
  (*annotations)["board"] = info->board();
  (*annotations)["installer_package_name"] = info->installer_package_name();
  (*annotations)["abi_name"] = info->abi_name();
  (*annotations)["custom_themes"] = info->custom_themes();
  (*annotations)["resources_verison"] = info->resources_version();
  (*annotations)["gms_core_version"] = info->gms_version_code();

  if (info->firebase_app_id()[0] != '\0') {
    (*annotations)["package"] = std::string(info->firebase_app_id()) + " v" +
                                info->package_version_code() + " (" +
                                info->package_version_name() + ")";
  }
}
