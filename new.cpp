#include <iostream>
using namespace std;

int main() {
    cout << "\033[1;31mThis is RED text!\033[0m\n";
    cout << "\033[1;32mThis is GREEN text!\033[0m\n";
    cout << "\033[1;33mThis is YELLOW text!\033[0m\n";
    cout << "\033[1;34mThis is BLUE text!\033[0m\n";
    cout << "\033[1;35mThis is MAGENTA text!\033[0m\n";
    cout << "\033[1;36mThis is CYAN text!\033[0m\n";
    cout << "\033[1;37mThis is WHITE text!\033[0m\n";
    cout << "\033[1;30mThis is BLACK text!\033[0m\n";
    cout << "\033[1;0mThis is DEFAULT text!\033[0m\n";

    return 0;
}
