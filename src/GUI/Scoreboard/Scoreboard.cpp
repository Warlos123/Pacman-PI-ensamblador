#include "Scoreboard.hpp"
#include <fstream>
#include <algorithm>

Scoreboard::Scoreboard(const std::string& filename)
    : filename_(filename) {
    load();
}

void Scoreboard::load() {
    entries_.clear();
    std::ifstream in(filename_);
    if (!in.is_open())
        return; //se crea al guardar

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::size_t sep = line.find(';');  // score;nombre
        if (sep == std::string::npos) continue;
        try {
            int score = std::stoi(line.substr(0, sep));
            entries_.push_back({ line.substr(sep + 1), score });
        } catch (...) {
        }
    }
    sortDesc();
}

void Scoreboard::add(const std::string& name, int score) {
    std::string clean;
    for (char c : name)               
        if (c != ';' && c != '\n' && c != '\r')
            clean.push_back(c);
    if (clean.empty()) clean = "ANON";

    entries_.push_back({ clean, score });
    sortDesc();
    save();
}

int Scoreboard::highScore() const {
    return entries_.empty() ? 0 : entries_.front().score;
}

std::vector<Scoreboard::Entry> Scoreboard::top(std::size_t n) const {
    std::vector<Entry> result;
    for (std::size_t i = 0; i < entries_.size() && i < n; ++i)
        result.push_back(entries_[i]);
    return result;
}

void Scoreboard::sortDesc() {
    std::sort(entries_.begin(), entries_.end(),
              [](const Entry& a, const Entry& b){ return a.score > b.score; });
}

void Scoreboard::save() {
    std::ofstream out(filename_, std::ios::trunc);
    if (!out.is_open()) return;
    for (const auto& e : entries_)
        out << e.score << ';' << e.name << '\n';
}