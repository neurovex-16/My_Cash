#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <limits>
#include <cctype>
#include <conio.h>
using namespace std;
#define BOLD        "\033[1m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define BLUE        "\033[34m"
#define LIGHT_PINK  "\033[95m"
#define ORANGE "\033[38;5;208m"
#define RESET       "\033[0m"
string getHiddenPin(int length);
class Member 
{
public:
    string mobile;
    string name;
    string pin;
    double amount;
};
class History
{
public:
    int tid;
    string mobile;
    string type;
    double amount;
    double balance;
    time_t timeStamp;
};
vector<Member> members;
int tranID = 100;
bool validMobile(string m) 
{
    return m.size() == 11 && m[0] == '0';
}
int findMember(string mob) 
{
    for (int i = 0; i < members.size(); i++)
        if (members[i].mobile == mob) return i;
    return -1;
}
void loadMembers() 
{
    ifstream in("members.txt");
    Member m;
    while (in >> m.mobile >> m.name >> m.amount >> m.pin)
        members.push_back(m);
}
void saveMembers() 
{
    ofstream out("members.txt");
    for (auto &m : members)
        out << m.mobile << " " << m.name << " " << m.amount << " " << m.pin << endl;
}
void loadLastTranID() 
{
    ifstream in("history.txt");
    History h;
    while (in >> h.tid >> h.mobile >> h.type >> h.amount >> h.balance >> h.timeStamp)
        tranID = h.tid;
}

bool verifyOTP() 
{
    srand(time(0));
    int otp = rand() % 9000 + 1000;
    time_t startTime = time(0);
    cout << "\033[33m";
    cout << "myCash OTP: " << otp << endl;
    cout << "\033[0m"; 
    cout << "Enter OTP (valid for 2 minutes): ";
    int userOTP;
    cin >> userOTP;
    time_t currentTime = time(0);
    if (difftime(currentTime, startTime) > 120) 
    {
        cout << "OTP time has expired\n";
        return false;
    }
    if (userOTP != otp) 
    {
        cout << "OTP does NOT match\n";
        return false;
    }
    return true;
}
void saveHistory(string mob, string type, double amt, double bal) 
{
    History h;
    h.tid = ++tranID;
    h.mobile = mob;
    h.type = type;
    h.amount = amt;
    h.balance = bal;
    h.timeStamp = time(0);
    ofstream out("history.txt", ios::app);
    out << h.tid << " "
        << h.mobile << " "
        << h.type << " "
        << h.amount << " "
        << h.balance << " "
        << h.timeStamp << endl;
}
void showHistory(string mob) 
{
    ifstream in("history.txt");
    History h;
    bool found=false;
    cout << left
     << setw(10) << "TranID"
     << setw(15) << "Description"
     << setw(10) << "Amount"
     << setw(10) << "Balance"
     << setw(20) << "Time" << endl;
    cout<<BLUE<<BOLD<<"-------------------------------------------------------------------"<<RESET<<"\n";
    while (in >> h.tid >> h.mobile >> h.type >> h.amount >> h.balance >> h.timeStamp)
    {
        if (h.mobile == mob)
        {
            found=true;
            tm *lt = localtime(&h.timeStamp);
            char timeBuf[30];
            strftime(timeBuf, sizeof(timeBuf), "%d-%m-%Y %H:%M:%S", lt);
            cout << left
                 << setw(10) << h.tid
                 << setw(15) << h.type
                 << setw(10) << fixed << setprecision(0) << h.amount
                 << setw(10) << fixed << setprecision(0) << h.balance
                 <<setw(20)<<timeBuf
                 << endl;
        }
    }
      if (!found)
      {
        cout << RED << BOLD
             << "\nNo previous history found for your account\n"
             << RESET;
      }
}

