// Online Voting System

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <windows.h>
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

// Add this near your other global constants
bool isElectionActive = false;   // Global election status flag
string currentElectionName = ""; // Track which election is active

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
    virtual void saveToFile() = 0;
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
    void viewElectionDetails();
    void viewElectionCandidates();
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

    void startElection()
    {
        string electionName;
        cout << "\033[1;36mEnter election name to start: \033[0m";
        cin >> electionName;

        // Simple validation - check if election exists
        ifstream inFile("elections.txt");
        bool exists = false;
        if (inFile.is_open())
        {
            string name, type, start, end, extraInfo;
            while (inFile >> name >> type >> start >> end>>extraInfo)
            {
                if (name == electionName)
                {
                    exists = true;
                    break;
                }
            }
            inFile.close();
        }

        if (exists)
        {
            isElectionActive = true;
            currentElectionName = electionName;
            cout << "\033[1;32mElection started successfully!\033[0m\n";
        }
        else
        {
            cout << "\033[1;31mElection not found.\033[0m\n";
        }
    }

    void endElection()
    {
        if (isElectionActive)
        {
            isElectionActive = false;
            cout << "\033[1;32mElection '" << currentElectionName << "' ended successfully!\033[0m\n";
            currentElectionName = "";
        }
        else
        {
            cout << "\033[1;31mNo active election to end.\033[0m\n";
        }
    }
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
    void saveToFile() override
    {
        ofstream outFile("elections.txt", ios::app);
        if (outFile.is_open())
        {
            outFile << name << " " << type << " " << startDate << " " << endDate << " " << district << endl;
            outFile.close();
        }
    }
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
    void saveToFile() override
    {
        ofstream outFile("elections.txt", ios::app);
        if (outFile.is_open())
        {
            outFile << name << " " << type << " " << startDate << " " << endDate << " " << country << endl;
            outFile.close();
        }
    }
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
        string ename, type, start, end, extraInfo;
        while (inFile >> ename >> type >> start >> end >> extraInfo)
        {
            if (ename == name)
            {
                if (type == "local")
                {
                    // inFile >> extraInfo; // Read district
                    return new LocalElection(ename, start, end, extraInfo);
                }
                else if (type == "national")
                {
                    // inFile >> extraInfo; // Read country
                    return new NationalElection(ename, start, end, extraInfo);
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
    bool flag = true;
    do
    {
        cout << "\033[1;35m\nVoter Menu:\033[0m\n";
        cout << "\033[1;36m1. View Elections\033[0m\n";
        cout << "\033[1;36m2. View Election Details\033[0m\n";
        cout << "\033[1;36m3. View Election Candidates\033[0m\n";
        cout << "\033[1;36m4. Cast Vote\033[0m\n";
        cout << "\033[1;36m5. Check Vote Status\033[0m\n";
        cout << "\033[1;36m6. Logout\033[0m\n";
        cout << "\033[1;35mEnter your choice: \033[0m";
        cin >> choice;

        switch (choice)
        {
        case '1':
            viewElections();
            break;
        case '2':
            viewElectionDetails();
            break;
        case '3':
            viewElectionCandidates();
            break;
        case '4':
            castVote();
            break;
        case '5':
            checkVoteStatus();
            break;
        case '6':
            cout << "\033[1;32mLogging out...\033[0m\n";
            flag = false;
            break;
        default:
            cout << "\033[1;31mInvalid choice. Try again.\033[0m\n";
        }
    } while (flag);
}
void Voter::viewElections()
{
    ifstream inFile("elections.txt");
    if (inFile.is_open())
    {
        string name, type, start, end, extraInfo;
        cout << "\033[1;34m\nAvailable Elections:\033[0m\n";
        cout<<"Election Name  Type  Start Date  End Date  District/Country\n";
        while (inFile >> name >> type >> start >> end>>extraInfo)
        {
            cout << "\033[1;36m" << name << "\033[0m   (" 
                 << "\033[1;33m" << type << "\033[0m): "
                 << "\033[1;32m" << start << "\033[0m to "
                 << "\033[1;32m" << end << "\033[0m "
                 << "\033[1;33m" << extraInfo << "\033[0m\n";
        }
        inFile.close();
    }
}

void Voter::castVote()
{
    if (!isElectionActive)
    {
        cout << "\033[1;31mNo election is currently active.\033[0m\n";
        return;
    }

    string electionName;
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> electionName;

    if (electionName != currentElectionName)
    {
        cout << "\033[1;31mThis election is not currently active.\033[0m\n";
        return;
    }

    // Check if user has already voted
    ifstream voterfile("voters.txt");
    if (voterfile.is_open())
    {
        string uname, votedElection;
        while (voterfile >> uname >> votedElection)
        {
            if (uname == username && votedElection == electionName)
            {
                cout << "\033[1;31mYou have already voted in this election.\033[0m\n";
                voterfile.close();
                return;
            }
        }
        voterfile.close();
    }

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        int count;
        Candidate *candidates = Candidate::loadCandidates(electionName, count);

        cout << "\033[1;34m\nCandidates:\033[0m\n";
        for (int i = 0; i < count; i++)
        {
            cout << "\033[1;36m" << i + 1 << ". " << candidates[i].getName() << "\033[0m ("
                 << "\033[1;33m" << candidates[i].getParty() << "\033[0m)\n";
        }

        int choice;
        cout << "\033[1;36mSelect candidate (1-" << count << "): \033[0m";
        cin >> choice;

        if (choice > 0 && choice <= count)
        {
            candidates[choice - 1].incrementVotes();

            // Save only updated candidates of this election carefully
            ofstream outFile("candidates.txt", ios::app);
            if (outFile.is_open())
            {
                for (int i = 0; i < count; i++)
                {
                    outFile << electionName << " " << candidates[i].getName()
                            << " " << candidates[i].getParty()
                            << " " << candidates[i].getVotes() << endl;
                }
                outFile.close();
                cout << "\033[1;32mVote cast successfully!\033[0m\n";

                hasVoted = true;
                ofstream voterAppend("voters.txt", ios::app);
                if (voterAppend.is_open())
                {
                    voterAppend << username << " " << electionName << endl;
                    voterAppend.close();
                }
                else
                {
                    cerr << "\033[1;31mUnable to open voters file for writing.\033[0m\n";
                }
            }
            else
            {
                cerr << "\033[1;31mUnable to open candidates file for writing.\033[0m\n";
            }
        }
        delete[] candidates;
        delete election;
    }
    else
    {
        cout << "\033[1;31mElection not found.\033[0m\n";
    }
}

void Voter::checkVoteStatus()
{
    if (hasVoted)
    {
        cout << "\033[1;32mYou have already cast your vote.\033[0m\n";
    }
    else
    {
        cout << "\033[1;31mYou have not cast your vote yet.\033[0m\n";
    }
}

void Voter::viewElectionDetails()
{
    string electionName;
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> electionName;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        election->displayDetails();
        delete election;
    }
    else
    {
        cout << "\033[1;31mElection not found.\033[0m\n";
    }
}

