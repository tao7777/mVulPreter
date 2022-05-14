ChooserContextBase::GetGrantedObjects(const GURL& requesting_origin,
                                      const GURL& embedding_origin) {
  DCHECK_EQ(requesting_origin, requesting_origin.GetOrigin());
  DCHECK_EQ(embedding_origin, embedding_origin.GetOrigin());

  if (!CanRequestObjectPermission(requesting_origin, embedding_origin))
     return {};
 
   std::vector<std::unique_ptr<Object>> results;
  content_settings::SettingInfo info;
   std::unique_ptr<base::DictionaryValue> setting =
      GetWebsiteSetting(requesting_origin, embedding_origin, &info);
   std::unique_ptr<base::Value> objects;
   if (!setting->Remove(kObjectListKey, &objects))
     return results;

  std::unique_ptr<base::ListValue> object_list =
      base::ListValue::From(std::move(objects));
  if (!object_list)
    return results;

  for (auto& object : *object_list) {
     base::DictionaryValue* object_dict;
     if (object.GetAsDictionary(&object_dict) && IsValidObject(*object_dict)) {
       results.push_back(std::make_unique<Object>(
          requesting_origin, embedding_origin, object_dict, info.source,
           host_content_settings_map_->is_incognito()));
     }
   }
  return results;
}
