// trupti sriharshith
// 2301cs56

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <climits>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <queue>
#include <iterator>
#include <utility>
#include <fstream>
#include <sstream>
#include <cctype>
#include <iomanip>
using namespace std;
#define ll long long
// Structure to represent each line in intermediate Representation (IR)
typedef struct IRlist
{
    ll lineNumber = -1;       // Original line number in source
    ll address = -1;          // Address for the line's instruction
    string label = "";        // Label, if present
    string mnemonic = "";     // Mnemonic for the instruction
    string operand = "";      // Operand for the instruction, if any
    string originalLine = ""; // The original line text
    bool isValid = true;      // Validity flag for error checking
} IRlist;

// Global variables and containers
vector<string> memory(1024, "n");
vector<IRlist> IRList;                // List to store intermediate representations of each line
vector<string> errors;                // List to store error messages
string fileName;                      // Input ASM file name
map<string, ll> symbolTable;          // Symbol table for labels and their addresses
map<string, pair<string, ll>> opcode; // Map for opcodes with mnemonic as key and pair as (opcode, operand count)
set<string> invalidLabels;            // Set of invalid labels
map<int, string> operandValue;
// Initialize opcode map with mnemonics, opcodes, and operand counts
void initialize()
{
    opcode["data"] = {"", 1};
    opcode["ldc"] = {"00", 1};
    opcode["adc"] = {"01", 1};
    opcode["ldl"] = {"02", 1};
    opcode["stl"] = {"03", 1};
    opcode["ldnl"] = {"04", 1};
    opcode["stnl"] = {"05", 1};
    opcode["add"] = {"06", 0};
    opcode["sub"] = {"07", 0};
    opcode["shl"] = {"08", 0};
    opcode["shr"] = {"09", 0};
    opcode["adj"] = {"0a", 1};
    opcode["a2sp"] = {"0b", 0};
    opcode["sp2a"] = {"0c", 0};
    opcode["call"] = {"0d", 1};
    opcode["return"] = {"0e", 0};
    opcode["brz"] = {"0f", 1};
    opcode["brlz"] = {"10", 1};
    opcode["br"] = {"11", 1};
    opcode["HALT"] = {"12", 0};
}
bool isValidHexadecimal(const string &s)
{
    bool check = true;
    if (!(s[0] == '0' && s[1] == 'x'))
    {
        check = false;
    }
    for (ll i = 2; i < s.size(); ++i)
    {
        if (!isxdigit(s[i]))
        {
            check = false;
            break;
        }
    }
    return check;
}
bool isValidOctal(const string &s)
{
    bool check = true;
    if (!(s[0] == '0'))
    {
        check = false;
    }
    for (ll i = 1; i < s.size(); ++i)
    {
        if (!(s[i] >= '0' && s[i] <= '7'))
        {
            check = false;
            break;
        }
    }
    return check;
}
bool isDecimal(const string &s)
{
    if (!(s[0] == '+' || s[0] == '-' || (s[0] >= '0' && s[0] <= '9')))
    {
        return false;
    }
    for (ll i = 1; i < s.size(); ++i)
    {
        if (!(s[i] >= '0' && s[i] <= '9'))
        {
            return false;
        }
    }
    return true;
}
// Function to trim leading spaces from a string
string trimLeadingSpaces(string str)
{
    auto it = find_if(str.begin(), str.end(), [](unsigned char ch)
                      { return !isspace(ch); });
    return string(it, str.end());
}

// Function to trim comments from a line
string trimComments(string &line)
{
    size_t semi = line.find(';');
    if (semi != string::npos)
    {
        line = line.substr(0, semi); // Trim anything after ';'
    }
    return line;
}

