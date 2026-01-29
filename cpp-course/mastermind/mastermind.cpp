#include <charconv>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define MAX_K 256
#define MIN_K 2
#define MAX_N 10
#define MIN_N 2
#define MAX_N_K 16777216

void returnError()
{
    std::cerr << "ERROR" << std::endl;
    std::exit(1);
}

// Funkcja odczytuje pojedynczą linijkę ze standardowego wejścia, sprawdzając
// poprawność formatu oraz danych.
std::vector<uint32_t> getInput()
{
    std::string line;
    if (!std::getline(std::cin, line))
        return {};

    if (line.empty())
        returnError();

    if (line.back() == '\r')
        line.pop_back();

    if (std::isspace(static_cast<unsigned char>(line.back())))
        returnError();

    if (!std::isdigit(static_cast<unsigned char>(line[0])))
        returnError();

    std::vector<uint32_t> in;
    std::size_t pos = 0;

    while (pos < line.size()) {
        if (line[pos] == ' ') {
            if (pos + 1 < line.size() && line[pos + 1] == ' ')
                return {};
            ++pos;
            continue;
        }

        std::size_t start = pos;
        while (pos < line.size() &&
               std::isdigit(static_cast<unsigned char>(line[pos])))
            ++pos;

        std::string_view str(line.data() + start, pos - start);

        uint32_t v = 0;
        auto [ptr, ec] =
            std::from_chars(str.data(), str.data() + str.size(), v);

        if (ec != std::errc() || ptr != str.data() + str.size())
            returnError();

        in.push_back(v);
    }

    return in;
}

// Funkcja wczytuje wartości B i W, sprawdzając, czy są w odpowiednim zakresie.
std::pair<uint32_t, uint32_t> getBW(uint32_t n)
{
    std::vector<uint32_t> input = getInput();
    if (std::cin.eof() && !input.size())
        exit(0);

    if (input.size() != 2)
        returnError();

    uint32_t b = input[0];
    uint32_t w = input[1];

    if (b > n)
        returnError();
    if (w > n)
        returnError();
    if (b + w > n)
        returnError();

    if (std::cin.eof())
        exit(0);

    std::pair<uint32_t, uint32_t> ret = {b, w};
    return ret;
}

// Funkcja odczytuje pojedynczą wartość argumentu.
int getData(char* argv[], int i)
{
    std::string_view arg{argv[i]};
    uint32_t value{};
    auto [ptr, ec] =
        std::from_chars(arg.data(), arg.data() + arg.size(), value);

    if (ec != std::errc())
        returnError();
    return value;
}

// Funkcja sprawdza, czy argumenty są odpowiednim w zakresie.
bool check_range(uint32_t n, uint32_t k)
{
    if (n > MAX_N || n < MIN_N)
        return 1;
    if (k > MAX_K || k < MIN_K)
        return 1;
    uint64_t pow = static_cast<uint64_t>(k);
    for (uint32_t i = 2; i <= n; ++i) {
        pow *= static_cast<uint64_t>(k);
        if (pow > MAX_N_K)
            return 1;
    }
    return 0;
}

void codemaker(uint32_t n, uint32_t k, std::vector<uint32_t>& secret)
{
    std::vector<uint32_t> colorCount(k);
    for (size_t i = 0; i < secret.size(); ++i) {
        colorCount[secret[i]]++;
    }

    uint32_t w = 0, b = 0;
    while (b != n || w) {
        std::vector<uint32_t> guess = getInput();

        if (std::cin.eof() && !guess.size())
            return;

        if (guess.size() != secret.size())
            return returnError();

        w = 0, b = 0;
        std::vector<uint32_t> colorCountCopy;
        std::copy(colorCount.begin(),
            colorCount.end(),
            std::back_inserter(colorCountCopy));

        for (size_t i = 0; i < secret.size(); ++i) {
            if (guess[i] >= k) {
                returnError();
            }

            if (secret[i] == guess[i]) {
                b++;
                colorCountCopy[secret[i]]--;
            }
        }

        for (size_t i = 0; i < secret.size(); ++i) {
            if (secret[i] != guess[i] && colorCountCopy[guess[i]]) {
                ++w;
                colorCountCopy[guess[i]]--;
            }
        }

        std::cout << b << " " << w << std::endl;

        if (std::cin.eof())
            return;
    }
}

