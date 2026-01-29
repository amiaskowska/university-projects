import <iostream>;

import colony;
import parser;

int main() {
    Colony colony;
    Parser parser(colony);
    parser.parse(std::cin);
    return 0;
}