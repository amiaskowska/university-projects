export module text;

export import <string>;
export import <vector>;
export import <memory>;
export import <iostream>;

export import moth;

export class Text {
private: 
    std::string content;
    std::vector<std::unique_ptr<Moth>> moths;

public: 
    explicit Text (std::string s) : content(std::move(s)) {}
    int getLength() const { return content.length(); }
    
    void addMoth(int pos, int v, int p, char type) {
        std::unique_ptr<Moth> new_moth;

        switch (type) {
            case '*':
                new_moth = std::make_unique<StandardMoth>(pos, v, p);
                break;
            case 'A': 
                new_moth = std::make_unique<LetterMoth>(pos, v, p);
                break;
            case '1': 
                new_moth = std::make_unique<DigitMoth>(pos, v, p);
                break;
            case '!': 
                new_moth = std::make_unique<PickyMoth>(pos, v, p);
                break;
            default: 
                return;
        }

        moths.push_back(std::move(new_moth));
    }

    void run() {
        for (auto& moth : moths) {
            moth->act(content);
        }
    }

    void printm() const {
        for (auto& moth: moths) {
            std::cout << *moth;
        }
    }

    void printt() const {
        std::cout << content << "\n";
    }
};