void codebreaker(uint32_t n, uint32_t k)
{
    // Wektor zawierający pary postaci {kolor, liczność} kolorów z sekretu.
    std::vector<std::pair<uint32_t, uint32_t>> colors;

    // Zdobywa informacje o liczności poszczególnych kolorów w sekrecie.
    uint32_t sum = 0; // Suma liczności wszystkich kolorów.
    for (uint32_t i = 0; i < k; ++i) {
        for (uint32_t j = 0; j < n - 1; ++j)
            std::cout << i << " ";
        std::cout << i << std::endl;

        std::pair<uint32_t, uint32_t> in = getBW(n);
        uint32_t b = in.first;
        uint32_t w = in.second;
        if (b == n && !w)
            return;

        if (w)
            returnError();

        if (b) {
            colors.push_back({i, b});
            sum += b;
            if (sum > n)
                returnError();
        }
    }
    if (sum != n)
        returnError();

    std::vector<uint32_t> secret(n);

    // Układa kolory w odpowiedniej kolejności, sprawdzając kolejne miejsca.
    uint32_t filler = colors[0].first;
    // Wybór zgadywanego miejsca.
    for (size_t i = 0; i < secret.size(); ++i) {
        bool color_found = false;
        // Wybór indeksu zgadywanego koloru - pomijając kolor filler.
        for (size_t j = 1; j < colors.size(); ++j) {
            std::vector<uint32_t> guess(n);

            // Wszystkie miejsca poza zgadywanym są koloru filler.
            for (size_t l = 0; l < guess.size(); ++l)
                guess[l] = filler;
            guess[i] = colors[j].first;

            for (size_t l = 0; l < guess.size() - 1; ++l)
                std::cout << guess[l] << " ";
            std::cout << guess.back() << std::endl;

            std::pair<uint32_t, uint32_t> in = getBW(n);
            uint32_t b = in.first;
            uint32_t w = in.second;

            if (b == n && !w)
                return;
            else if (b == (colors[0].second + 1)) {
                if (!w) {
                    color_found = true;
                    secret[i] = colors[j].first;
                    break;
                }
                else
                    returnError();
            }
            else if (b == (colors[0].second - 1)) {
                if (w == 2 && j == 1) {
                    color_found = true;
                    secret[i] = colors[0].first;
                    break;
                }
                else
                    returnError();
            }
            else if (b != colors[0].second || w != 1)
                returnError();
        }
        // Odpowiedzi użytkownika nie pozwoliły określić koloru.
        if (!color_found)
            returnError();
    }

    for (uint32_t i = 0; i < n - 1; ++i)
        std::cout << secret[i] << " ";
    std::cout << secret.back() << std::endl;

    std::pair<uint32_t, uint32_t> in = getBW(n);
    uint32_t b = in.first;
    uint32_t w = in.second;
    if (b != n || w)
        returnError();
}

int main(int argc, char* argv[])
{
    if (argc <= 2) {
        returnError();
    }
    else if (argc == 3) {
        uint32_t n = getData(argv, 2);
        uint32_t k = getData(argv, 1);

        if (check_range(n, k))
            returnError();

        codebreaker(n, k);
    }
    else {
        std::vector<uint32_t> secret;
        for (int i = 2; i < argc; ++i)
            secret.push_back(getData(argv, i));

        uint32_t k = getData(argv, 1);
        uint32_t n = (uint32_t)secret.size();

        if (check_range(n, k))
            returnError();

        for (uint32_t& x : secret) {
            if (x >= k)
                returnError();
        }

        codemaker(n, k, secret);
    }
    return 0;
}