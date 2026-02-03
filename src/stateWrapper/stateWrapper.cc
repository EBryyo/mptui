#include "action/action.hh"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <stateWrapper/stateWrapper.hh>

void StateWrapper::ProcessActions() {
  // do not treat empty queue
  if (actions_->empty()) {
    return;
  }

  // transfer ownership to the function scope
  std::unique_ptr<Action> action = std::move(actions_->front());
  actions_->pop();

  if (action->type == SELECT) {
    music_player_->setTrack(action->track.value());
  }

  // force re-draw
  screen_->PostEvent(ftxui::Event::Custom);
}

bool StateWrapper::OnEvent(ftxui::Event event) {
  if (event == ftxui::Event::Character('q')) {
    closure_();
  }
  return ftxui::ComponentBase::OnEvent(event);
}

ftxui::Element StateWrapper::OnRender() {
  return split->Render() | ftxui::reflect(box_);
}
