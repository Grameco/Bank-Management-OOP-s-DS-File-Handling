#include <iostream>
#include <fstream>
#include <map>
using namespace std;

#define MIN_Balance 500

// a class which will be called if balance < 500 after withdrawel
class insufficientFunds
{
};

// to store the account details and access them
class Account
{
private:
    long accNumber;
    string fname;
    string lname;
    float balance;

    // to ensure that nextAccNumber stores the same value for each instance of Account, we make it static
    static long nextAccNumber;

public:
    // default constructor to call account to run some function
    Account(){};

    // parameterised constructor to put account details
    Account(string fname, string lname, float balance)
    {
        // update the account number one step next
        nextAccNumber++;
        accNumber = nextAccNumber;

        this->fname = fname;
        this->lname = lname;
        this->balance = balance;
    }

    void setName()
    {
        string newfname, newlname;
        float newbalance;
        int confirm;
        cout << "\nEnter the first name: ";
        cin >> newfname;
        cout << "\nEnter the last name: ";
        cin >> newlname;
        cout << "\nWant to update balance? Confirm by entring 1" << endl;
        cin >> confirm;

        if (confirm == 1)
        {
            cout << "\nEnter the balance: ";
            cin >> newbalance;
            this->balance = newbalance;
        }

        this->fname = newfname;
        this->lname = newlname;
    }

    long getAccNumber()
    {
        return accNumber;
    }

    string getFirstName()
    {
        return fname;
    }

    string getLastName()
    {
        return lname;
    }

    float getBalance()
    {
        return balance;
    }

    void deposit(float amount)
    {
        balance += amount;
    }

    void withdraw(float amount)
    {
        if (balance - amount < MIN_Balance)
        {
            throw insufficientFunds();
        }
        balance -= amount;
    }

    // static functions for static variables
    static void setLastAccNumber(long accNumber);
    static long getLastAccountNumber();

    friend ofstream &operator<<(ofstream &ofs, Account &a);
    friend ifstream &operator>>(ifstream &ifs, Account &a);
    friend ostream &operator<<(ostream &os, Account &a);
};

// values of static variables is defined outside the class
long Account ::nextAccNumber = 0;

// static functions are declared outside
void Account ::setLastAccNumber(long accNumber)
{
    nextAccNumber = accNumber;
}

long Account ::getLastAccountNumber()
{
    return nextAccNumber;
}

// overload '<<' operator to write 'Account details' in the file and return the file stream
ofstream &operator<<(ofstream &ofs, Account &a)
{
    ofs << a.accNumber << endl;
    ofs << a.fname << endl;
    ofs << a.lname << endl;
    ofs << a.balance << endl;
    return ofs;
}

// overload '>>' operator to read 'Account details' from the file stream and assign to corresponding values
ifstream &operator>>(ifstream &ifs, Account &a)
{
    ifs >> a.accNumber;
    ifs >> a.fname;
    ifs >> a.lname;
    ifs >> a.balance;
    return ifs;
}

// to print the file data on console and return the stream
ostream &operator<<(ostream &os, Account &a)
{
    os << "First Name: " << a.getFirstName() << endl;
    os << "Last Name " << a.getLastName() << endl;
    os << "Account Number: " << a.getAccNumber() << endl;
    os << "Balance: " << a.getBalance() << endl;
    return os;
}

class Bank
{
private:
    // store accounts <account number, Account> so we can reuse the file to perform other functions
    // we will iterate the map instead of file to get the data
    map<long, Account> m;

public:
    Bank();

    Account openAccount(string fname, string lname, float balance);

    Account balanceEnquiry(long accNumber);

    Account deposit(long accNumber, float amount);

    Account withdraw(long accNumber, float amount);

    Account updateAccount(long accNumber);

    bool searchAccNumber(long accNumber);

    void closeAccount(long accNumber);

    void showAllAccounts();

    // New member function to check if a name already exists
    bool isNameExists(string firstName, string lastName) const
    {
        for (auto i : m)
        {
            if (i.second.getFirstName() == firstName && i.second.getLastName() == lastName)
            {
                return true; // Name exists
            }
        }
        return false; // Name does not exist
    }

    ~Bank();
};

Bank ::Bank()
{
    Account a;
    ifstream infile;

    infile.open("Bank.data");
    if (!infile)
    {
        cout << "Error in Opening! File not Found!!" << endl;
        return;
    }

    // read account information from the file until the end of the file is reached.
    while (!infile.eof())
    {
        infile >> a;
        m.insert(pair<long, Account>(a.getAccNumber(), a)); // map account number and account
    }

    Account ::setLastAccNumber(a.getAccNumber()); // update the last account number with the new one

    infile.close();
}

Account Bank ::openAccount(string fname, string lname, float balance)
{
    ofstream outfile;
    Account a(fname, lname, balance); // create an account
    m.insert(pair<long, Account>(a.getAccNumber(), a));

    outfile.open("Bank.data", ios ::trunc); // overrite from start and erase the previously written data

    for (auto i : m)
    {
        outfile << i.second; // write all the existing record from map to the file
    }

    outfile.close();
    return a;
}

