// Online Voting System

#include <iostream>
// #include<ctype>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
using namespace std;

// Forward declarations
class Voter;
class Administrator;
class Election;
class LocalElection;
class NationalElection;
class Candidate;

// Constants for array sizes
const int MAX_CANDIDATES = 50;
const int MAX_USERS = 100;
const int MAX_ELECTIONS = 20;

void DisplayMainMenu();

// Base User class
class User
{
protected:
    string username;
    string password;
    string role; // "voter" or "admin"

public:
    User() {}
    User(string uname, string pwd, string r) : username(uname), password(pwd), role(r) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    bool authenticate(string uname, string pwd)
    {
        return (username == uname && password == pwd);
    }

    virtual void displayMenu() = 0;

    static void saveToFile(const User &user)
    {
        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open())
        {
            outFile << user.username << " " << user.password << " " << user.role << endl;
            outFile.close();
        }
        else
        {
            cerr << "Unable to open users file for writing." << endl;
        }
    }

    static User *authenticateAdmin(const string &username, const string &password);
    static User *authenticatevoter(const string &username, const string &password);
};

// Candidate class
class Candidate
{
private:
    string name;
    string party;
    int votes;

public:
    Candidate() : name(""), party(""), votes(0) {}
    Candidate(string n, string p) : name(n), party(p), votes(0) {}

    string getName() const { return name; }
    string getParty() const { return party; }
    int getVotes() const { return votes; }

    void incrementVotes() { votes++; }

    void saveToFile(string electionName)
    {
        ofstream outFile("candidates.txt", ios::app);
        if (outFile.is_open())
        {
            outFile << electionName << " " << name << " " << party << " " << votes << endl;
            outFile.close();
        }
    }

    static Candidate *loadCandidates(string electionName, int &count);
};

// Base Election class (declaration only)
class Election
{
protected:
    string name;
    string type;
    string startDate;
    string endDate;
    Candidate candidates[MAX_CANDIDATES];
    int candidateCount;

public:
    Election(string n, string t, string s, string e);
    virtual ~Election() {}

    string getName() const { return name; }
    string getType() const { return type; }
    string getStartDate() const { return startDate; }
    string getEndDate() const { return endDate; }
    int getCandidateCount() const { return candidateCount; }

    virtual void displayDetails() = 0;
    virtual void conductElection() = 0;
    virtual void displayResults() = 0;

    void addCandidate(string name, string party);
    void saveToFile();
    static Election *loadElection(string name);
};

// Voter class
class Voter : public User
{
private:
    bool hasVoted;

public:
    Voter(string uname, string pwd) : User(uname, pwd, "voter"), hasVoted(false) {}

    void displayMenu() override;
    void viewElections();
    void castVote();
    void checkVoteStatus();
};

// Administrator class
class Administrator : public User
{
public:
    Administrator(string uname, string pwd) : User(uname, pwd, "admin") {}

    void displayMenu() override;
    void createElection();
    void addCandidate();
    void viewResults();
};

// LocalElection class
class LocalElection : public Election
{
private:
    string district;

public:
    LocalElection(string n, string s, string e, string d="Default District")
        : Election(n, "local", s, e), district(d) {}

    void displayDetails()  override;
    void conductElection() override;
    void displayResults() override;
};

// NationalElection class
class NationalElection : public Election
{
private:
    string country;

public:
    NationalElection(string n, string s, string e, string c = "Default Country")
        : Election(n, "national", s, e), country(c) {}

    void displayDetails() override;
    void conductElection() override;
    void displayResults() override;
};

// Now implement all the methods that were declared earlier:

// User methods
User *User::authenticateAdmin(const string &username, const string &password)
{
    ifstream inFile("users.txt");
    if (inFile.is_open())
    {
        string uname, pwd, role;
        while (inFile >> uname >> pwd >> role)
        {
            if (uname == username && pwd == password)
            {
                if (role == "admin")
                {
                    return new Administrator(uname, pwd);
                }
            }
        }
        inFile.close();
    }
    return nullptr;
}

