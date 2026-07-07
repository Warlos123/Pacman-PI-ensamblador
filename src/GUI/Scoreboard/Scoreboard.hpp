#pragma once
#include <string>
#include <vector>
#include <cstddef>

class Scoreboard {
public:
    struct Entry {
        std::string name;
        int score;
    };

    explicit Scoreboard(const std::string& filename = "scores.txt");

    void load();
    void add(const std::string& name, int score);
    int  highScore() const;
    std::vector<Entry> top(std::size_t n) const;

private:
    void sortDesc();
    void save();

    std::string filename_;
    std::vector<Entry> entries_;
};