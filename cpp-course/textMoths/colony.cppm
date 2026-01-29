export module colony;

export import <unordered_map>;
export import <string>;

export import text;

export class Colony {
private:
    std::unordered_map<int, Text> texts;

public:
    Colony() = default;
    bool textExists(int id) const {
        return texts.contains(id);
    }

    bool addText(int id, const std::string& newText) {
        auto [it, inserted] = texts.try_emplace(id, std::move(newText));
        return inserted;
    }

    Text* getText(int id) {
        auto it = texts.find(id);
        if (it == texts.end()) return nullptr;
        return &it->second;
    }

    bool deleteText(int id) {
        return texts.erase(id) > 0;
    }
};