User *User::authenticatevoter(const string &username, const string &password)
{
    ifstream inFile("users.txt");
    if (inFile.is_open())
    {
        string uname, pwd, role;
        while (inFile >> uname >> pwd >> role)
        {
            if (uname == username && pwd == password)
            {
                if (role == "voter")
                {
                    return new Voter(uname, pwd);
                }
            }
        }
        inFile.close();
    }
    return nullptr;
}
// Candidate methods
Candidate *Candidate::loadCandidates(string electionName, int &count)
{
    ifstream inFile("candidates.txt");
    Candidate *candidates = new Candidate[MAX_CANDIDATES];
    count = 0;

    if (inFile.is_open())
    {
        string ename, name, party;
        int votes;
        while (inFile >> ename >> name >> party >> votes)
        {
            if (ename == electionName && count < MAX_CANDIDATES)
            {
                candidates[count] = Candidate(name, party);
                for (int i = 0; i < votes; i++)
                {
                    candidates[count].incrementVotes();
                }
                count++;
            }
        }
        inFile.close();
    }
    return candidates;
}

// Election methods
Election::Election(string n, string t, string s, string e)
    : name(n), type(t), startDate(s), endDate(e), candidateCount(0) {}

void Election::addCandidate(string name, string party)
{
    if (candidateCount < MAX_CANDIDATES)
    {
        candidates[candidateCount] = Candidate(name, party);
        candidates[candidateCount].saveToFile(this->name);
        candidateCount++;
    }
}

void Election::saveToFile()
{
    ofstream outFile("elections.txt", ios::app);
    if (outFile.is_open())
    {
        outFile << name << " " << type << " " << startDate << " " << endDate << endl;
        outFile.close();
    }
}

Election *Election::loadElection(string name)
{
    ifstream inFile("elections.txt");
    if (inFile.is_open())
    {
        string ename, type, start, end;
        while (inFile >> ename >> type >> start >> end)
        {
            if (ename == name)
            {
                if (type == "local")
                {
                    return new LocalElection(ename, start, end);
                }
                else if (type == "national")
                {
                    return new NationalElection(ename, start, end);
                }
            }
        }
        inFile.close();
    }
    return nullptr;
}

// Voter methods
void Voter::displayMenu()
{
    char choice;
    bool flag=true;
    do
    {
        cout << "\nVoter Menu:\n";
        cout << "1. View Elections\n";
        cout << "2. Cast Vote\n";
        cout << "3. Check Vote Status\n";
        cout << "4. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case '1':
            viewElections();
            break;
        case '2':
            castVote();
            break;
        case  '3':
            checkVoteStatus();
            break;
        case '4':
            cout << "Logging out...\n";
            flag=false;
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (flag);
}

void Voter::viewElections()
{
    ifstream inFile("elections.txt");
    if (inFile.is_open())
    {
        string name, type, start, end;
        cout << "\nAvailable Elections:\n";
        while (inFile >> name >> type >> start >> end)
        {
            cout << name << " (" << type << "): " << start << " to " << end << endl;
        }
        inFile.close();
    }
}

void Voter::castVote()
{
    string electionName;
    cout << "Enter election name: ";
    cin >> electionName;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        int count;
        Candidate *candidates = Candidate::loadCandidates(electionName, count);

        cout << "\nCandidates:\n";
        for (int i = 0; i < count; i++)
        {
            cout << i + 1 << ". " << candidates[i].getName() << " (" << candidates[i].getParty() << ")\n";
        }

        int choice;
        cout << "Select candidate (1-" << count << "): ";
        cin >> choice;

        if (choice > 0 && choice <= count)
        {
            candidates[choice - 1].incrementVotes();
            // Save updated votes
            ofstream outFile("candidates.txt", ios::out);
            if (outFile.is_open())
            {
                for (int i = 0; i < count; i++)
                {
                    outFile << electionName << " " << candidates[i].getName()
                            << " " << candidates[i].getParty()
                            << " " << candidates[i].getVotes() << endl;
                }
                outFile.close();
                cout << "Vote cast successfully!\n";
            }
        }
        delete[] candidates;
        delete election;
    }
    else
    {
        cout << "Election not found.\n";
    }
}

