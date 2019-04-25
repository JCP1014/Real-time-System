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
    int preced;
};

int gcd(int a, int b);
int find_hyperPeriod(vector<int> &period, int n);

int main()
{
    int m = 0, n = 0, c = 0;
    freopen("input.txt", "r", stdin);
    cin >> m >> n;
    struct processor p[m];
    struct task t[n];
    vector<struct task> q;
    int currTime = 0;
    int currTask = -1;
    int least = 0;
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
    bool isHold = false;    // whether the task with higher precedence is in queue
    vector<int> exclude;    // whose precedence is low
    vector<bool> more;  // whether task is periodic or sporadic
    int last = -1;  // Record the task at last second

    for (int i = 0; i < m; i++)
    {
        cin >> p[i].id >> p[i].ability;
    }
    for (int i = 0; i < n; i++)
    {
        cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
        // Initialize
        t[i].preced = -1;
        more.push_back(true);   
        waitingTime.push_back(0);
    }
    cin >> c;   // how many rules of precedence
    for (int i = 0; i < c; i++)
    {
        int high;   // task with higher precedence
        int low;    // task with lower precedence
        cin >> high >> low;
        t[low].preced = high;
    }

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
            // Check release time, and push executable tasks to the waiting queue
            for (int i = 0; i < n; i++)
            {
                if (t[i].releTime <= currTime && more[i] == true)
                {
                    if (t[i].period == 0)
                    {
                        more[i] = false;    // Sporadic task should not be put into queue again
                    }
                    else
                    {
                        memcpy(&newRele[i], &t[i].releTime, sizeof(int));    // Store release time of this job
                        t[i].releTime += periods[i];                         // Update release time for next job
                        memcpy(&t[i].deadline, &t[i].releTime, sizeof(int)); // Update deadline of this job
                    }
                    q.push_back(t[i]);  // Push to waiting queue
                    total_jobs += 1; // Update number of jobs
                }
            }
            // Compute slack time
            for (int k = 0; k < q.size(); k++)
            {
                q[k].slackTime = q[k].deadline - currTime - q[k].execTime;
            }
            // Choose the task whose slack time is the least
            for (int k = 0; k < q.size(); k++)
            {
                if (q[k].slackTime < q[least].slackTime) 
                {
                    least = k;
                    isChange = true;
                }
                else if (q[k].slackTime == q[least].slackTime && q[k].id < q[least].id) // If deadline are same, choose smallest id
                {
                    least = k;
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
                    if (q[i].id == q[least].preced)
                    {
                        isHold = true;
                        exclude.push_back(q[least].id);
                        isChange = false;
                        least = 0;
                        break;
                    }
                }
                if (isHold == true) // Find another task with least slack time
                {
                    for (int k = 0; k < q.size(); k++)
                    {
                        if (find(exclude.begin(), exclude.end(), q[k].id) == exclude.end()) // q[k] isn't the task with lower precedence that we just gave up
                        {
                            if (q[k].slackTime < q[least].slackTime)
                            {
                                least = k;
                                if (q[least].id != last)
                                    isChange = true;
                            }
                            else if (q[k].slackTime == q[least].slackTime && q[k].id < q[least].id) // If deadline are same, choose smallest id
                            {
                                least = k;
                                if (q[least].id != last)
                                    isChange = true;
                            }
                        }
                    }
                }
            } while (isHold);   // While still find the task with higher precedence in queue

            if (!q.empty()) // There is a job executing
            {
                currTask = q[least].id;
                if (isChange == true)
                {
                    if (isEnd == false) // If the job is preempted
                        cout << currTime << endl;
                    cout << currTime << " Task" << currTask << " ";
                    isChange = false;
                    isEnd = false;
                }
            }

            --q[least].execTime;
            last = q[least].id; // Record the task at this second and go to next second
            ++currTime; // Update time
            if (!q.empty())
            {
                ++usedTime; // Increased time that CPU is executing tasks
            }

            // Current task is finished
            if (q[least].execTime == 0)
            {
                isEnd = true;
                waitingTime[currTask] += currTime - newRele[currTask] - copyExec[currTask];
                cout << currTime << endl;
                memcpy(&t[currTask].execTime, &copyExec[currTask], sizeof(int)); // Restore original execution time
                q.erase(q.begin() + least);
                least = 0; // Reset
                isChange = true;
                last = -1;
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
        if (period[i] > 0)
            hyper = period[i] * hyper / gcd(period[i], hyper);
    }
    return hyper;
}