void Voter::viewElectionCandidates()
{
    string electionName;
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> electionName;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        int count;
        Candidate *candidates = Candidate::loadCandidates(electionName, count);

        cout << "\033[1;34m\nCandidates:\033[0m\n";
        for (int i = 0; i < count; i++)
        {
            cout << "\033[1;36mName: \033[0m" << candidates[i].getName()
                 << "\t\033[1;33mParty: \033[0m(" << candidates[i].getParty() << ")\n";
        }
        delete[] candidates;
        delete election;
    }
    else
    {
        cout << "\033[1;31mElection not found.\033[0m\n";
    }
}

// Administrator methods
void Administrator::displayMenu()
{
    char choice;
    bool flag = true;
    do
    {
        cout << endl;
        cout << "\033[1;35m========================================\033[0m\n";
        cout << "\033[1;35m|           Administrator Menu         |\033[0m\n";
        cout << "\033[1;35m========================================\033[0m\n";
        cout << "\033[1;35m| 1. Create Election                   |\033[0m\n";
        cout << "\033[1;35m| 2. Add Candidate                     |\033[0m\n";
        cout << "\033[1;35m| 3. View Results                      |\033[0m\n";
        cout << "\033[1;35m| 4. Start Election                    |\033[0m\n";
        cout << "\033[1;35m| 5. End Election                      |\033[0m\n";
        cout << "\033[1;35m| 6. Logout                            |\033[0m\n";
        cout << "\033[1;35m========================================\033[0m\n";
        cout << "\033[1;35mEnter your choice: \033[0m";
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
            startElection();
            break;
        case '5':
            endElection();
            break;
        case '6': // Update logout to be case 6 now
            cout << "\033[1;32mLogging out...\033[0m\n";
            flag = false;
            break;
        default:
            cout << "\033[1;31mInvalid choice. Try again.\033[0m\n";
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
                cout << "\033[1;31mElection name already exists. Please choose another name.\033[0m\n";
                inFile.close();
                return false;
            }
        }
        inFile.close();
    }
    return true;
}