void registerUser() 
{
    cout<<GREEN<<BOLD<<"-----Register-----"<<RESET<<"\n";
    Member m;
    cout << "Enter Mobile No. (11-digit): ";
    cin >> m.mobile;
    if (!validMobile(m.mobile))
    {
        cout <<RED<<BOLD<<"Invalid Mobile Number"<<RESET<<"\n";
        return;
    }
    if (findMember(m.mobile) != -1) 
    {
        cout << "Member already exists\n";
        return;
    }
    cout << "Enter Name: ";
    cin >> m.name;
    string p1, p2;
    cout << "Enter pin (5-digit): ";
    p1 = getHiddenPin(5);
    if (p1 == "") return;
    if (p1.length() != 5)
  {
    cout <<RED<<BOLD<< "Your pin must be 5 digits, not more or not less"<<RESET<<"\n";
    return;
  }
    cout <<"Reconfirm pin: ";
    p2 = getHiddenPin(5);
    if (p1 != p2) 
    {
        cout <<RED<<BOLD<<"Pins must be same"<<RESET<<"\n";
        return;
    }
    if (!verifyOTP()) return;
    m.pin = p1;
    m.amount = 0;
    members.push_back(m);
    saveMembers();
    cout <<BLUE<<BOLD<< "Registration is Successful."<<RESET<<"\n";
}
int login()
{
    cout << BLUE << BOLD << "-----Login-----" << RESET << "\n";
    string mob, pin;
    cout << "Enter Mobile No. (11-digit): ";
    cin >> mob;
    int idx = findMember(mob);
    if (idx == -1)
    {
        cout <<RED<<BOLD<<"This account doesn't exist"<<RESET<<"\n";
        return -1;
    }
    cout << "Enter pin: ";
    pin = getHiddenPin(5);
    if (members[idx].pin != pin)
    {
         cout << RED << BOLD << "-----Incorrect Pin Number-----" << RESET << "\n";
        return -1;
    }
     cout << BLUE << BOLD << "-----Login Successful-----" << RESET << "\n";
    return idx;
}
void cashIn(int u) 
{
     cout << BLUE << BOLD << "-----CashIn-----" << RESET << "\n";
    double amt;
    cout << "Enter Amount: ";
    cin >> amt;
    members[u].amount += amt;
    saveMembers();
    saveHistory(members[u].mobile, "Cash-in", amt, members[u].amount);
    cout <<BLUE<<BOLD<< "Cash-in is Successful"<<RESET<<"\n";
}
void cashOut(int u) 
{
    cout << BLUE << BOLD << "-----CashOut-----" << RESET << "\n";
    double amt;
    cout << "Enter Amount: ";
    cin >> amt;
    if (members[u].amount < amt) 
    {
        cout <<RED<<BOLD<<"Insufficient Fund"<<RESET<<"\n";
        return;
    }
    cout << "Are you sure(Y/N)? ";
    char c;
    cin >> c;
    if (c != 'Y') 
    {
        cout << "Enter Y or N\n";
        return;
    }
    if (!verifyOTP()) return;
    members[u].amount -= amt;
    saveMembers();
    saveHistory(members[u].mobile, "Cash-out", amt, members[u].amount);
    cout <<BLUE<<BOLD<<"Cash-out is Successful"<<RESET<<"\n";
}
void sendMoney(int u) 
{
    cout << BLUE << BOLD << "-----Send Money-----" << RESET << "\n";
    string dest;
    double amt;
    cout << "Enter Destination no. (11-digit): ";
    cin >> dest;
    int d = findMember(dest);
    if (d == -1) 
    {
        cout <<RED<<BOLD<<"Destination Mobile no. is invalid"<<RESET<<"\n";
        return;
    }
    cout << "Enter Amount: ";
    cin >> amt;
    if (members[u].amount < amt) 
    {
        cout <<RED<<BOLD<<"Insufficient Fund"<<RESET<<"\n";
        return;
    }
    cout << "Are you sure(Y/N)? ";
    char c;
    cin >> c;
    if (c != 'Y') 
    {
        cout << "Enter Y or N\n";
        return;
    }
    if (!verifyOTP()) return;
    members[u].amount -= amt;
    members[d].amount += amt;
    saveMembers();
    saveHistory(members[u].mobile, "SendMoney", amt, members[u].amount);
    cout <<BLUE<<BOLD<<"Send Money is Successful"<<RESET<<"\n";
}
void payBill(int u)
{
     cout << BLUE << BOLD << "-----PayBill-----" << RESET << "\n";
    int t;
    double bills[] = {500, 800, 600, 1000};
    string billName[] = {"Gas", "Electricity", "Water", "Internet"};
    cout << "Enter Bill Type (Gas/Electricity/Water/Internet-1/2/3/4): ";
    cin >> t;
    if (t < 1 || t > 4)
    {
        cout <<RED<<BOLD<<"Invalid Option"<<RESET<<"\n";
        return;
    }
    double amt = bills[t - 1];
    cout << "Your " << billName[t - 1] << " Bill: " << amt << endl;
    if (members[u].amount < amt)
    {
        cout <<RED<<BOLD<<"Insufficient Fund"<<RESET<<"\n";
        return;
    }
    cout << "Want to pay(Y/N)? ";
    char c;
    cin >> c;
    if (c != 'Y')
    {
        cout << "Enter Y or N\n";
        return;
    }
    if (!verifyOTP()) return;

    members[u].amount -= amt;
    saveMembers();
    saveHistory(members[u].mobile, billName[t - 1] + string("_Bill"), amt, members[u].amount);
    cout <<BLUE<<BOLD<<"Bill Payment is Successful"<<RESET<<"\n";
}
void updateMe(int u) 
{
    cout << BLUE << BOLD << "-----Update Me-----" << RESET << "\n";
    string newName, newPin, confirmPin;
    cout << "Old Name: " << members[u].name << endl;
    cout << "New Name (enter to ignore): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, newName);
    cout << "Old pin: *****" << endl;
    cout << "New pin (enter to ignore): ";
    newPin=getHiddenPin(5);
    if (!newPin.empty()) 
    {
        cout << "Confirm New pin: ";
        confirmPin = getHiddenPin(5);
        if (newPin != confirmPin)
        {
            cout <<RED<<BOLD<<"Pins must be same"<<RESET<<"\n";
            return;
        }
    }
    if (!verifyOTP()) return;

    if (!newName.empty())
        members[u].name = newName;

    if (!newPin.empty())
        members[u].pin = newPin;

    saveMembers();
    cout <<BLUE<<BOLD<<"Update is Successful"<<RESET<<"\n";
}
void removeMe(int u) {
    cout << BLUE << BOLD << "-----Remove Me-----" << RESET << "\n";
    if (!verifyOTP()) return;
    members.erase(members.begin() + u);
    saveMembers();
    cout << "Remove is Successful\n";
    cout << "Back to MyCash Login Menu\n";
}
void pressAnyKey()
{
    cout << "\nPress ENTER to go to main menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
string getHiddenPin(int length)
{
    string pin;
    char ch;
    while (true)
    {
        ch = _getch(); 
        if (ch == '\r')
        {
            cout << endl;
            break;
        }
        else if (ch == '\b')
        {
            if (!pin.empty())
            {
                pin.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit(ch))
        {
            pin.push_back(ch);
            cout << "*";
        }
    }
    if (pin.length() != length)
    {
        cout << "PIN must be exactly " << length << " digits\n";
        return "";
    }
    return pin;
}
int main() 
{
    loadMembers();
    loadLastTranID();
    while (1) {
        cout << ORANGE<< BOLD;
        cout << "\n*** Welcome to MyCash Terminal -- 2403127 ***\n";
        cout << RESET;
        cout << "1. Login\n2. Register\n3. Exit\n";
        cout << "Enter option: ";
        int ch;
        cin >> ch;
        if (ch == 2)
        {
          registerUser();
          pressAnyKey();
        }
        else if (ch == 1) 
        {
            int u = login();
            if (u == -1) 
            {
                pressAnyKey();
                continue;
            }
            while (1) 
            {
                cout <<ORANGE<<BOLD<<"\n******** MyCash Menu ********"<<RESET<<"\n";
                cout << "1.Update Me\n2.Remove Me\n3.Send Money\n";
                cout << "4.Cash-in\n5.Cash-out\n6.Pay Bill\n";
                cout << "7.Check Balance\n8.History\n9.Logout\n";
                cout << "Enter option (1-9): ";
                int c;
                cin >> c;
                if (c == 1) 
                {
                    updateMe(u);
                    continue;
                }
                else if (c == 2) 
                {
                removeMe(u);
                break;
                } 
                if (c == 3) sendMoney(u);
                else if (c == 4) cashIn(u);
                else if (c == 5) cashOut(u);
                else if (c == 6) payBill(u);
                else if (c == 7) cout << "Balance: " << members[u].amount << endl;
                else if (c == 8) showHistory(members[u].mobile);
                else if (c == 9) break;
                else cout << "Invalid Option\n";
            }
        } else if (ch == 3) 
        {
            cout<<LIGHT_PINK<<"Thank You for Using MyCash.\nHave a Nice Day."<<RESET<<"\n";
            pressAnyKey();
            break;
        }

        else cout << "Invalid Option\n";
    }

    return 0;
}