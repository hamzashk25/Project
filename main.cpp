// Online Voting System

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
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

    static User *authenticateUser(const string &username, const string &password);
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
    LocalElection(string n, string s, string e, string d = "Default District")
        : Election(n, "local", s, e), district(d) {}

    void displayDetails() override;
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
User *User::authenticateUser(const string &username, const string &password)
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
                else if (role == "admin")
                {
                    return new Administrator(uname, pwd);
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
    int choice;
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
        case 1:
            viewElections();
            break;
        case 2:
            castVote();
            break;
        case 3:
            checkVoteStatus();
            break;
        case 4:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 4);
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
    int choice;
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
        case 1:
            createElection();
            break;
        case 2:
            addCandidate();
            break;
        case 3:
            viewResults();
            break;
        case 4:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 4);
}

void Administrator::createElection()
{
    string name, type, start, end;
    cout << "Enter election name: ";
    cin >> name;
    cout << "Enter election type (local/national): ";
    cin >> type;
    cout << "Enter start date (YYYY-MM-DD): ";
    cin >> start;
    cout << "Enter end date (YYYY-MM-DD): ";
    cin >> end;

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
    delete[] candidates;
}

// Main program
int main()
{
    // Initialize some sample data
    ofstream userFile("users.txt");
    if (userFile.is_open())
    {
        userFile << "admin 123 admin\n";
        userFile << "voter 123 voter\n";
        userFile << "hamza 123 voter\n";
        userFile.close();
    }

    int choice;
    do
    {
        cout << "\nOnline Voting System\n";
        cout << "1. Login\n";
        cout << "2. Register as Voter\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Login functionality
            // int select;
            // cout << "Select role:\n1. Voter\n2. Admin\n";
            // cout << "Enter your choice: ";
            // cin >> select;
            // switch (select)
            // {
            // case 1:
            // {
            //     string username, password;
            //     cout << "Enter username: ";
            //     cin >> username;
            //     cout << "Enter password: ";
            //     cin >> password;
            //     cout << "Login as Voter\n";
            //     User *user = User::authenticateUser(username, password);
            //     user->displayMenu();
            //     break;
            // }
            // case 2:
            // {
            //     // Login as admin
            //     string uname, pass;
            //     cout << "Enter username: ";
            //     cin >> uname;
            //     cout << "Enter password: ";
            //     cin >> pass;
            //     cout << "Login as Admin\n";
            //     User *user = User::authenticateUser(uname, pass);
            //     break;
            // }
            // default:
            //     cout << "Invalid choice. Try again.\n";
            //     break;
            // }

            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            User *user = User::authenticateUser(username, password);
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
        case 2:
        {
            string username, password;
            cout << "Choose a username: ";
            cin >> username;
            cout << "Choose a password: ";
            cin >> password;

            Voter newVoter(username, password);
            User::saveToFile(newVoter);
            cout << "Registration successful! You can now login.\n";
            break;
        }
        case 3:
            cout << "Exiting system. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 3);

    return 0;
}