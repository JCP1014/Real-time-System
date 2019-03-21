#include <cstdio>
#include <iostream>
#include <vector>
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
};

int main()
{
    int m = 0, n = 0;
    freopen("input.txt", "r", stdin);
    cin >> m >> n;
    struct processor p[m];
    struct task t[n];
    vector<struct task> q; // waiting queue
    int currTime = 0;   
    int currTask = -1;
    vector<int> restTask;   // tasks that haven't released
    int shortest = 0;   // id of the shortest task in the waiting queue
    bool isSwitch = false;  // context switch
    bool isFirst = true;    // first task
    vector<int> waitingTime;
    double hit = 0; // number of tasks whose finish time <= deadline
    vector<int> copyExec;   // store the original execution time

    // Read input 
    for (int i = 0; i < m; i++)
    {
        cin >> p[i].id >> p[i].ability;
    }
    for (int i = 0; i < n; i++)
    {
        cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
        restTask.push_back(i);
        waitingTime.push_back(0);
    }

    // Store original execution time in another vector
    for(int i=0;i<n;i++)
    {
        copyExec.push_back(t[i].execTime);
    }

    // Start to schedule
    for (int j = 1; j <= m; j++)
    {
        cout << "Processor " << j << ":" << endl;
        shortest = 0;
        while (n > 0)
        {
            // Check release time, and push executable tasks to the waiting queue
            for (int i = 0; i < n; i++)
            {
                if (!restTask.empty())
                {
                    if (t[restTask[i]].releTime <= currTime)
                    {
                        q.push_back(t[restTask[i]]);
                        restTask.erase(restTask.begin() + i);
                    }
                }
            }

            // Choose the task whose execution time is the shortest
            for (int k = 0; k < q.size(); k++)
            {
                if (q[k].execTime < q[shortest].execTime)
                {
                    shortest = k;
                    isSwitch = true;
                }
            }
            currTask = q[shortest].id;

            // Context switch
            if (isSwitch == true)
            {
                
                cout << currTime << endl;
                cout << currTime << " Context Switch " << ++currTime << endl;
                cout << currTime << " Task" << currTask << " ";
                isSwitch = false;
            }
            else
            {
                if (isFirst == true)
                {
                    cout << currTime << " Task" << currTask << " ";
                    isFirst = false;
                }
            }
            --q[shortest].execTime;
            ++currTime;

            // Current task is finished
            if (q[shortest].execTime == 0)
            {
                waitingTime[q[shortest].id] = currTime - q[shortest].releTime - copyExec[q[shortest].id];
                --n;
                if (currTime <= q[shortest].deadline)
                {
                    ++hit;
                }
                q.erase(q.begin() + shortest);
                shortest = 0;
                isSwitch = true;
            }
        }
        cout << currTime << endl;

        // Cauculate the average waiting time and hit rate
        double total = 0;
        for (int i = 0; i < waitingTime.size(); i++)
        {
            total += waitingTime[i];
        }
        double avg = total / waitingTime.size();
        double hitRate = hit / waitingTime.size();
        cout << "Average Waiting Time : " << avg << endl;
        cout << "Hit Rate : " << hitRate << endl;
    }

    return 0;
}
