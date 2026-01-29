export module parser;

import <regex>;
import <istream>;
import <string>;
import <stdexcept>;

import colony;
import text;

export class Parser {
private:
    Colony& colony;
    static const inline std::regex re_text   { R"(^TEXT (0|[1-9]\d*) ([!-~]+)$)" };   
    static const inline std::regex re_moth   { R"(^MOTH (0|[1-9]\d*) (0|[1-9]\d*) ([*A1!]) ([1-9]\d*) ([1-9][0-9]?)$)" };
    static const inline std::regex re_feed   { R"(^FEED (0|[1-9]\d*) ([1-9]\d*)$)" };
    static const inline std::regex re_printm { R"(^PRINTM (0|[1-9]\d*)$)" };
    static const inline std::regex re_printt { R"(^PRINTT (0|[1-9]\d*)$)" };
    static const inline std::regex re_delete { R"(^DELETE (0|[1-9]\d*)$)" };

public:
    explicit Parser(Colony& m) : colony(m) {}
    void parse(std::istream& is) {
        std::string line;
        int line_num = 0;

        while (std::getline(is, line)) {
            line_num++;
            if (!tryParseLine(line)) {
                printError(line_num);
            }
        }
    }

private: 
    // Returns false when given input of an incorrect pattern or
    // input containing incorrect data.
    bool tryParseLine(const std::string& line) {
        std::smatch matches;
        try {
            // TEXT
            if (std::regex_match(line, matches, re_text)) {
                return handleText(std::stoi(matches[1]), matches[2]);
            }

            // MOTH
            if (std::regex_match(line, matches, re_moth)) {
                return handleMoth(std::stoi(matches[1]), std::stoi(matches[2]),
                                *matches[3].first, std::stoi(matches[4]), 
                                std::stoi(matches[5]));
            }

            // FEED
            if (std::regex_match(line, matches, re_feed)) {
                return handleFeed(std::stoi(matches[1]), std::stoi(matches[2]));
            }

            // PTRINTM
            if (std::regex_match(line, matches, re_printm)) {
                return handlePrintm(std::stoi(matches[1]));
            }

            // PRINTT
            if (std::regex_match(line, matches, re_printt)) {
                return handlePrintt(std::stoi(matches[1]));
            }

            // DELETE
            if (std::regex_match(line, matches, re_delete)) {
                return handleDelete(std::stoi(matches[1]));
            }
        } catch (const std::out_of_range& e) {
            return false;
        }

        // No correct input pattern found.
        return false;
    }

    // TEXT: Adds new text of given id.
    bool handleText(int id, std::string content) {
        if (colony.textExists(id)) {
            return false;
        }
        
        colony.addText(id, content);
        return true;
    }

    // MOTH: Adds a new moth of given parameters to a text of a given id.
    bool handleMoth(int t_id, int n_pos, char type, int vitality, int p) {
        auto* text = colony.getText(t_id);
        if (!text) {
            return false;
        }
        if (n_pos >= text->getLength()) {
            return false;
        }

        text->addMoth(n_pos, vitality, p, type);
        return true;
    }

    // FEED: Runs given amount of feeding cycles on a text of a given id.
    bool handleFeed(int t_id, int n_cycles) {
        auto* text = colony.getText(t_id);
        if (!text) {
            return false;
        }

        while (n_cycles--) {
            text->run();
        }   
        return true;
    }

    // PTRINTM : Print moth information from text of a given id.
    bool handlePrintm(int t_id) {                
        auto* text = colony.getText(t_id);
        if (!text) {
            return false;
        }
        text->printm();
        return true;
    }

    // PRINTT: Print text content from given id.
    bool handlePrintt(int t_id) {
        auto* text = colony.getText(t_id);
        if (!text) {
            return false;
        }
        text->printt();
        return true;
    }

    // DELETE: Delete text of a given id.
    bool handleDelete(int t_id) {
        if (!colony.textExists(t_id)) {
            return false;
        }

        colony.deleteText(t_id);
        return true;
    }

    void printError(int line_num) {
        std::cerr << "ERROR " << line_num << "\n";
    }
};