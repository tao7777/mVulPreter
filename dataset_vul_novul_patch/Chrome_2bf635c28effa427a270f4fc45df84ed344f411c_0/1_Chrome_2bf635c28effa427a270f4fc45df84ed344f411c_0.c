 Node::InsertionNotificationRequest HTMLStyleElement::InsertedInto(
     ContainerNode* insertion_point) {
   HTMLElement::InsertedInto(insertion_point);
  return kInsertionShouldCallDidNotifySubtreeInsertions;
 }
