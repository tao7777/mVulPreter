 DictionaryValue* BaseNode::GetDetailsAsValue() const {
   DictionaryValue* node_info = GetSummaryAsValue();
  node_info->SetString(
      "modificationTime",
      browser_sync::GetTimeDebugString(GetModificationTime()));
   node_info->SetString("parentId", base::Int64ToString(GetParentId()));
  node_info->SetString("externalId",
                       base::Int64ToString(GetExternalId()));
  node_info->SetString("predecessorId",
                       base::Int64ToString(GetPredecessorId()));
  node_info->SetString("successorId",
                       base::Int64ToString(GetSuccessorId()));
  node_info->SetString("firstChildId",
                       base::Int64ToString(GetFirstChildId()));
  node_info->Set("entry", GetEntry()->ToValue());
  return node_info;
}
