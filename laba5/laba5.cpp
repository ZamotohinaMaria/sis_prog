#include <iostream>
#include <fstream>
#include <vector>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h> 

#include <sys/types.h>

using namespace std;

struct user_info
{
    string name;
    string x;
    unsigned int uid;
    unsigned int gid;
    string dop;
    string folder;
    string shell;
};

vector<string> file_reading(ifstream& file)
{
    vector<string> all_line;
    string line;

    if (file.is_open())
    {
        int tmp = 1;
        while (getline(file, line))
        {
            all_line.push_back(line);
            cout << tmp++ << "==" << line << endl;
        }
    }
    return all_line;
}

vector<string> split(string str)
{
    string tmp = {};
    vector<string> buff;

    //cout << "split str = " << str;
    for (int i = 0; i < str.size() + 1; i++)
    {
        if (str[i] == ':' || str[i] == 0)
        {
            buff.push_back(tmp);
            tmp = {};
        }
        else
            tmp += str[i];
    }
    //buff.push_back(tmp);
    // for (int i = 0; i < buff.size(); i++)
    //     cout << "=="<<buff[i] << endl;

    // user_info res;
    // res.name = buff[0];
    // res.x = buff[1];
    // res.uid = atoi(buff[2].c_str());
    // res.gid = atoi(buff[3].c_str());
    // res.dop = buff[4];
    // res.folder = buff[5];
    // res.shell = buff[6];

    return buff;
}

void passwd_cout(user_info tmp)
{
    cout << "name = " << tmp.name << endl;
    cout << "uid = " << tmp.uid << endl;
    cout << "gid = " << tmp.gid << endl;
    cout << "dop = " << tmp.dop << endl;
    cout << "folder = " << tmp.folder << endl;
    cout << "shell = " << tmp.shell << endl;
}

int main()
{
    ifstream passwd_file("/etc/passwd");
    if (!passwd_file) cout << "error passwd" <<endl;

    ifstream shadow_file("/etc/shadow");
    if (!shadow_file) cout << "error shadow" <<endl;

    setuid(getuid());
    vector<string> passwd_read = file_reading(passwd_file);
    vector<string> shadow_read = file_reading(shadow_file);
    passwd_file.close();
    shadow_file.close();

    vector<vector<string>> passwd_arr;
    vector<vector<string>> shadow_arr;
    
    for (int i = 0; i < passwd_read.size(); i++)
    {
        passwd_arr.push_back(split(passwd_read[i]));
        cout << "i = " << i << endl;
        for (int j = 0; j < passwd_arr[i].size(); j++)
            cout << passwd_arr[i][j] << "\t";
        cout << endl;
        // passwd_cout(passwd_arr[i]);
    }
    
    cout << "\n\n======================================================================\n\n";

    for (int i = 0; i < shadow_read.size(); i++)
    {
        shadow_arr.push_back(split(shadow_read[i]));
        cout << "i = " << i << endl;
        for (int j = 0; j < shadow_arr[i].size(); j++)
            cout << shadow_arr[i][j] << endl;
        // passwd_cout(passwd_arr[i]);
    }

}