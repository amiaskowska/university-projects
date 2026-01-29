export module moth;

export import <string>;
export import <vector>; 
export import <cctype>;
export import <ostream>;

export class Moth {
private:
    int position;
    int vitality;
    int P;
    static constexpr int COST = 10;

public:
    Moth(int startPos, int v, int p) noexcept
        : position(startPos), vitality(v), P(p) {}

    virtual ~Moth() = default;
    bool isActive() const noexcept { return vitality > 0; }
    int getPosition() const noexcept { return position; }
    int getVitality() const noexcept { return vitality; }
    int getP() const noexcept { return P; }

    void act(std::string& text) {
        if (!isActive() || text.empty())
            return;

        const int step = nextStep();
        const int cost = COST * step;

        if (vitality < cost) {
            vitality = 0;
            return;
        }

        vitality -= cost;
        const int len = static_cast<int>(text.size());
        position = (position + step) % len;
        const char& c = text[position];

        if (c == ' ') {
            vitality -= static_cast<int>(c);
            if (vitality < 0) {
                vitality = 0;
            }
        } else if (canEat(c)) {
            vitality += static_cast<int>(c);
            text[position] = ' ';
        }
    }

    virtual char typeSymbol() const noexcept = 0;
    friend std::ostream& operator<<(std::ostream& os, const Moth& moth);
    
protected:
    virtual int nextStep() noexcept { return P; }
    virtual bool canEat(char c) const noexcept = 0;
};

export std::ostream& operator<<(std::ostream& os, const Moth& moth) {
    os << moth.typeSymbol() << " " << moth.getP() << " " 
       << moth.getPosition() << " " << moth.getVitality() << "\n"; 
    return os;
}


export class StandardMoth : public Moth {
public:
    using Moth::Moth;
    char typeSymbol() const noexcept override { return '*'; }

private:
    bool canEat(char c) const noexcept override { return c != ' '; }
};

export class LetterMoth : public Moth {
public:
    using Moth::Moth;
    char typeSymbol() const noexcept override { return 'A'; }

private:
    bool canEat(char c) const noexcept override {
        return std::isalpha(static_cast<unsigned char>(c));
    }
};

export class DigitMoth : public Moth {
public:
    using Moth::Moth;
    char typeSymbol() const noexcept override { return '1'; }

private:
    bool canEat(char c) const noexcept override {
        return std::isdigit(static_cast<unsigned char>(c));
    }
};
        

export class PickyMoth : public Moth {
private: 
    int currentStep = 1;

public:
    using Moth::Moth;
    char typeSymbol() const noexcept override { return '!'; }


private:
    int nextStep() noexcept override {
        const int ret = currentStep;
        if (currentStep == getP()) {
            currentStep = 1;
        }
        else {
            currentStep++;
        }
        
        return ret;
    }

    bool canEat(char c) const noexcept override {
        return !std::isalnum(static_cast<unsigned char>(c));
    }
};