bool parseDate(const string &dateStr, tm &date)
{
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");
    return !ss.fail();
}

bool isStartBeforeEnd(const string &start, const string &end)
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
bool isValidDateFormat(const string &dateStr)
{
    if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-')
        return false;

    tm date = {};
    istringstream ss(dateStr);
    ss >> get_time(&date, "%Y-%m-%d");

    if (ss.fail())
        return false;

    // Check range validity
    if (date.tm_year < 0 || date.tm_mon < 0 || date.tm_mon > 11 || date.tm_mday < 1 || date.tm_mday > 31)
        return false;

    return true;
}
void Administrator::createElection()
{
    string name, type, start, end;
Tag1:
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> name;
    if (!checkElectionName(name))
    {
        goto Tag1;
    }
    cout << "\033[1;36mEnter election type (local/national): \033[0m";
    cin >> type;
Tag:
Tag2:
    cout << "\033[1;36mEnter start date (YYYY-MM-DD): \033[0m";
    cin >> start;
    if (!isValidDateFormat(start))
    {
        cout << "\033[1;31mInvalid date format. Please enter date in YYYY-MM-DD format.\033[0m\n";
        goto Tag2;
    }
Tag3:
    cout << "\033[1;36mEnter end date (YYYY-MM-DD): \033[0m";
    cin >> end;
    if (!isValidDateFormat(end))
    {
        cout << "\033[1;31mInvalid date format. Please enter date in YYYY-MM-DD format.\033[0m\n";
        goto Tag3;
    }
    if (!isStartBeforeEnd(start, end))
    {
        cout << "\033[1;31mStart date must be before end date. Re-enter the dates!\033[0m\n";
        goto Tag;
    }

    Election *election = nullptr;
    if (type == "local")
    {
        string district;
        cout << "\033[1;36mEnter district: \033[0m";
        cin >> district;
        election = new LocalElection(name, start, end, district);
    }
    else if (type == "national")
    {
        string country;
        cout << "\033[1;36mEnter country: \033[0m";
        cin >> country;
        election = new NationalElection(name, start, end, country);
    }

    if (election)
    {
        election->saveToFile();
        cout << "\033[1;32mElection created successfully!\033[0m\n";
        delete election;
    }
    else
    {
        cout << "\033[1;31mInvalid election type.\033[0m\n";
    }
}

void Administrator::addCandidate()
{
    string electionName, name, party;
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> electionName;
    cout << "\033[1;36mEnter candidate name: \033[0m";
    cin >> name;
    cout << "\033[1;36mEnter party affiliation: \033[0m";
    cin >> party;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        election->addCandidate(name, party);
        cout << "\033[1;32mCandidate added successfully!\033[0m\n";
        delete election;
    }
    else
    {
        cout << "\033[1;31mElection not found.\033[0m\n";
    }
}