void Voter::checkVoteStatus()
{
    cout << "it's not implemented yet\n";
}

// Administrator methods
void Administrator::displayMenu()
{
    char choice;
    bool flag=true;
    do
    {
        cout << "\nAdministrator Menu:\n";
        cout << "1. Create Election\n";
        cout << "2. Add Candidate\n";
        cout << "3. View Results\n";
        cout << "4. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case '1':
            createElection();
            break;
        case '2':
            addCandidate();
            break;
        case '3':
            viewResults();
            break;
        case '4':
            cout << "Logging out...\n";
            flag=false;
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (flag);
}
bool checkElectionName(string name)
{
    // Check if election name already exists in the file
    ifstream inFile("elections.txt");
    if (inFile.is_open())
    {
        string ename, type, start, end;
        while (inFile >> ename >> type >> start >> end)
        {
            if (ename == name)
            {
                cout << "Election name already exists. Please choose another name.\n";
                inFile.close();
                return false;
            }
        }
        inFile.close();
    }
    return true;
}
// bool checkdateformat()
// {
    
//     // Check if start date is before end date
//     struct tm tmStart = {0}, tmEnd = {0};
//     strptime(start.c_str(), "%Y-%m-%d", &tmStart);
//     strptime(end.c_str(), "%Y-%m-%d", &tmEnd);

//     if (difftime(mktime(&tmStart), mktime(&tmEnd)) > 0)
//     {
//         cout << "Start date must be before end date.\n";
//         return false;
//     }
//     return true;
// }
// bool checkElectionDate(string start, string end)
// {
//     // Check if start date is before end date
//     struct tm tmStart = {0}, tmEnd = {0};
//     strptime(start.c_str(), "%Y-%m-%d", &tmStart);
//     strptime(end.c_str(), "%Y-%m-%d", &tmEnd);

//     if (difftime(mktime(&tmStart), mktime(&tmEnd)) > 0)
//     {
//         cout << "Start date must be before end date.\n";
//         return false;
//     }
//     return true;
// }

bool parseDate(const string& dateStr, tm& date)
{
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");
    return !ss.fail();
}

bool isStartBeforeEnd(const string& start, const string& end)
{
    tm tmStart = {}, tmEnd = {};

    istringstream ssStart(start);
    istringstream ssEnd(end);

    ssStart >> get_time(&tmStart, "%Y-%m-%d");
    ssEnd >> get_time(&tmEnd, "%Y-%m-%d");

    // Check parsing success
    if (ssStart.fail() || ssEnd.fail())
        return false;

    // Convert to time_t for comparison
    time_t tStart = mktime(&tmStart);
    time_t tEnd = mktime(&tmEnd);

    // If conversion failed, return false
    if (tStart == -1 || tEnd == -1)
        return false;

    return tStart <= tEnd;
}
bool isValidDateFormat(const string& dateStr)
{
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-')
        return false;

    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");

    if (ss.fail()) return false;

    // Check range validity
    if (date.tm_year < 0 || date.tm_mon < 0 || date.tm_mon > 11 || date.tm_mday < 1 || date.tm_mday > 31)
        return false;

    return true;
}
void Administrator::createElection()
{
    string name, type, start, end;
    Tag1:
    cout << "Enter election name: ";
    cin >> name;
    if (!checkElectionName(name))
    {
        goto Tag1;
    }
    cout << "Enter election type (local/national): ";
    cin >> type;
    Tag:
    Tag2:
    cout << "Enter start date (YYYY-MM-DD): ";
    cin >> start;
    if(!isValidDateFormat(start))
    {
        cout<<"Invalid date format. Please enter date in YYYY-MM-DD format.\n";
        goto Tag2;
    }
    Tag3:
    cout << "Enter end date (YYYY-MM-DD): ";
    cin >> end;
    if(!isValidDateFormat(end))
    {
        cout<<"Invalid date format. Please enter date in YYYY-MM-DD format.\n";
        goto Tag3;
    }
    if(!isStartBeforeEnd(start, end))
    {
        cout << "Start date must be before end date.Re enter the Dates!\n";
        goto Tag;
    }
    Election *election = nullptr;
    if (type == "local")
    {
        string district;
        cout << "Enter district: ";
        cin >> district;
        election = new LocalElection(name, start, end, district);
    }
    else if (type == "national")
    {
        string country;
        cout << "Enter country: ";
        cin >> country;
        election = new NationalElection(name, start, end, country);
    }

    if (election)
    {
        election->saveToFile();
        cout << "Election created successfully!\n";
        delete election;
    }
    else
    {
        cout << "Invalid election type.\n";
    }
}

void Administrator::addCandidate()
{
    string electionName, name, party;
    cout << "Enter election name: ";
    cin >> electionName;
    cout << "Enter candidate name: ";
    cin >> name;
    cout << "Enter party affiliation: ";
    cin >> party;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        election->addCandidate(name, party);
        cout << "Candidate added successfully!\n";
        delete election;
    }
    else
    {
        cout << "Election not found.\n";
    }
}