Account Bank ::balanceEnquiry(long accNumber)
{
    if (!searchAccNumber(accNumber))
    {
        cout << "Account not found! Please enter right account number." << endl;
        exit(0);
    }

    map<long, Account>::iterator i;
    i = m.find(accNumber);
    return i->second;
}

Account Bank ::deposit(long accNumber, float amount)
{
    if (!searchAccNumber(accNumber))
    {
        cout << "Account not found! Please enter right account number." << endl;
        exit(0);
    }
    map<long, Account>::iterator i;
    i = m.find(accNumber);
    i->second.deposit(amount);
    return i->second;
}

Account Bank ::withdraw(long accNumber, float amount)
{
    if (!searchAccNumber(accNumber))
    {
        cout << "Account not found! Please enter right account number." << endl;
        exit(0);
    }

    map<long, Account>::iterator i;
    i = m.find(accNumber);
    i->second.withdraw(amount);
    return i->second;
}

Account Bank ::updateAccount(long accNumber)
{
    if (!searchAccNumber(accNumber))
    {
        cout << "Account not found! Please enter right account number." << endl;
        exit(0);
    }

    map<long, Account>::iterator i;
    i = m.find(accNumber);
    i->second.setName();
    return i->second;
}

bool Bank ::searchAccNumber(long accNumber)
{
    for (auto i : m)
    {
        if (i.first == accNumber)
            return true;
    }
    return false;
}

void Bank ::closeAccount(long accNumber)
{
    if (!searchAccNumber(accNumber))
    {
        cout << "Account not found! Please enter right account number." << endl;
        exit(0);
    }

    map<long, Account>::iterator i;
    i = m.find(accNumber);
    m.erase(i);
    cout << "Account is Closed." << endl;
}

void Bank ::showAllAccounts()
{
    for (auto i : m)
    {
        cout << "Account Number: " << i.first << endl
             << i.second << endl;
    }
}

Bank ::~Bank()
{
    ofstream outfile;
    outfile.open("Bank.data", ios ::trunc);

    if (!outfile)
    {
        cout << "Error opening file for writing." << endl;
        return;
    }

    // before the scope of map ends, make sure to print all its data into the file
    for (auto i : m)
        outfile << i.second;

    outfile.close();
}

int main()
{
    Bank b;
    Account a;

    int choice;
    string fname, lname;
    long accNumber;
    float balance, amount;

    cout << "*********** WELCOME TO OUR BANKING SYSTEM ***********" << endl;

    do
    {
        cout << "\n\tSelect one option below ";
        cout << "\n\t1 Open an Account";
        cout << "\n\t2 Balance Enquiry";
        cout << "\n\t3 Deposit";
        cout << "\n\t4 Withdrawal";
        cout << "\n\t5 Close an Account";
        cout << "\n\t6 Show All Accounts";
        cout << "\n\t7 Update account details";
        cout << "\n\t8 Quit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            cout << "\nEnter First Name: ";
            cin >> fname;
            cout << "\nEnter Last Name: ";
            cin >> lname;

            // Check if the name already exists before opening an account
            if (b.isNameExists(fname, lname))
            {
                cout << "Account with the given name already exists. Cannot open a new account." << endl;
                break;
            }
            cout << "\nEnter Opening Balance: ";
            cin >> balance;

            if (balance < 500)
            {
                cout << "\nSorry! You need at least 500 in order to open an account.\nPlease try again with a higher amount." << endl;
                break;
            }

            a = b.openAccount(fname, lname, balance);

            cout << "\nThe account has been successfully opened." << endl;
            cout << a; // overloaded '<<' operator to show ostream file data in the console
            break;
        }

        case 2:
        {
            cout << "\nEnter Account Number: ";
            cin >> accNumber;
            a = b.balanceEnquiry(accNumber);
            cout << endl
                 << "Your account  details : " << endl;
            cout << a;
            break;
        }

        case 3:
        {
            cout << "\nEnter the account number: ";
            cin >> accNumber;
            cout << "\nEnter the amount to deposit: ";
            cin >> amount;
            a = b.deposit(accNumber, amount);
            cout << "\nDeposited Successfully!" << endl;
            cout << a;
            break;
        }

        case 4:
        {
            cout << "\nEnter the account number: ";
            cin >> accNumber;
            cout << "\nEnter amount to withdraw: ";
            cin >> amount;
            try
            {
                a = b.withdraw(accNumber, amount);
                cout << "Amount Withdrawn." << endl;
                cout << a;
            }
            catch (insufficientFunds &e)
            {
                cout << "Insufficient Balance!!!" << endl;
            }
            break;
        }

        case 5:
        {
            cout << "\nEnter the account number: ";
            cin >> accNumber;
            b.closeAccount(accNumber);
            break;
        }

        case 6:
        {
            b.showAllAccounts();
            break;
        }

        case 7:
        {
            cout << "\nEnter the account number: ";
            cin >> accNumber;
            a = b.updateAccount(accNumber);
            cout << "Updated Account Information" << endl;
            cout << a;
        }

        case 8:
        {
            cout << "Quitting the Banking System. Thank you!" << endl;
            break;
        }

        default:
        {
            cout << "\nInvalid Option! Please Enter Again.\n";
            exit(0);
        }
        }

    } while (choice != 8);

    return 0;
}