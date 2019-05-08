#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <algorithm>
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
int gcd(int a, int b);
int find_hyperPeriod(vector<int> &period, int n);
void scheduling(int algo, int m, int n, struct processor p[], struct task t[]);

int main()
{   
    int m = 0, n = 0, c = 0;
    freopen("input.txt", "r", stdin);
    //freopen("input(non-preemtable).txt", "r", stdin);
    cin >> m >> n;
    struct processor p[m];
    struct task t[n];
    for (int i = 0; i < m; i++)
    {
        cin >> p[i].id >> p[i].ability;
    }
    for (int i = 0; i < n; i++)
    {
        cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
    }
    cout << "_____With preemption:_____" << endl << endl;
    scheduling(SJF,m,n,p,t);
    scheduling(EDF,m,n,p,t);
    scheduling(LSTF,m,n,p,t);
    cout << endl;

    freopen("input(non-preemtable).txt", "r", stdin);
    cin >> m >> n;
    for (int i = 0; i < m; i++)
    {
        cin >> p[i].id >> p[i].ability;
    }
    for (int i = 0; i < n; i++)
    {
        cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
    }
    cout << "_____No preemption:_____" << endl << endl;
    scheduling(SJF,m,n,p,t);
    scheduling(EDF,m,n,p,t);
    scheduling(LSTF,m,n,p,t);

    return 0;
}

/* Find the greatest common divisor of twon numbers */
int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}
/* Find the least common multiple of periods, that is hyper-period */
int find_hyperPeriod(vector<int> &period, int n)
{
    int hyper = period[0];
    for (int i = 1; i < n; i++)
    {
        if (period[i] > 0)
            hyper = period[i] * hyper / gcd(period[i], hyper);
    }
    return hyper;
}

