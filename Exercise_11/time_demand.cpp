#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <cmath>
using namespace std;

struct processor
{
    int id;
    int ability;
};

struct task
{
    int id;
    int releTime;
    int execTime;
    int deadline;
    int period;
    int preempt;
    int type;
    int slackTime;
    vector<int> preced;
};

enum algorithm
{
    SJF,
    EDF,
    LSTF
};

bool determine_schedulable(int n, struct task t[]);

int main()
{
    FILE *fp;
    int m = 0, n = 0, c = 0;
    vector<bool> isSchedulable;
    bool schedule_flag = false;

    fp = freopen("input.txt", "r", stdin);
    while (cin >> m >> n)
    {
        struct processor p[m];
        struct task t[n];
        vector<int> periods;
        int hyperPeriod = 0;

        for (int i = 0; i < m; i++)
        {
            cin >> p[i].id >> p[i].ability;
        }
        for (int i = 0; i < n; i++)
        {
            cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
        }
        cin >> c; // how many rules of precedence
        for (int i = 0; i < c; i++)
        {
            int high; // task with higher precedence
            int low;  // task with lower precedence
            cin >> high >> low;
            t[low].preced.push_back(high);
        }

        for (int i = 0; i < n; i++)
        {
            periods.push_back(t[i].period); // Put periods of tasks in an array
        }

        bool result = determine_schedulable(n, t);
        isSchedulable.push_back(result);
    }
    fclose(fp);

    for (int i = 0; i < isSchedulable.size(); i++)
    {
        cout << "Test case " << i + 1 << " : ";
        isSchedulable[i] ? (cout << "schedulable") : (cout << "not schedulable");
        cout << endl;
    }
    return 0;
}

bool determine_schedulable(int n, struct task t[])
{
    bool check = false;
    for (int i = 0; i < n; i++)
    {
        check = false;
        for (float time = 1; time <= t[i].period; time++)
        {
            int w = t[i].execTime;
            for (int k = 0; k < i; k++)
            {
                w += (ceil(time / t[k].period) * t[k].execTime);
            }
            //cout << "W" << i << "(" << time << ") = " << w << endl;
            if (w <= time)
            {
                check = true;
                break;
            }
        }
        if (check == false)
        {
            return false;
        }
    }
    return true;
}