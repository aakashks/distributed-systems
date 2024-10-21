#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <map>
#include <algorithm>

using namespace std;

class BSS
{
    vector<vector<int>> vector_clocks;

    // need to maintain the list of msgs (also 1 sender could have sent multiple msgs)
    // so keep a map from msg to vector t.s. for every sender
    vector<unordered_map<string, vector<int>>> messages;

    // store received messages (actually delivered ones)
    vector<vector<pair<int, string>>> rec_msgs;

    // every process maintains its buffer which stores delayed messages
    vector<vector<pair<int, string>>> buffer;

    int n;

    bool check_bss(int sender, vector<int> &vt, vector<int> &local)
    {
        // check if the message should be delayed or kept
        if (local[sender] != vt[sender] - 1)
            return false;
        for (int i = 0; i < n; i++)
            if (i != sender && local[i] < vt[i])
                return false;

        return true;
    }

    void check_buffer(vector<int> local, int receiver)
    {
        // check previous entries in buffer that can now be delivered
        for (auto it = buffer[receiver].begin(); it != buffer[receiver].end();)
        {
            int s = it->first;
            string m = it->second;
            if (messages[s].find(m) != messages[s].end() && check_bss(s, messages[s][m], local))
            {
                rec_msgs[receiver].emplace_back(s, m);
                merge_vcs(local, messages[s][m]);
                cout << "recv_A p" << s + 1 << " " << m << " " << print_vc(local) << endl;
                it = buffer[receiver].erase(it);
            }
            else
                it++;
        }
    }

public:
    BSS(int n = 9) : n(n), vector_clocks(n, vector<int>(n, 0)), messages(n), rec_msgs(n), buffer(n) {}

    string broadcast(int sender, string msg)
    {
        vector_clocks[sender][sender]++;
        messages[sender][msg] = vector_clocks[sender];
        return print_vc(vector_clocks[sender]);
    }

    void receive_B(int sender, string msg, int receiver)
    {
        vector<int> &local = vector_clocks[receiver];
        cout << "recv_B p" << sender + 1 << " " << msg << " " << print_vc(local) << endl;

        check_buffer(local, receiver);

        // check if the current message can be delivered
        if (messages[sender].find(msg) != messages[sender].end() && check_bss(sender, messages[sender][msg], local))
        {
            rec_msgs[receiver].emplace_back(sender, msg);
            merge_vcs(local, messages[sender][msg]);
            cout << "recv_A p" << sender + 1 << " " << msg << " " << print_vc(local) << endl;
        }

        else
            buffer[receiver].emplace_back(sender, msg);

        check_buffer(local, receiver);
    }

    /////

    string print_vc(const vector<int> &vc)
    {
        string s = "(";
        for (auto x : vc)
            s += to_string(x);
        s += ")";
        return s;
    }

    void merge_vcs(vector<int> &local, vector<int> &received)
    {
        for (int i = 0; i < local.size(); i++)
        {
            local[i] = max(local[i], received[i]);
        }
    }
};

int main()
{
    string line;
    vector<pair<int, vector<string>>> operations;
    int pi;
    int np = 0;

    while (getline(cin, line))
    {
        if (line.substr(0, 13) == "begin process")
        {
            string process_name = line.substr(14);
            pi = stoi(process_name.substr(1)) - 1;
            operations.push_back({pi, {}});
        }
        else if (line.substr(0, 11) == "end process")
        {
            np++;
            continue;
        }
        else
        {
            operations.back().second.push_back(line);
        }
    }

    BSS bss(np);

    for (auto [pid, ops] : operations)
    {
        cout << "begin process p" << pid + 1 << endl;

        for (string &op : ops)
        {
            if (op.substr(0, 4) == "send")
            {
                string msg = op.substr(5);
                op += " ";
                op += bss.broadcast(pid, msg);
            }
            else if (op.substr(0, 6) == "recv_B")
            {
                int sender_id = stoi(op.substr(8, 1)) - 1;
                string msg = op.substr(10);
                bss.receive_B(sender_id, msg, pid);
            }
        }
        cout << "end process p" << pid + 1 << endl << endl;
    }

    return 0;
}