void Administrator::viewResults()
{
    string electionName;
    cout << "Enter election name: ";
    cin >> electionName;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        election->displayResults();
        delete election;
    }
    else
    {
        cout << "Election not found.\n";
    }
}

// LocalElection methods
void LocalElection::displayDetails()
{
    cout << "Local Election: " << name << endl;
    cout << "District: " << district << endl;
    cout << "Period: " << startDate << " to " << endDate << endl;
}

void LocalElection::conductElection()
{
    cout << "Conducting local election for " << district << " district" << endl;
}

void ShowWinner(string electionName)
{
    int count;
    Candidate *candidates = Candidate::loadCandidates(electionName, count);
    int maxVotes = 0;
    string winnerName;
    for (int i = 0; i < count; i++)
    {
        if (candidates[i].getVotes() > maxVotes)
        {
            maxVotes = candidates[i].getVotes();
            winnerName = candidates[i].getName();
        }
    }
    cout << "Winner of the election " << electionName << " is " << winnerName << " with " << maxVotes << " votes.\n";
}
void LocalElection::displayResults()
{
    cout << "\nResults for Local Election: " << name << endl;
    cout << "District: " << district << endl;

    int count;
    Candidate *candidates = Candidate::loadCandidates(name, count);

    for (int i = 0; i < count; i++)
    {
        cout << candidates[i].getName() << " (" << candidates[i].getParty()
             << "): " << candidates[i].getVotes() << " votes" << endl;
    }
    ShowWinner(name);
    // cout << "Winner of the election " << name << " is " << candidates[0].getName() << " with " << candidates[0].getVotes() << " votes.\n";
    delete[] candidates;
}

// NationalElection methods
void NationalElection::displayDetails()
{
    cout << "National Election: " << name << endl;
    cout << "Country: " << country << endl;
    cout << "Period: " << startDate << " to " << endDate << endl;
}

void NationalElection::conductElection()
{
    cout << "Conducting national election for " << country << endl;
}

void NationalElection::displayResults()
{
    cout << "\nResults for National Election: " << name << endl;
    cout << "Country: " << country << endl;

    int count;
    Candidate *candidates = Candidate::loadCandidates(name, count);

    for (int i = 0; i < count; i++)
    {
        cout << candidates[i].getName() << " (" << candidates[i].getParty()
             << "): " << candidates[i].getVotes() << " votes" << endl;
    }
    ShowWinner(name);
    
    delete[] candidates;
}
bool CheckCNIClogin(string cnic)
{
    // Check if CNIC is valid (for simplicity, just check length)
    if (cnic.length() != 13)
    {
        cout << "Invalid lenght of CNIC. Please enter a valid 13-digit CNIC.\n";
        return false;
    }
    for (int i = 0; i < cnic.length(); i++)
    {
        if (!isdigit(cnic[i]))
        {
            cout << "Invalid Digits of CNIC. Please enter a valid 13-digit CNIC.\n";
            return false;
        }
    }
    if (!(cnic[0] == '3' && cnic[1] == '6' && cnic[2] == '5' && cnic[3] == '0' && cnic[4] == '2'))
    {
        cout << "Invalid First Five Digits CNIC. Please enter a valid 13-digit CNIC.\n";
        ;
        return false;
    }
    return true;
}

