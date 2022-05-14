bool SessionService::CreateTabsAndWindows(
    const std::vector<SessionCommand*>& data,
    std::map<int, SessionTab*>* tabs,
    std::map<int, SessionWindow*>* windows) {
 
   for (std::vector<SessionCommand*>::const_iterator i = data.begin();
        i != data.end(); ++i) {
     const SessionCommand::id_type kCommandSetWindowBounds2 = 10;
    const SessionCommand* command = *i;

    switch (command->id()) {
      case kCommandSetTabWindow: {
        SessionID::id_type payload[2];
        if (!command->GetPayload(payload, sizeof(payload)))
          return true;
        GetTab(payload[1], tabs)->window_id.set_id(payload[0]);
        break;
      }

      case kCommandSetWindowBounds2: {
        WindowBoundsPayload2 payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetWindow(payload.window_id, windows)->bounds.SetRect(payload.x,
                                                              payload.y,
                                                              payload.w,
                                                              payload.h);
        GetWindow(payload.window_id, windows)->show_state =
            payload.is_maximized ?
                ui::SHOW_STATE_MAXIMIZED : ui::SHOW_STATE_NORMAL;
        break;
      }

      case kCommandSetWindowBounds3: {
        WindowBoundsPayload3 payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetWindow(payload.window_id, windows)->bounds.SetRect(payload.x,
                                                              payload.y,
                                                              payload.w,
                                                              payload.h);
        ui::WindowShowState show_state = ui::SHOW_STATE_NORMAL;
        if (payload.show_state > ui::SHOW_STATE_DEFAULT &&
            payload.show_state < ui::SHOW_STATE_END &&
            payload.show_state != ui::SHOW_STATE_INACTIVE) {
          show_state = static_cast<ui::WindowShowState>(payload.show_state);
        } else {
          NOTREACHED();
        }
        GetWindow(payload.window_id, windows)->show_state = show_state;
        break;
      }

      case kCommandSetTabIndexInWindow: {
        TabIndexInWindowPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetTab(payload.id, tabs)->tab_visual_index = payload.index;
        break;
      }

      case kCommandTabClosed:
      case kCommandWindowClosed: {
        ClosedPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        if (command->id() == kCommandTabClosed) {
          delete GetTab(payload.id, tabs);
          tabs->erase(payload.id);
        } else {
          delete GetWindow(payload.id, windows);
          windows->erase(payload.id);
        }
        break;
      }

      case kCommandTabNavigationPathPrunedFromBack: {
        TabNavigationPathPrunedFromBackPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        SessionTab* tab = GetTab(payload.id, tabs);
        tab->navigations.erase(
            FindClosestNavigationWithIndex(&(tab->navigations), payload.index),
            tab->navigations.end());
        break;
      }

      case kCommandTabNavigationPathPrunedFromFront: {
        TabNavigationPathPrunedFromFrontPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)) ||
            payload.index <= 0) {
          return true;
        }
        SessionTab* tab = GetTab(payload.id, tabs);

        tab->current_navigation_index =
            std::max(-1, tab->current_navigation_index - payload.index);

        for (std::vector<TabNavigation>::iterator i = tab->navigations.begin();
             i != tab->navigations.end();) {
          i->set_index(i->index() - payload.index);
          if (i->index() < 0)
            i = tab->navigations.erase(i);
          else
            ++i;
        }
        break;
      }

      case kCommandUpdateTabNavigation: {
        TabNavigation navigation;
        SessionID::id_type tab_id;
        if (!RestoreUpdateTabNavigationCommand(*command, &navigation, &tab_id))
          return true;

        SessionTab* tab = GetTab(tab_id, tabs);
        std::vector<TabNavigation>::iterator i =
            FindClosestNavigationWithIndex(&(tab->navigations),
                                           navigation.index());
        if (i != tab->navigations.end() && i->index() == navigation.index())
          *i = navigation;
        else
          tab->navigations.insert(i, navigation);
        break;
      }

      case kCommandSetSelectedNavigationIndex: {
        SelectedNavigationIndexPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetTab(payload.id, tabs)->current_navigation_index = payload.index;
        break;
      }

      case kCommandSetSelectedTabInIndex: {
        SelectedTabInIndexPayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetWindow(payload.id, windows)->selected_tab_index = payload.index;
        break;
      }

      case kCommandSetWindowType: {
        WindowTypePayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetWindow(payload.id, windows)->is_constrained = false;
        GetWindow(payload.id, windows)->type =
            BrowserTypeForWindowType(
                static_cast<WindowType>(payload.index));
        break;
      }

      case kCommandSetPinnedState: {
        PinnedStatePayload payload;
        if (!command->GetPayload(&payload, sizeof(payload)))
          return true;
        GetTab(payload.tab_id, tabs)->pinned = payload.pinned_state;
        break;
      }

      case kCommandSetExtensionAppID: {
        SessionID::id_type tab_id;
        std::string extension_app_id;
        if (!RestoreSetTabExtensionAppIDCommand(
                *command, &tab_id, &extension_app_id)) {
          return true;
        }

        GetTab(tab_id, tabs)->extension_app_id.swap(extension_app_id);
        break;
      }

      default:
         return true;
     }
   }
   return true;
 }
