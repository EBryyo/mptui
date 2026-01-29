#include "fileExplorer.hh"
#include <algorithm>
#include <ftxui/dom/elements.hpp>

FileExplorer::FileExplorer(std::shared_ptr<FileNode> root) : root_(root) {
    BuildVisible(*root_);
    selected_index_ = 0;
}

bool FileExplorer::OnEvent(ftxui::Event event) { 
    bool flag = false;

    if (event == ftxui::Event::ArrowDown) {
        selected_index_ = std::min(selected_index_ + 1, (int)visible_nodes_.size() - 1);
        flag = true;
    } else if (event == ftxui::Event::ArrowUp) {
        selected_index_ = std::max(selected_index_ - 1, 0);
        flag = true;
    } else if (event == ftxui::Event::Return) {
        FileNode* node = visible_nodes_[selected_index_];
        if (node->is_dir) {
            node->expanded = !node->expanded;
            flag = true;
        } else {
            // Handle file selection here
        }
    }
    if (flag) {
        visible_nodes_.clear();
        BuildVisible(*root_);
    }
    return flag;
}

ftxui::Element FileExplorer::OnRender() { 
    //return ftxui::text("I am rendering") | ftxui::border | ftxui::flex;
    return ftxui::vbox(renderNode(*root_, visible_nodes_[selected_index_])) | ftxui::border | ftxui::flex; 
}

void FileExplorer::BuildVisible(FileNode& node) {
    visible_nodes_.push_back(&node);
    if (node.is_dir && node.expanded) {
        for (auto& child : node.children) {
            BuildVisible(*child);
        }
    }
}