void Administrator::viewResults()
{
    string electionName;
    cout << "\033[1;36mEnter election name: \033[0m";
    cin >> electionName;

    Election *election = Election::loadElection(electionName);
    if (election)
    {
        election->displayResults();
        delete election;
    }
    else
    {
        cout << "\033[1;31mElection not found.\033[0m\n";
    }
}

// LocalElection methods
void LocalElection::displayDetails()
{
    cout << "\033[1;34mLocal Election: \033[0m" << name << endl;
    cout << "\033[1;34mDistrict: \033[0m" << district << endl;
    cout << "\033[1;34mPeriod: \033[0m" << startDate << " to " << endDate << endl;
}

void LocalElection::conductElection()
{
    cout << "\033[1;32mConducting local election for \033[0m" << district << " district" << endl;
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
    cout << "\033[1;32mWinner of the election \033[0m" << electionName << " \033[1;32mis \033[0m" << winnerName << " \033[1;32mwith \033[0m" << maxVotes << " \033[1;32mvotes.\033[0m\n";
}
void LocalElection::displayResults()
{
    cout << "\033[1;34m\nResults for Local Election: \033[0m" << name << endl;
    cout << "\033[1;34mDistrict: \033[0m" << district << endl;

    int count;
    Candidate *candidates = Candidate::loadCandidates(name, count);

    for (int i = 0; i < count; i++)
    {
        cout << "\033[1;34m" << candidates[i].getName() << "\033[0m ("
             << "\033[1;33m" << candidates[i].getParty() << "\033[0m): "
             << "\033[1;32m" << candidates[i].getVotes() << " votes\033[0m" << endl;
    }
    ShowWinner(name);
    delete[] candidates;
}

// NationalElection methods
void NationalElection::displayDetails()
{
    cout << "\033[1;34mNational Election: \033[0m" << name << endl;
    cout << "\033[1;34mCountry: \033[0m" << country << endl;
    cout << "\033[1;34mPeriod: \033[0m" << startDate << " to " << endDate << endl;
}

void NationalElection::conductElection()
{
    cout << "\033[1;32mConducting national election for \033[0m" << country << endl;
}

void NationalElection::displayResults()
{
    cout << "\033[1;34m\nResults for National Election: \033[0m" << name << endl;
    cout << "\033[1;34mCountry: \033[0m" << country << endl;

    int count;
    Candidate *candidates = Candidate::loadCandidates(name, count);

    for (int i = 0; i < count; i++)
    {
        cout << "\033[1;36m" << candidates[i].getName() << "\033[0m ("
             << "\033[1;33m" << candidates[i].getParty() << "\033[0m): "
             << "\033[1;32m" << candidates[i].getVotes() << " votes\033[0m" << endl;
    }
    ShowWinner(name);

    delete[] candidates;
}

bool CheckCNIClogin(string cnic)
{
    if (cnic.length() != 13)
    {
        cout << "\033[1;31mInvalid length of CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
        return false;
    }
    for (int i = 0; i < cnic.length(); i++)
    {
        if (!isdigit(cnic[i]))
        {
            cout << "\033[1;31mInvalid digits in CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
            return false;
        }
    }
    if (!(cnic[0] == '3' && cnic[1] == '6' && cnic[2] == '5' && cnic[3] == '0' && cnic[4] == '2'))
    {
        cout << "\033[1;31mInvalid first five digits of CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
        return false;
    }
    return true;
}

