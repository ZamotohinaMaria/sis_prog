//sudo su
//sudo chwon root:root laba5
// ./laba5
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
    unsigned int uid;
    unsigned int gid;
    string folder;
    string shell;
    string password;
    vector<string> groups;
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
            //cout << tmp++ << "==" << line << endl;
        }
    }
    return all_line;
}

vector<string> split(string str, char delimiter)
{
    string tmp = {};
    vector<string> buff;

    //cout << "split str = " << str;
    if (delimiter == ':' || delimiter == ',')
    {
        for (int i = 0; i < str.size() + 1; i++)
        {
            if (str[i] == delimiter || str[i] == 0)
            {
                buff.push_back(tmp);
                tmp = {};
            }
            else
                tmp += str[i];
        }
    }

    if (delimiter == '$')
    {
        int count = 0;
        for (int i = 0; i < str.size(); i++)
        {
            if ((str[i] == delimiter || str[i] == 0) && count <= 2)
            {
                buff.push_back(tmp);
                tmp = {};
                count ++;
            }
            else
                tmp += str[i];
        }
        buff.push_back(tmp);
    }

    return buff;
}

void user_cout(user_info tmp)
{
    cout << "\tname = " << tmp.name << endl;
    cout << "\tuid = " << tmp.uid << endl;
    cout << "\tgid = " << tmp.gid << endl;
    cout << "\tfolder = " << tmp.folder << endl;
    cout << "\tshell = " << tmp.shell << endl;
    cout << "\tpassword hesh = " << tmp.password << endl;
    cout << "\tgropus:" << endl;
    for (int i = 0; i <  tmp.groups.size(); i++)
        cout << "\t\t" << i+1 << ") " << tmp.groups[i] << endl;
}

int main()
{
    ifstream passwd_file("/etc/passwd");
    if (!passwd_file) cout << "error passwd" <<endl;

    ifstream shadow_file("/etc/shadow");
    if (!shadow_file) cout << "error shadow" <<endl;

    ifstream group_file("/etc/group");
    if (!shadow_file) cout << "error group" <<endl;

    setuid(getuid());
    vector<string> passwd_read = file_reading(passwd_file);
    vector<string> shadow_read = file_reading(shadow_file);
    vector<string> group_read = file_reading(group_file);
    passwd_file.close();
    shadow_file.close();
    group_file.close();

    vector<vector<string>> passwd_arr;
    vector<vector<string>> shadow_arr;
    vector<vector<string>> group_arr;
    vector<user_info> user_arr;

    for (int i = 0; i < passwd_read.size(); i++)
    {
        passwd_arr.push_back(split(passwd_read[i], ':'));

        user_info tmp;
        tmp.name = passwd_arr[i][0];
        tmp.uid = atoi(passwd_arr[i][2].c_str());
        tmp.gid = atoi(passwd_arr[i][3].c_str());
        tmp.folder = passwd_arr[i][5];
        tmp.shell = passwd_arr[i][6];

        user_arr.push_back(tmp);

        // cout << "i = " << i << endl;
        // for (int j = 0; j < passwd_arr[i].size(); j++)
        //     cout << passwd_arr[i][j] << "\t";
        // cout << endl;
    }
    
    //cout << "\n\n======================================================================\n\n";

    for (int i = 0; i < shadow_read.size(); i++)
    {
        shadow_arr.push_back(split(shadow_read[i], ':'));

        if (shadow_arr[i][1].size() <= 1)
            user_arr[i].password = shadow_arr[i][1];
        else
        {
            vector<string> tmp = split(shadow_arr[i][1], '$');
            user_arr[i].password = tmp[3];
        }

        // cout << "i = " << i << endl;
        // for (int j = 0; j < shadow_arr[i].size(); j++)
        //     cout << shadow_arr[i][j] << "\t";
        // cout << endl;
    }

    //cout << "\n\n======================================================================\n\n";

    for (int i = 0; i < group_read.size(); i++)
    {
        group_arr.push_back(split(group_read[i], ':'));

        if (group_arr[i].size() > 3)
        {
            vector<string> tmp = split(group_arr[i][3], ',');
            for(int k = 0; k < tmp.size(); k++)
            {
                for (int j = 0; j < user_arr.size(); j++)
                {
                    if (user_arr[j].name == tmp[k])
                    {
                        user_arr[j].groups.push_back(group_arr[i][0]);
                        break;
                    }
                }
            }
        }


        // cout << "i = " << i << endl;
        // for (int j = 0; j < group_arr[i].size(); j++)
        //     cout << group_arr[i][j] << "\t";
        // cout << endl;
    }


    for(int i = 0; i < user_arr.size(); i++)
    {
        cout << "User " << i << endl;
        user_cout(user_arr[i]);
        cout << endl;
    }
}
