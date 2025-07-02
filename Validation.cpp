#include "Validation.h"
#include <iostream>
#include <limits>

using namespace std;

int Validation::getValidatedInt(const string &prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            if (value >= min && value <= max)
            {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            else
            {
                cout << "Please enter a number between " << min << " and " << max << ".\n";
            }
        }
        else
        {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

string Validation::getValidatedString(const string &prompt, int minLength, int maxLength)
{
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        if (input.length() >= minLength && input.length() <= maxLength)
        {
            return input;
        }
        else
        {
            cout << "Input must be between " << minLength << " and " << maxLength << " characters.\n";
        }
    }
}

string Validation::getNonEmptyString(const string &prompt)
{
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        if (!input.empty())
        {
            return input;
        }
        else
        {
            cout << "Input cannot be empty. Try again.\n";
        }
    }
}
