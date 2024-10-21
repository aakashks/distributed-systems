#include <iostream>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <map>
#include <algorithm>

using namespace std;

string print_vc(const vector<int> &vc)
{
    string s;
    for (auto x : vc)
        s += to_string(x);

    return "(" + s + ")";
}

void merge_vcs(vector<int> &local, vector<int> &received)
{
    for (int i = 0; i < local.size(); i++)
        local[i] = max(local[i], received[i]);
}

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

    // operations (final output)
    vector<vector<string>> operations;

    // halt operation outputs and resume again using this
    vector<int> halted_op;
    vector<pair<int, string>> halted_for_msg;

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

public:
    BSS(int n = 9) : n(n), vector_clocks(n, vector<int>(n, 0)), messages(n), 
        rec_msgs(n), buffer(n), operations(n), halted_op(n, -1), 
        halted_for_msg(n, make_pair(-1, "")) {}

    void push_operation(int pid, string op) {
        operations[pid].push_back(op);
    }

    void run_rec_ops(int pid, int op_id) {
        for (int i = op_id; i < operations[pid].size(); i++) {
            auto& op = operations[pid][i];
            if (op.substr(0, 4) == "send") {
                string msg = op.substr(5);
                op += " " + broadcast(pid, msg);

                // check if the any process is halted for this message
                for (int j = 0; j < n; j++) {
                    if (halted_op[j] != -1 && halted_for_msg[j] == make_pair(pid, msg)) {
                        halted_for_msg[j] = {-1, ""};
                        run_rec_ops(j, halted_op[j]);
                    }
                }
            }
            else if (op.substr(0, 6) == "recv_B")
            {
                int sender_id = stoi(op.substr(8, 1)) - 1;
                string msg = op.substr(10);

                // check if message exists or not
                if (messages[sender_id].count(msg) == 0) {
                    halted_op[pid] = i;
                    halted_for_msg[pid] = {sender_id, msg};
                    return;
                }
                op += " " + print_vc(vector_clocks[pid]);
                receive_B(sender_id, msg, pid, i);
            }
            else
                cout << "Error: Invalid operation" << endl;
        }
    }

    void simulate() {
        for (int pid = 0; pid < n; pid++) {
            run_rec_ops(pid, 0);
        }
    }

    void print_output(vector<int> pids) {
        for (int i = 0; i < n; i++) {
            int id = pids[i];
            cout << "begin process p" << id + 1 << endl;
            for (auto &op : operations[id]) {
                if (!op.empty()) cout << op << endl;
            }
            cout << "end process p" << id + 1;
            // if (i != n - 1) 
                cout << endl << endl;
        }
    }

    string broadcast(int sender, string msg)
    {
        vector_clocks[sender][sender]++;
        messages[sender][msg] = vector_clocks[sender];
        return print_vc(vector_clocks[sender]);
    }

    void receive_B(int sender, string msg, int receiver, int op_id)
    {
        vector<int> &local = vector_clocks[receiver];

        check_buffer(receiver, op_id);

        // check if the current message can be delivered
        if (check_bss(sender, messages[sender][msg], local))
        {
            rec_msgs[receiver].emplace_back(sender, msg);
            merge_vcs(local, messages[sender][msg]);
            operations[receiver].insert(
                operations[receiver].begin() + op_id + 1, 
                "recv_A p" + to_string(sender + 1) + " " + msg + " " + print_vc(local));
            
            check_buffer(receiver, op_id+1);
        }
        else
            buffer[receiver].emplace_back(sender, msg);

    }

    void check_buffer(int receiver, int op_id)
    {
        vector<int> &local = vector_clocks[receiver];
        // check previous entries in buffer that can now be delivered
        for (auto it = buffer[receiver].begin(); it != buffer[receiver].end();)
        {
            int s = it->first;
            string m = it->second;
            if (messages[s].count(m) && check_bss(s, messages[s][m], local))
            {
                rec_msgs[receiver].emplace_back(s, m);
                merge_vcs(local, messages[s][m]);
                operations[receiver].insert(
                    operations[receiver].begin() + op_id + 1, 
                    "recv_A p" + to_string(s + 1) + " " + m + " " + print_vc(local));
                it = buffer[receiver].erase(it);
                
                it = buffer[receiver].begin();
                op_id++;
            }
            else
                it++;
        }
    }
};

int main()
{
    string line;
    vector<int> pids;
    vector<vector<string>> operations(9);
    int pi;
    int np = 0;
    int max_pid = 0;

    while (getline(cin, line))
    {
        if (line.substr(0, 13) == "begin process")
        {
            string process_name = line.substr(14);
            pi = stoi(process_name.substr(1)) - 1;
            pids.push_back(pi);
            max_pid = max(max_pid, pi);
        }
        else if (line.substr(0, 11) == "end process")
        {
            np++;
            continue;
        }
        else
            operations[pi].push_back(line);

    }

    if (max_pid == 0 || max_pid != np-1)
    {
        cout << "Error" << endl;
        return 0;
    }

    BSS bss(np);

    for (auto pid : pids)
    {
        for (string &op : operations[pid])
            bss.push_operation(pid, op);
    }

    bss.simulate();
    bss.print_output(pids);

    return 0;
}