bool CheckCNIC(string cnic)
{
    // Check if CNIC is valid (for simplicity, just check length)
    if (cnic.length() != 13)
    {
        cout << "Invalid lenght of CNIC. Please enter a valid 13-digit CNIC.\n";
        return false;
    }
    for (int i = 0; i < cnic.length(); i++)
    {
        if (!isdigit(cnic[i]))
        {
            cout << "Invalid Digits of CNIC. Please enter a valid 13-digit CNIC.\n";
            return false;
        }
    }
    if (!(cnic[0] == '3' && cnic[1] == '6' && cnic[2] == '5' && cnic[3] == '0' && cnic[4] == '2'))
    {
        cout << "Invalid First Five Digits CNIC. Please enter a valid 13-digit CNIC.\n";
        ;
        return false;
    }

    // Check if CNIC already exists in the file
    ifstream inFile("users.txt");
    if (inFile.is_open())
    {
        string uname, pwd, role;
        while (inFile >> uname >> pwd >> role)
        {
            if (uname == cnic)
            {
                cout << "CNIC already exists. Please Register with another CNIC or Login using this!\n";
                DisplayMainMenu();
                inFile.close();
                return true;
            }
        }
        inFile.close();
    }
    return true;
}

void DisplayMainMenu()
{

    char choice;
    // if (choice == '0' && choice == 1 && choice == 2 && choice == 3 && choice == 4)
    // {
    //     cout << "Invalid choice. Try again.\n";
    // }
    do
    {
        cout << "\nOnline Voting System\n";
        cout << "1. Login AS Admin \n";
        cout << "2. Login AS Voter \n";
        cout << "3. Register as Voter\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case '1':
        {
        Tag1:
            string cnic, password;
            cout << "Enter CNIC: ";
            cin >> cnic;
            if (!CheckCNIClogin(cnic))
            {
                goto Tag1;
            }
            cout << "Enter password: ";
            cin >> password;

            User *user = User::authenticateAdmin(cnic, password);
            if (user != nullptr)
            {
                user->displayMenu();
                delete user;
            }
            else
            {
                cout << "Invalid credentials.\n";
            }
    
            break;
        }

        case '2':
        {
        Tag2:
            string cnic, password;
            cout << "Enter CNIC: ";
            cin >> cnic;
            if (!CheckCNIClogin(cnic))
            {
                goto Tag2;
            }
            cout << "Enter password: ";
            cin >> password;

            User *user = User::authenticatevoter(cnic, password);
            if (user != nullptr)
            {
                user->displayMenu();
                delete user;
            }
            else
            {
                cout << "Invalid credentials.\n";
            }
            break;
        }
        case '3':
        {
        up:
            string cnic, password;
            cout << "Choose a CNIC: ";
            cin >> cnic;
            if (!CheckCNIC(cnic))
            {
                goto up;
            }
            // Check if CNIC is valid (for simplicity, just check length)
            cout << "Choose a password: ";
            cin >> password;

            int age;
            cout << "Enter your age: ";
            cin >> age;
            if (age < 18)
            {
                cout << "You must be at least 18 years old to register.\n\n";
                DisplayMainMenu();
                break;
            }
            // Check if username already exists
            Voter newVoter(cnic, password);
            User::saveToFile(newVoter);
            cout << "Registration successful! You can now login.\n";
            DisplayMainMenu();
        }
        case '4':
            cout << "Exiting system. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);
}
// Main program
int main()
{
    cout << "\nOnline Voting System\n";
    // Initialize some sample data
    ofstream userFile("users.txt");
    if (userFile.is_open())
    {
        userFile << "3650212345678 123 admin\n";
        userFile << "3650209876543 123 voter\n";
        userFile << "3650234567890 123 voter\n";
        userFile.close();
    }

    DisplayMainMenu();
    return 0;
}