bool CheckCNIC(string cnic)
{
    if (cnic.length() != 13)
    {
        cout << "\033[1;31mInvalid length of CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
        return false;
    }
    for (int i = 0; i < cnic.length(); i++)
    {
        if (!isdigit(cnic[i]))
        {
            cout << "\033[1;31mInvalid digits in CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
            return false;
        }
    }
    if (!(cnic[0] == '3' && cnic[1] == '6' && cnic[2] == '5' && cnic[3] == '0' && cnic[4] == '2'))
    {
        cout << "\033[1;31mInvalid first five digits of CNIC. Please enter a valid 13-digit CNIC.\033[0m\n";
        return false;
    }

    ifstream inFile("users.txt");
    if (inFile.is_open())
    {
        string uname, pwd, role;
        while (inFile >> uname >> pwd >> role)
        {
            if (uname == cnic)
            {
                cout << "\033[1;31mCNIC already exists. Please register with another CNIC or login using this!\033[0m\n";
                DisplayMainMenu();
                inFile.close();
                return true;
            }
        }
        inFile.close();
    }
    return true;
}
// Function to display the main menu and handle user input
void DisplayMainMenu()
{
    char choice;
    do
    {
        cout << "\033[1;35m1. Login AS Admin\033[0m\n";
        cout << "\033[1;35m2. Login AS Voter\033[0m\n";
        cout << "\033[1;35m3. Register as Voter\033[0m\n";
        cout << "\033[1;35m4. Exit\033[0m\n";
        cout << "\033[1;35mEnter your choice: \033[0m";
        cin >> choice;

        switch (choice)
        {
        case '1':
        {
        Tag1:
            string cnic, password;
            cout << "\033[1;36mEnter CNIC: \033[0m";
            cin >> cnic;
            if (!CheckCNIClogin(cnic))
            {
                goto Tag1;
            }
            cout << "\033[1;36mEnter password: \033[0m";
            cin >> password;

            User *user = User::authenticateAdmin(cnic, password);
            if (user != nullptr)
            {
                user->displayMenu();
                delete user;
            }
            else
            {
                cout << "\033[1;31mInvalid credentials.\033[0m\n";
            }

            break;
        }

        case '2':
        {
        Tag2:
            string cnic, password;
            cout << "\033[1;36mEnter CNIC: \033[0m";
            cin >> cnic;
            if (!CheckCNIClogin(cnic))
            {
                goto Tag2;
            }
            cout << "\033[1;36mEnter password: \033[0m";
            cin >> password;

            User *user = User::authenticatevoter(cnic, password);
            if (user != nullptr)
            {
                user->displayMenu();
                delete user;
            }
            else
            {
                cout << "\033[1;31mInvalid credentials.\033[0m\n";
            }
            break;
        }

        case '3':
        {
        up:
            string cnic, password;
            cout << "\033[1;36mChoose a CNIC: \033[0m";
            cin >> cnic;
            if (!CheckCNIC(cnic))
            {
                goto up;
            }

            cout << "\033[1;36mChoose a password: \033[0m";
            cin >> password;

            int age;
            cout << "\033[1;36mEnter your age: \033[0m";
            cin >> age;
            if (age < 18)
            {
                cout << "\033[1;31mYou must be at least 18 years old to register.\033[0m\n\n";
                DisplayMainMenu();
                break;
            }

            Voter newVoter(cnic, password);
            User::saveToFile(newVoter);
            cout << "\033[1;32mRegistration successful! You can now login.\033[0m\n";
            DisplayMainMenu();
            break;
        }

        case '4':
            cout << "\033[1;32mExiting system. Goodbye!\033[0m\n";
            break;

        default:
            cout << "\033[1;31mInvalid choice. Try again.\033[0m\n";
        }
    } while (choice != '4');
}

// Main program
int main()
{
    cout << "\033[1;32m";
    cout << "       ============================================================\n";
    cout << "                      Welcome to Online Voting System              \n";
    cout << "       ============================================================\n";
    cout << "\033[0m";
    cout << endl;
    // Initialize some sample data
    // ofstream userFile("users.txt");
    // if (userFile.is_open())
    // {
    //     userFile << "3650212345678 123 admin\n";
    //     userFile << "3650212345678 123 voter\n";
    //     userFile << "3650234567890 123 voter\n";
    //     userFile.close();
    // }

    DisplayMainMenu();
    return 0;
}