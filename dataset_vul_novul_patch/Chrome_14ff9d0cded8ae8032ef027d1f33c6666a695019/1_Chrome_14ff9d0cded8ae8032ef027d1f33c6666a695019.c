AutomationInternalCustomBindings::AutomationInternalCustomBindings(
    ScriptContext* context)
    : ObjectBackedNativeHandler(context),
      is_active_profile_(true),
      tree_change_observer_overall_filter_(
          api::automation::TREE_CHANGE_OBSERVER_FILTER_NOTREECHANGES) {
  #define ROUTE_FUNCTION(FN) \
  RouteFunction(#FN, \
                 base::Bind(&AutomationInternalCustomBindings::FN, \
                base::Unretained(this)))
   ROUTE_FUNCTION(IsInteractPermitted);
   ROUTE_FUNCTION(GetSchemaAdditions);
   ROUTE_FUNCTION(GetRoutingID);
  ROUTE_FUNCTION(StartCachingAccessibilityTrees);
  ROUTE_FUNCTION(DestroyAccessibilityTree);
  ROUTE_FUNCTION(AddTreeChangeObserver);
  ROUTE_FUNCTION(RemoveTreeChangeObserver);
  ROUTE_FUNCTION(GetChildIDAtIndex);
  ROUTE_FUNCTION(GetFocus);
  ROUTE_FUNCTION(GetState);
  #undef ROUTE_FUNCTION


  RouteTreeIDFunction(
      "GetRootID", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                      TreeCache* cache) {
        result.Set(v8::Integer::New(isolate, cache->tree.root()->id()));
      });
  RouteTreeIDFunction(
      "GetDocURL", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                      TreeCache* cache) {
        result.Set(
            v8::String::NewFromUtf8(isolate, cache->tree.data().url.c_str()));
      });
  RouteTreeIDFunction(
      "GetDocTitle", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                        TreeCache* cache) {
        result.Set(
            v8::String::NewFromUtf8(isolate, cache->tree.data().title.c_str()));
      });
  RouteTreeIDFunction(
      "GetDocLoaded", [](v8::Isolate* isolate,
                         v8::ReturnValue<v8::Value> result, TreeCache* cache) {
        result.Set(v8::Boolean::New(isolate, cache->tree.data().loaded));
      });
  RouteTreeIDFunction("GetDocLoadingProgress",
                      [](v8::Isolate* isolate,
                         v8::ReturnValue<v8::Value> result, TreeCache* cache) {
                        result.Set(v8::Number::New(
                            isolate, cache->tree.data().loading_progress));
                      });
  RouteTreeIDFunction("GetAnchorObjectID",
                      [](v8::Isolate* isolate,
                         v8::ReturnValue<v8::Value> result, TreeCache* cache) {
                        result.Set(v8::Number::New(
                            isolate, cache->tree.data().sel_anchor_object_id));
                      });
  RouteTreeIDFunction("GetAnchorOffset", [](v8::Isolate* isolate,
                                            v8::ReturnValue<v8::Value> result,
                                            TreeCache* cache) {
    result.Set(v8::Number::New(isolate, cache->tree.data().sel_anchor_offset));
  });
  RouteTreeIDFunction("GetFocusObjectID",
                      [](v8::Isolate* isolate,
                         v8::ReturnValue<v8::Value> result, TreeCache* cache) {
                        result.Set(v8::Number::New(
                            isolate, cache->tree.data().sel_focus_object_id));
                      });
  RouteTreeIDFunction("GetFocusOffset", [](v8::Isolate* isolate,
                                           v8::ReturnValue<v8::Value> result,
                                           TreeCache* cache) {
    result.Set(v8::Number::New(isolate, cache->tree.data().sel_focus_offset));
  });


  RouteNodeIDFunction(
      "GetParentID", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                        TreeCache* cache, ui::AXNode* node) {
        if (node->parent())
          result.Set(v8::Integer::New(isolate, node->parent()->id()));
      });
  RouteNodeIDFunction("GetChildCount", [](v8::Isolate* isolate,
                                          v8::ReturnValue<v8::Value> result,
                                          TreeCache* cache, ui::AXNode* node) {
    result.Set(v8::Integer::New(isolate, node->child_count()));
  });
  RouteNodeIDFunction(
      "GetIndexInParent",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         TreeCache* cache, ui::AXNode* node) {
        result.Set(v8::Integer::New(isolate, node->index_in_parent()));
      });
  RouteNodeIDFunction(
      "GetRole", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                    TreeCache* cache, ui::AXNode* node) {
        std::string role_name = ui::ToString(node->data().role);
        result.Set(v8::String::NewFromUtf8(isolate, role_name.c_str()));
      });
  RouteNodeIDFunction(
      "GetLocation", [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
                        TreeCache* cache, ui::AXNode* node) {
        gfx::Rect location = ComputeGlobalNodeBounds(cache, node);
        location.Offset(cache->location_offset);
        result.Set(RectToV8Object(isolate, location));
      });


  RouteNodeIDPlusRangeFunction(
      "GetBoundsForRange",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         TreeCache* cache, ui::AXNode* node, int start, int end) {
        gfx::Rect location = ComputeGlobalNodeBounds(cache, node);
        location.Offset(cache->location_offset);
        if (node->data().role == ui::AX_ROLE_INLINE_TEXT_BOX) {
          std::string name = node->data().GetStringAttribute(ui::AX_ATTR_NAME);
          std::vector<int> character_offsets =
              node->data().GetIntListAttribute(ui::AX_ATTR_CHARACTER_OFFSETS);
          int len =
              static_cast<int>(std::min(name.size(), character_offsets.size()));
          if (start >= 0 && start <= end && end <= len) {
            int start_offset = start > 0 ? character_offsets[start - 1] : 0;
            int end_offset = end > 0 ? character_offsets[end - 1] : 0;

            switch (node->data().GetIntAttribute(ui::AX_ATTR_TEXT_DIRECTION)) {
              case ui::AX_TEXT_DIRECTION_LTR:
              default:
                location.set_x(location.x() + start_offset);
                location.set_width(end_offset - start_offset);
                break;
              case ui::AX_TEXT_DIRECTION_RTL:
                location.set_x(location.x() + location.width() - end_offset);
                location.set_width(end_offset - start_offset);
                break;
              case ui::AX_TEXT_DIRECTION_TTB:
                location.set_y(location.y() + start_offset);
                location.set_height(end_offset - start_offset);
                break;
              case ui::AX_TEXT_DIRECTION_BTT:
                location.set_y(location.y() + location.height() - end_offset);
                location.set_height(end_offset - start_offset);
                break;
            }
          }
        }
        result.Set(RectToV8Object(isolate, location));
      });


  RouteNodeIDPlusAttributeFunction(
      "GetStringAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        ui::AXStringAttribute attribute =
            ui::ParseAXStringAttribute(attribute_name);
        std::string attr_value;
        if (!node->data().GetStringAttribute(attribute, &attr_value))
          return;

        result.Set(v8::String::NewFromUtf8(isolate, attr_value.c_str()));
      });
  RouteNodeIDPlusAttributeFunction(
      "GetBoolAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        ui::AXBoolAttribute attribute =
            ui::ParseAXBoolAttribute(attribute_name);
        bool attr_value;
        if (!node->data().GetBoolAttribute(attribute, &attr_value))
          return;

        result.Set(v8::Boolean::New(isolate, attr_value));
      });
  RouteNodeIDPlusAttributeFunction(
      "GetIntAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        ui::AXIntAttribute attribute = ui::ParseAXIntAttribute(attribute_name);
        int attr_value;
        if (!node->data().GetIntAttribute(attribute, &attr_value))
          return;

        result.Set(v8::Integer::New(isolate, attr_value));
      });
  RouteNodeIDPlusAttributeFunction(
      "GetFloatAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        ui::AXFloatAttribute attribute =
            ui::ParseAXFloatAttribute(attribute_name);
        float attr_value;

        if (!node->data().GetFloatAttribute(attribute, &attr_value))
          return;

        result.Set(v8::Number::New(isolate, attr_value));
      });
  RouteNodeIDPlusAttributeFunction(
      "GetIntListAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        ui::AXIntListAttribute attribute =
            ui::ParseAXIntListAttribute(attribute_name);
        if (!node->data().HasIntListAttribute(attribute))
          return;
        const std::vector<int32_t>& attr_value =
            node->data().GetIntListAttribute(attribute);

        v8::Local<v8::Array> array_result(
            v8::Array::New(isolate, attr_value.size()));
        for (size_t i = 0; i < attr_value.size(); ++i)
          array_result->Set(static_cast<uint32_t>(i),
                            v8::Integer::New(isolate, attr_value[i]));
        result.Set(array_result);
      });
  RouteNodeIDPlusAttributeFunction(
      "GetHtmlAttribute",
      [](v8::Isolate* isolate, v8::ReturnValue<v8::Value> result,
         ui::AXNode* node, const std::string& attribute_name) {
        std::string attr_value;
        if (!node->data().GetHtmlAttribute(attribute_name.c_str(), &attr_value))
          return;

        result.Set(v8::String::NewFromUtf8(isolate, attr_value.c_str()));
      });
}
