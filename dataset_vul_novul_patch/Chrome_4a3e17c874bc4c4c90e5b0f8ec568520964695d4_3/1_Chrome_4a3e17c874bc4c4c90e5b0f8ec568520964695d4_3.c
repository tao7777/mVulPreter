HeapVector<NotificationAction> Notification::actions() const
{
    HeapVector<NotificationAction> actions;
    actions.grow(m_data.actions.size());

     for (size_t i = 0; i < m_data.actions.size(); ++i) {
         actions[i].setAction(m_data.actions[i].action);
         actions[i].setTitle(m_data.actions[i].title);
     }
 
     return actions;
}
