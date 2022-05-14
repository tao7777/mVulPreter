 float AudioParam::finalValue()
 {
    float value;
     calculateFinalValues(&value, 1, false);
     return value;
 }
