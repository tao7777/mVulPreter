 DictionaryValue* BaseNode::GetDetailsAsValue() const {
   DictionaryValue* node_info = GetSummaryAsValue();
  // TODO(akalin): Return time in a better format.
  node_info->SetString("modificationTime",
                       base::Int64ToString(GetModificationTime()));
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
