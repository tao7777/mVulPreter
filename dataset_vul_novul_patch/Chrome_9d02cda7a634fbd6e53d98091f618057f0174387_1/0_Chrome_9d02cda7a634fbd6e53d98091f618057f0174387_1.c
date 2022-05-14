 PolicyStatusInfo::PolicyStatusInfo(
    const string16& name,
     PolicySourceType source_type,
     PolicyLevel level,
     Value* value,
     PolicyStatus status,
    const string16& error_message)
     : name(name),
       source_type(source_type),
       level(level),
      value(value),
      status(status),
      error_message(error_message) {
}
