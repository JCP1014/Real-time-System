#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
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

int gcd(int a, int b);
int find_hyperPeriod(vector<int> &period, int n);

int main()
{
    int m = 0, n = 0;
    freopen("input.txt", "r", stdin);
    cin >> m >> n;
    struct processor p[m];
    struct task t[n];
    vector<struct task> q;
    int currTime = 0;
    int currTask = -1;
    int earliest = 0;
    vector<int> copyExec;
    vector<int> periods;
    vector<int> newRele; // For updating release time
    int hyperPeriod = 0;
    vector<int> waitingTime;
    double usedTime = 0;
    double avg_wait = 0;
    double cpu_util = 0;
    double total_jobs = 0;
    bool isChange = true;
    bool isEnd = true;

    for (int i = 0; i < m; i++)
    {
        cin >> p[i].id >> p[i].ability;
    }
    for (int i = 0; i < n; i++)
    {
        cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
        waitingTime.push_back(0);
    }

    /* Store original execution time in another vector */
    for (int i = 0; i < n; i++)
    {
        copyExec.push_back(t[i].execTime); // Store original execution time in another vector
        periods.push_back(t[i].period);    // Put periods of tasks in an array
        newRele.push_back(t[i].releTime);  // vector for updating release time of currentT task later
    }

    hyperPeriod = find_hyperPeriod(periods, n);

    for (int j = 1; j <= m; j++)
    {
        cout << "Processor " << j << ":" << endl;
        while (currTime < hyperPeriod)
        {
            /* Check release time, and push executable tasks to the waiting queue */
            for (int i = 0; i < n; i++)
            {
                if (t[i].releTime <= currTime)
                {
                    memcpy(&newRele[i], &t[i].releTime, sizeof(int));    // Store release time of this job
                    t[i].releTime += periods[i];                         // Update release time for next job
                    memcpy(&t[i].deadline, &t[i].releTime, sizeof(int)); // Update deadline of this job
                    q.push_back(t[i]);
                    total_jobs += 1; // Update number of jobs
                }
            }

            /* Choose the task whose dealine is the earliest */
            for (int k = 0; k < q.size(); k++)
            {
                if (q[k].deadline < q[earliest].deadline) 
                {
                    earliest = k;
                    isChange = true;
                }
                else if (q[k].deadline == q[earliest].deadline && q[k].id < q[earliest].id) // If deadline are same, choose smallest id
                {
                    earliest = k;
                    isChange = true;
                }
            }
            if (!q.empty()) // There is a job executing
            {
                currTask = q[earliest].id;
                if (isChange == true)
                {
                    if (isEnd == false) // If the job is preempted
                        cout << currTime << endl;
                    cout << currTime << " Task" << currTask << " ";
                    isChange = false;
                    isEnd = false;
                }
            }

            --q[earliest].execTime;
            ++currTime;
            if (!q.empty())
            {
                ++usedTime; // Increased time that CPU is executing tasks
            }

            /* Current task is finished */
            if (q[earliest].execTime == 0)
            {
                isEnd = true;
                waitingTime[currTask] += currTime - newRele[currTask] - copyExec[currTask];
                cout << currTime << endl;
                memcpy(&t[currTask].execTime, &copyExec[currTask], sizeof(int)); // Restore original execution time
                q.erase(q.begin() + earliest);
                earliest = 0; // Reset
                isChange = true;
            }
        }

        // Cauculate the average waiting time and CPU utilization
        double total_wait = 0;
        for (int i = 0; i < waitingTime.size(); i++)
        {
            total_wait += waitingTime[i];
        }
        avg_wait = total_wait / total_jobs;
        cpu_util = usedTime / hyperPeriod;
        cout << "Average Waiting Time : " << fixed << setprecision(2) << avg_wait << endl;
        cout << "CPU Utilization : " << fixed << setprecision(2) << cpu_util << endl;
    }

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
        hyper = period[i] * hyper / gcd(period[i], hyper);
    }
    return hyper;
}