// Function to validate label names according to naming rules
bool labelName(string &str, ll lineN)
{
    bool flag = false;
    // Check if the first character is a letter
    if (!((str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z')))
    {
        flag = true;
    }
    // Check if remaining characters are alphanumeric
    for (ll i = 1; i < str.size(); ++i)
    {
        if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')))
        {
            flag = true;
        }
    }
    // If invalid label, log an error
    return !flag;
}
bool inRange(ll dv, string mn)
{
    if (mn == "data")
    {
        if (dv > INT_MAX / 2 || dv < INT_MIN / 2)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (dv > 8388607 || dv < -8388608)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}
int main()
{
    initialize(); // Initialize opcode table

    // Prompt user for ASM file name and open the file
    cout << "Enter ASM file name to assemble: ";
    cin >> fileName;
    ifstream file(fileName);
    if (!file)
    {
        cout << "Input file doesn't exist, please make sure file is in same directory as the code!" << endl;
        exit(0);
    }
    int cur = 500;
    string line;
    ll address = 0; // Address counter
    while (getline(file, line))
    {
        IRlist ir;
        ir.address = address;
        ir.lineNumber = address + 1;

        line = trimLeadingSpaces(line); // Trim leading spaces
        ir.originalLine = line;         // Store original line
        line = trimComments(line);      // Remove comments
        // If line is empty after trimming, proceed to the next address
        if (line.size() == 0)
        {
            address++;
            IRList.push_back(ir);
            continue;
        }

        // Process label if present
        string label;
        size_t colon = line.find(':');
        if (colon != string::npos)
        {
            size_t bfc = line.find(' ');
            if (bfc != string::npos && bfc < colon)
            {
                colon--;
                label = line.substr(0, colon - 1);
                line = line.substr(colon + 1, line.size() - 1 - colon);
            }
            else
            {
                label = line.substr(0, colon);
                line = line.substr(colon + 1, line.size() - (colon + 1));
            }
        }
        ir.label = label;
        // Check for duplicate labels in the symbol table
        if (symbolTable.find(label) != symbolTable.end())
        {
            errors.push_back("Error at line: " + to_string(address + 1) + " :Duplicate label");
        }

        // Validate and add label to symbol table
        // if (label.size() > 0)
        // {
        //     if (!labelName(label, address + 1))
        //     {
        //         errors.push_back("Error at line: " + to_string(address + 1) + " :Invalid label name");
        //         invalidLabels.insert(label);
        //     }
        //     if (invalidLabels.find(label) == invalidLabels.end() && symbolTable.find(label) == symbolTable.end()) {
        //         symbolTable[label] = address;
        //     }
        // }

        // Split the line into mnemonic and operands
        vector<string> mneOp;
        line = trimLeadingSpaces(line);
        if (line.size() == 0)
        {
            if (label.size() > 0)
            {
                if (!labelName(label, address + 1))
                {
                    errors.push_back("Error at line: " + to_string(address + 1) + " :Invalid label name");
                    invalidLabels.insert(label);
                }
                if (invalidLabels.find(label) == invalidLabels.end() && symbolTable.find(label) == symbolTable.end())
                {
                    symbolTable[label] = address;
                }
            }
            IRList.push_back(ir);
            address++;
            continue;
        }

        istringstream iss(line);
        string word;
        while (iss >> word)
        {
            mneOp.push_back(word);
        }
        if (ir.label.size() > 0)
        {
            if (!labelName(ir.label, address + 1))
            {
                errors.push_back("Error at line: " + to_string(address + 1) + " :Invalid label name");
                invalidLabels.insert(ir.label);
            }
            bool flag2 = false;
            if (invalidLabels.find(ir.label) == invalidLabels.end() && symbolTable.find(ir.label) == symbolTable.end())
            {
                if (mneOp.size() == 0)
                {
                }
                else if (opcode.find(mneOp[0]) != opcode.end())
                {
                    if (mneOp[0] == "data")
                    {
                        if (mneOp.size() > 1)
                        {
                            if (isValidHexadecimal(mneOp[1]))
                            {
                                string p = mneOp[1];
                                ll dv = stoi(p, nullptr, 16);
                                if (inRange(dv, mneOp[1]))
                                {
                                    flag2 = true;
                                    memory[cur++] = p;
                                    stringstream ss1;
                                    ss1 << setfill('0') << setw(6) << hex << cur - 1;
                                    symbolTable[label] = cur - 1;
                                }
                            }
                            else if (isValidOctal(mneOp[1]))
                            {
                                string p = mneOp[1];
                                ll dv = stoi(p, nullptr, 8);
                                if (inRange(dv, mneOp[1]))
                                {
                                    flag2 = true;
                                    memory[cur++] = p;
                                    stringstream ss1;
                                    ss1 << setfill('0') << setw(6) << hex << cur - 1;
                                    symbolTable[label] = cur - 1;
                                }
                            }
                            else if (isDecimal(mneOp[1]))
                            {
                                string p = mneOp[1];
                                ll dv = stoi(p, nullptr, 10);
                                if (inRange(dv, mneOp[1]))
                                {
                                    flag2 = true;
                                    memory[cur++] = to_string(dv);
                                    stringstream ss1;
                                    ss1 << setfill('0') << setw(6) << hex << cur - 1;
                                    symbolTable[label] = cur - 1;
                                }
                            }
                        }
                    }
                }
                if (!flag2)
                {
                    symbolTable[ir.label] = address;
                }
            }
        }
        // Populate IR fields
        ir.label = label;
        ir.mnemonic = mneOp[0];
        bool check = false;
        // Check if mnemonic is valid
        if (opcode.find(mneOp[0]) == opcode.end())
        {
            errors.push_back("Error at line: " + to_string(address + 1) + " :Unknown instruction(mnemonic)");
            ir.isValid = false;
            check = true;
        }
        // Validate operand count based on opcode requirements
        if (mneOp.size() != opcode[mneOp[0]].second + 1 && check == false)
        {
            errors.push_back("Error at line: " + to_string(address + 1) + " :Invalid no of operand(s)");
            ir.isValid = false;
        }

        // If there is an operand, assign it to IR operand field
        if ((opcode[mneOp[0]].second))
        {
            if (mneOp.size() > 1)
            {
                ir.operand = mneOp[1];
            }
        }

        IRList.push_back(ir); // Add IR entry
        address++;
    }
    file.close();

    // second pass
    vector<string> machineCode;
    for (auto &x : IRList)
    {
        if (!x.isValid)
        {
            machineCode.push_back("i");
            continue;
        }
        if (x.mnemonic.size() == 0)
        {
            machineCode.push_back("");
            continue;
        }
        if (x.operand.size() != 0)
        {
            string s;
            if (symbolTable.find(x.operand) != symbolTable.end())
            {
                if (x.mnemonic == "ldc" || x.mnemonic == "adc" || x.mnemonic == "adj")
                {
                    stringstream ss;
                    string s1 = to_string(symbolTable[x.operand]);
                    operandValue[x.address] = s1;
                    ss << setfill('0') << setw(6) << hex << (symbolTable[x.operand]);
                    s = ss.str();
                }
                else
                {
                    stringstream ss;
                    string s1 = to_string(symbolTable[x.operand] - x.address - 1);
                    operandValue[x.address] = s1;
                    ss << setfill('0') << setw(6) << hex << (symbolTable[x.operand] - x.address - 1);
                    s = ss.str();
                }
            }
            else if (isValidHexadecimal(x.operand))
            {
                s = x.operand;
                ll dv = stoi(s, nullptr, 16);
                if (!inRange(dv, x.mnemonic))
                {
                    errors.push_back("Error at line: " + to_string(x.lineNumber) + " :Overflow/Underflow in operand");
                    x.isValid = false;
                    machineCode.push_back("i");
                    continue;
                }
                operandValue[x.address] = to_string(dv);
                s = s.substr(2, s.size() - 2);
            }
            else if (isValidOctal(x.operand))
            {
                ll dv = stoi(x.operand, nullptr, 8);
                if (!inRange(dv, x.mnemonic))
                {
                    errors.push_back("Error at line: " + to_string(x.lineNumber) + " :Overflow/Underflow in operand");
                    x.isValid = false;
                    machineCode.push_back("i");
                    continue;
                }
                operandValue[x.address] = to_string(dv);
                stringstream ss;
                ss << setfill('0') << setw(6) << hex << dv;
                s = ss.str();
            }
            else if (isDecimal(x.operand))
            {
                ll dv = stoi(x.operand);
                if (!inRange(dv, x.mnemonic))
                {
                    errors.push_back("Error at line: " + to_string(x.lineNumber) + " :Overflow/Underflow in operand");
                    x.isValid = false;
                    machineCode.push_back("i");
                    continue;
                }
                operandValue[x.address] = to_string(dv);
                stringstream ss;
                ss << setfill('0') << setw(6) << hex << dv;
                s = ss.str();
            }
            else if (labelName(x.operand, x.lineNumber))
            {
                errors.push_back("Error at line: " + to_string(x.lineNumber) + " : Undefined label");
                x.isValid = false;
                machineCode.push_back("i");
                continue;
            }
            else
            {
                errors.push_back("Error at line: " + to_string(x.lineNumber) + " :Invalid Operand");
                x.isValid = false;
                machineCode.push_back("i");
                continue;
            }
            if (x.mnemonic == "data")
            {
                if (x.label.size() == 0)
                {
                    int dv = stoi(s, nullptr, 16);
                    memory[cur++] = to_string(dv);
                }
                continue;
            }
            else
            {
                if (s.size() > 6)
                {

                    s = s.substr(s.size() - 6, 6);
                }
                machineCode.push_back(s + opcode[x.mnemonic].first);
            }
        }
        else
        {
            string zr;
            for (ll i = 0; i < 6; ++i)
            {
                zr.push_back('0');
            }
            machineCode.push_back(zr + opcode[x.mnemonic].first);
        }
    }
    ofstream outFile("machineCode.txt");
    for (auto &data : memory)
    {
        outFile << data << " ";
    }
    outFile << "\n";
    for (auto &code : machineCode)
    {
        if (code.size() == 0)
            continue;
        if (code == "i")
        {
            outFile << "" << endl;
            continue;
        }
        outFile << code << endl;
    }
    outFile.close();
    ofstream outFile1("listingFile.lst");
    for (int i = 0; i < machineCode.size(); ++i)
    {
        stringstream ss;
        ss << setfill('0') << setw(6) << hex << IRList[i].address;
        if (machineCode[i].size() != 0 && machineCode[i] != "i")
        {
            outFile1 << ss.str() << " " << machineCode[i] << " ";
        }
        else
        {
            outFile1 << ss.str() << " " << "        " << " ";
        }
        if (!IRList[i].isValid)
        {
            outFile1 << IRList[i].originalLine << endl;
            continue;
        }
        if (IRList[i].operand.size() == 0)
        {
            outFile1 << IRList[i].originalLine << endl;
        }
        else if (symbolTable.find(IRList[i].operand) != symbolTable.end())
        {
            outFile1 << IRList[i].label << " " << IRList[i].mnemonic << " " << operandValue[i] << endl;
        }
        else
        {
            outFile1 << IRList[i].originalLine << endl;
        }
    }
    outFile1.close();
    ofstream outfile2("error.txt");
    for (auto &error : errors)
    {
        outfile2 << error << endl;
    }
    outfile2.close();
    return 0;
}