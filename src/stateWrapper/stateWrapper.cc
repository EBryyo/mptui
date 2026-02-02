#include <ftxui/component/component.hpp>
#include <stateWrapper/stateWrapper.hh>

void StateWrapper::ProcessActions() {}

ftxui::Element StateWrapper::OnRender() {
  ftxui::Component explorer =
      ftxui::Renderer([this] { return file_explorer_->Render(); });
  ftxui::Component tracklist =
      ftxui::Renderer([this] { return track_queue_->Render(); });

  ftxui::Component split =
      ftxui::ResizableSplitLeft(explorer, tracklist, &explorer_size_);

  return split->Render();
}
