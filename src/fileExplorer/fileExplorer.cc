#include "fileExplorer.hh"
#include <algorithm>
#include <ftxui/dom/elements.hpp>

FileExplorer::FileExplorer(std::shared_ptr<FileNode> root) : root_(root) {
    BuildVisible(*root_);
    if (visible_nodes_.size() > 1) {
        hovered_index_ = 1;
    }
}

bool FileExplorer::OnEvent(ftxui::Event event) { 
    bool flag = false;

    if (event == ftxui::Event::ArrowDown) {
        hovered_index_ = std::min(hovered_index_ + 1, (int)visible_nodes_.size() - 1);
        flag = true;
    } else if (event == ftxui::Event::ArrowUp) {
        hovered_index_ = std::max(hovered_index_ - 1, 1);
        flag = true;
    } else if (event == ftxui::Event::Return) {
        FileNode* node = visible_nodes_[hovered_index_];
        if (node->is_dir) {
            node->expanded = !node->expanded;
        } else {
            // Handle file selection here
            selected_ = node;
        }
        flag = true;
    } else if (event.is_mouse()) {
        // Handle mouse events if necessary
        auto& m = event.mouse();

        int row = m.y;
        if (row > 0 && row < (int)visible_nodes_.size()) {
            hovered_index_ = row;
            FileNode* node = visible_nodes_[hovered_index_];
            if (m.button == ftxui::Mouse::Left && m.motion == ftxui::Mouse::Pressed) {
                if (node->is_dir) {
                    node->expanded = !node->expanded;
                } else {
                    // Handle file selection here
                    selected_ = node;
                }
                flag = true;
            }
        } else {
            hovered_index_ = -1; // No selection
        }

    } else if (event == ftxui::Event::Escape) {
        // Handle exit or other action
        hovered_index_ = -1;
        flag = true;
    }
    if (flag) {
        visible_nodes_.clear();
        BuildVisible(*root_);
    }
    return flag;
}

ftxui::Element FileExplorer::OnRender() { 
    //return ftxui::text("I am rendering") | ftxui::border | ftxui::flex;
    return ftxui::vbox(renderNode(*root_, selected_, visible_nodes_[hovered_index_])) | ftxui::flex; 
}

void FileExplorer::BuildVisible(FileNode& node) {
    visible_nodes_.push_back(&node);
    if (node.is_dir && node.expanded) {
        for (auto& child : node.children) {
            BuildVisible(*child);
        }
    }
}