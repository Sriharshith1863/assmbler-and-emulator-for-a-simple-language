//trupti sriharshith
//2301cs56

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
vector<string> memory(1024, "n");
vector<string> machineCode;
ll A, B, PC, SP;
ll signedValue(string s)
{
        ll value;
        stringstream ss;
        ss << hex << s;
        ss >> value;
        ll bits = s.size() * 4;
        ll svalue = value;
        if (value & (1ll << (bits - 1)))
        {
                // Step 4: Convert to negative by subtracting from power of 2
                svalue = value - (1 << bits);
        }
        return svalue;
}
ll operation(ll opcode, ll value = 0)
{
        switch (opcode)
        {
        case 0:
                B = A;
                A = value;
                return 1;
        case 1:
                A += value;
                return 1;
        case 2:
                B = A;
                A = stoll(memory[SP + value], nullptr, 10);
                return 1;
        case 3:
                memory[SP + value] = to_string(A);
                A = B;
                return 1;
        case 4:
                A = stoll(memory[A + value], nullptr, 10);
                return 1;
        case 5:
                memory[A + value] = to_string(B);
                return 1;
        case 6:
                A += B;
                return 1;
        case 7:
                A = B - A;
                return 1;
        case 8:
                A = B << A;
                return 1;
        case 9:
                A = B >> A;
                return 1;
        case 10:
                SP += value;
                return 1;
        case 11:
                SP = A;
                A = B;
                return 1;
        case 12:
                B = A;
                A = SP;
                return 1;
        case 13:
                B = A;
                A = PC;
                PC += value;
                return 1;
        case 14:
                PC = A;
                A = B;
                return 1;
        case 15:
                if (A == 0)
                        PC += value;
                return 1;
        case 16:
                if (A < 0)
                        PC += value;
                return 1;
        case 17:
                PC += value;
                return 1;
        case 18:
                return 0;
        default:
                cerr << "Invalid opcode: " << opcode << endl;
                return -1;
        }
}
int main()
{
        string fileName;
        cout << "Enter the object file name: ";
        cin >> fileName;
        ifstream file(fileName);
        if (!file)
        {
                cout << "Input file doesn't exist, please make sure file is in same directory as the code!" << endl;
                exit(0);
        }
        string line;
        getline(file, line);
        istringstream iss(line);
        string word;
        ll cur = 0;
        while (iss >> word)
        {
                if (cur >= 1024)
                {
                        break;
                }
                memory[cur++] = word;
        }
        cur = 1;
        while (getline(file, line))
        {
                if (line.size() == 0)
                {
                        cout << "Execution halted due to errors. See the error file for details." << "\n";
                        file.close();
                        return 0;
                }
                machineCode.push_back(line);

                memory[cur++] = line;
        }
        SP = cur + 1;
        file.close();
        PC = 1;
        ll time = 0;
        while (PC <= machineCode.size())
        {
                time++;
                string instruction = memory[PC];
                ll opcode = signedValue(instruction.substr(instruction.size() - 2, 2));
                ll operand = signedValue(instruction.substr(0, instruction.size() - 2));
                ll x = operation(opcode, operand);
                if (x == 0)
                {
                        cout << "HALT found, terminating execution..." << "\n";
                        break;
                }
                else if (x == -1)
                {
                        cout << "Error at line: " << PC << ", invalid opcode encountered." << "\n";
                        return 0;
                }
                PC++;
                if(time >= 1e7) {
                        //ofstream outFile("error.txt");
                        cout << "Execution time exceeded. Halting execution..." << "\n";
                        //outFile.close();
                        return 0;
                
                } 
        }
        ofstream outFile("memoryDump.txt");
        for (ll i = 0; i < memory.size(); i++)
        {
                outFile << memory[i] << " ";
        }
        outFile.close();
        return 0;
}