void scheduling(int algo, int m, int n, struct processor p[], struct task t[])
{
    vector<struct task> q;
    int currTime = 0;
    int currTask = -1;
    int best = 0;
    vector<int> copyExec;
    vector<int> periods;
    vector<int> newRele; // For updating release time
    vector<int> waitingTime;
    double hit = 0;
    double avg_wait = 0;
    double total_jobs = 0;
    bool isChange = true;
    bool isEnd = true;
    bool isHold = false; // whether the task with higher precedence is in queue
    vector<int> exclude; // whose precedence is low
    vector<bool> more;   // whether task is periodic or sporadic
    int last = -1;       // Record the task at last second

    for (int i = 0; i < n; i++)
    {
        more.push_back(true);
        waitingTime.push_back(0);
        copyExec.push_back(t[i].execTime); // Store original execution time in another vector
        periods.push_back(t[i].period);    // Put periods of tasks in an array
        newRele.push_back(t[i].releTime);  // vector for updating release time of currentT task later
    }

    switch(algo)
    {
        case SJF:
        {
            cout << "--- SJF ---" << endl;
            break;
        }
        case EDF:
        {
            cout << "--- EDF ---" << endl;
            break;
        }
        case LSTF:
        {
            cout << "--- LSTF ---" << endl;
            break;
        }
    }
    for (int j = 1; j <= m; j++)
    {
        cout << "Processor " << j << ":" << endl;
        while (n > 0)
        {
            // Check release time, and push executable tasks to the waiting queue
            for (int i = 0; i < more.size(); i++)
            {             
                if (t[i].releTime <= currTime && more[i] == true)
                {
                    if (t[i].period == 0)
                    {
                        more[i] = false; // Sporadic task should not be put into queue again
                    }
                    else
                    {
                        memcpy(&newRele[i], &t[i].releTime, sizeof(int));    // Store release time of this job
                        t[i].releTime += periods[i];                         // Update release time for next job
                        memcpy(&t[i].deadline, &t[i].releTime, sizeof(int)); // Update deadline of this job
                    }
                    q.push_back(t[i]); // Push to waiting queue
                    total_jobs += 1;   // Update number of jobs
                }
            }
            // Compute slack time
            for (int k = 0; k < q.size(); k++)
            {
                q[k].slackTime = q[k].deadline - currTime - q[k].execTime;
            }
            if (isEnd == true || (isEnd == false && q[best].preempt == 1))
            {
                // Choose the task whose slack time is the least
                for (int k = 0; k < q.size(); k++)
                {
                    switch (algo)
                    {
                    case SJF:
                    {
                        if (q[k].execTime < q[best].execTime)
                        {
                            best = k;
                        }
                        else if (q[k].execTime == q[best].execTime && q[k].id < q[best].id) // If deadline are same, choose smallest id
                        {
                            best = k;
                        }
                        break;
                    }
                    case EDF:
                    {
                        if (q[k].deadline < q[best].deadline)
                        {
                            best = k;
                        }
                        else if (q[k].deadline == q[best].deadline && q[k].id < q[best].id) // If deadline are same, choose smallest id
                        {
                            best = k;
                        }
                        break;
                    }
                    case LSTF:
                    {
                        if (q[k].slackTime < q[best].slackTime)
                        {
                            best = k;
                        }
                        else if (q[k].slackTime == q[best].slackTime && q[k].id < q[best].id) // If deadline are same, choose smallest id
                        {
                            best = k;
                        }
                        break;
                    }
                    }
                    if (q[best].id != last)
                        isChange = true;
                }
            }
            // Check precedence
            do
            {
                isHold = false; // Reset
                for (int i = 0; i < q.size(); i++)
                {
                    // If task with higher precedence is still in waiting queue
                    if (find(q[best].preced.begin(), q[best].preced.end(), q[i].id) != q[best].preced.end())
                    {
                        isHold = true;
                        exclude.push_back(q[best].id);
                        isChange = false;
                        best = 0;
                        break;
                    }
                }
                if (isHold == true) // Find another task with least slack time
                {
                    for (int k = 0; k < q.size(); k++)
                    {
                        if (find(exclude.begin(), exclude.end(), q[k].id) == exclude.end()) // q[k] isn't the task with lower precedence that we just gave up
                        {
                            if (q[k].slackTime < q[best].slackTime)
                            {
                                best = k;
                                if (q[best].id != last)
                                {
                                    isChange = true;
                                }
                            }
                            else if (q[k].slackTime == q[best].slackTime && q[k].id < q[best].id) // If deadline are same, choose smallest id
                            {
                                best = k;
                                if (q[best].id != last)
                                    isChange = true;
                            }
                        }
                    }
                }
            } while (isHold); // While still find the task with higher precedence in queue
            if (q[best].id != last)
                isChange = true;

            if (!q.empty()) // There is a job executing
            {
                currTask = q[best].id;
                if (isChange == true)
                {
                    if (isEnd == false) // If the job is preempted
                        cout << currTime << endl;
                    cout << currTime << " Task" << currTask << " ";
                    isChange = false;
                    isEnd = false;
                }
            }

            --q[best].execTime;
            last = q[best].id; // Record the task at this second and go to next second
            ++currTime;        // Update time

            // Current task is finished
            if (q[best].execTime == 0)
            {
                isEnd = true;
                --n;
                if (currTime <= t[currTask].deadline)
                {
                    ++hit;
                }
                waitingTime[currTask] += currTime - newRele[currTask] - copyExec[currTask];
                cout << currTime << endl;
                memcpy(&t[currTask].execTime, &copyExec[currTask], sizeof(int)); // Restore original execution time
                q.erase(q.begin() + best);
                best = 0; // Reset
                isChange = true;
                last = -1;
            }
        }
        // Cauculate the average waiting time and hit rate
        double total = 0;
        for (int i = 0; i < waitingTime.size(); i++)
        {
            total += waitingTime[i];
        }
        double avg = total / waitingTime.size();
        double hitRate = hit / waitingTime.size();
        cout << "Average Waiting Time : " << fixed << setprecision(2) << avg << endl;
        cout << "Hit Rate : " << fixed << setprecision(2) << hitRate << endl;
    }
    cout << endl;
}