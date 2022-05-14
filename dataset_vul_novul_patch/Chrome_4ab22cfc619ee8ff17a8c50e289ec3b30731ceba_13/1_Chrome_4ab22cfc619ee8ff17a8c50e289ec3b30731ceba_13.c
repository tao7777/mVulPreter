void CheckValueType(const Value::ValueType expected,
                    const Value* const actual) {
  DCHECK(actual != NULL) << "Expected value to be non-NULL";
  DCHECK(expected == actual->GetType())
      << "Expected " << print_valuetype(expected)
      << ", but was " << print_valuetype(actual->GetType());
}
