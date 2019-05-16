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
int compute_frame(int n, struct task t[], int hyperPeriod);
void scheduling(int m, int n, struct processor p[], struct task t[], int hyperPeriod, int frame);

int main()
{
    FILE *fp;
    int m = 0, n = 0, c = 0;
    fp = freopen("input.txt", "r", stdin);
    cin >> m >> n;
    struct processor p[m];
    struct task t[n];
    vector<int> periods;
    int hyperPeriod = 0;
    int frame = 0;

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
    hyperPeriod = find_hyperPeriod(periods, n);

    frame = compute_frame(n, t, hyperPeriod);
    scheduling(m, n, p, t, hyperPeriod, frame);

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

int compute_frame(int n, struct task t[], int hyperPeriod)
{
    int maxExec = t[0].execTime;
    for (int i = 1; i < n; i++)
    {
        maxExec = max(maxExec, t[i].execTime);
    }

    vector<int> possible;
    for (int i = 2; i < (hyperPeriod / 2); i++)
    {
        if (hyperPeriod % i == 0 && i>=maxExec)
            possible.push_back(i);
    }

    for (int i = 0; i < possible.size(); i++)
    {
        bool check = true;
        for (int j = 0; j < n; j++)
        {
            if (t[j].period > 0)
            {
                int f = possible[i];
                if ((2 * f - gcd(t[j].period, f)) > t[j].period)
                {
                    check = false;
                    break;
                }
            }
        }
        if (check == true)
        {
            return possible[i];
        }
    }
}

void scheduling(int m, int n, struct processor p[], struct task t[], int hyperPeriod, int frame)
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
    int table[hyperPeriod][3];
    int frame_rest = frame;
    int forward = frame;
    vector<int> aperiodic;
    vector<struct task> aq;

    for (int j = 1; j <= m; j++)
    {
        cout << "Processor " << j << ":" << endl;

        for (int i = 0; i < hyperPeriod; i++)
        {
            table[i][0] = i;
            table[i][1] = -1;
            table[i][2] = (i + 1);
        }

        for (int i = 0; i < n; i++)
        {
            more.push_back(true);
            waitingTime.push_back(0);
            copyExec.push_back(t[i].execTime); // Store original execution time in another vector
            periods.push_back(t[i].period);    // Put periods of tasks in an array
            newRele.push_back(t[i].releTime);  // vector for updating release time of currentT task later
            if (t[i].period == 0)
            {
                aperiodic.push_back(i);
            }
        }

        while (forward <= hyperPeriod)
        {
            frame_rest = frame;
            currTime = forward - frame;

            while (frame_rest > 0)
            {
                // Check release time, and push executable tasks to the waiting queue
                for (int i = 0; i < more.size(); i++)
                {
                    if (t[i].releTime <= currTime && more[i] == true && t[i].period > 0)
                    {
                        memcpy(&newRele[i], &t[i].releTime, sizeof(int));    // Store release time of this job
                        t[i].releTime += periods[i];                         // Update release time for next job
                        memcpy(&t[i].deadline, &t[i].releTime, sizeof(int)); // Update deadline of this job
                        q.push_back(t[i]);                                   // Push to waiting queue
                        total_jobs += 1;                                     // Update number of jobs
                    }
                }

                if (!q.empty())
                {
                    if (isEnd == true || (isEnd == false && q[best].preempt == 1))
                    {
                        // Choose the task whose slack time is the least
                        for (int k = 0; k < q.size(); k++)
                        {
                            if (q[k].deadline < q[best].deadline)
                            {
                                best = k;
                            }
                            else if (q[k].deadline == q[best].deadline && q[k].id < q[best].id) // If deadline are same, choose smallest id
                            {
                                best = k;
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
                                    if (q[k].deadline < q[best].deadline)
                                    {
                                        best = k;
                                        if (q[best].id != last)
                                        {
                                            isChange = true;
                                        }
                                    }
                                    else if (q[k].deadline == q[best].deadline && q[k].id < q[best].id) // If deadline are same, choose smallest id
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

                    //if (!q.empty()) // There is a job executing
                    //{
                    if (q[best].execTime <= frame_rest && q[best].period != 0)
                    {
                        currTask = q[best].id;
                        table[currTime][1] = currTask;
                        isChange = false;
                        isEnd = false;
                        // }

                        --q[best].execTime;
                        --frame_rest;
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
                            memcpy(&t[currTask].execTime, &copyExec[currTask], sizeof(int)); // Restore original execution time
                            q.erase(q.begin() + best);
                            best = 0; // Reset
                            isChange = true;
                            last = -1;
                        }
                    }
                    else
                    {
                        ++currTime;
                        --frame_rest;
                    }
                }
                else
                {
                    ++currTime;
                    --frame_rest;
                }
            }

            forward += frame;
        }

        // Scheduling for aperiodic tasks 
        currTime = 0;
        while (currTime < hyperPeriod && aperiodic.empty() == false)
        {
            if (table[currTime][1] == -1)
            {
                bool ready = false;
                int index = 0;
                best = aperiodic.at(index);
                if (t[best].releTime <= currTime)
                    ready = true;

                for (int j = 0; j < aperiodic.size(); j++)
                {
                    if (t[aperiodic[j]].releTime <= currTime && t[aperiodic[j]].deadline < t[best].deadline)
                    {
                        best = aperiodic[j];
                        ready = true;
                        index = j;
                    }
                }
                if (ready == true)
                {
                    for (int k = currTime; k < (currTime + t[best].execTime); k++)
                        table[k][1] = best;
                    currTime += t[best].execTime;
                    waitingTime[best] = currTime - t[best].releTime - t[best].execTime;
                    ++total_jobs;
                    aperiodic.erase(aperiodic.begin() + index);
                }
                else
                {
                    ++currTime;
                }
            }
            else
            {
                ++currTime;
            }
        }

        // Print the result of scheduling
        double usedTime = 0;
        bool keep = false;
        for (int i = 0; i < hyperPeriod; i++)
        {
            if(table[i][1]>-1)
            {
                ++usedTime;
                if(table[i][1]!=table[i+1][1])
                {    
                    if(keep==true)
                    {    
                        cout << table[i][2] << endl;
                        keep = false;
                    }
                    else
                        cout << table[i][0] << " Task" << table[i][1] << " " << table[i][2] << endl;
                }
                else
                {
                    if(keep==false)
                    {   cout << table[i][0] << " Task" << table[i][1] << " ";
                        keep = true;
                    }
                }
            }
        }
        
        // Cauculate the average waiting time and CPU utilization
        double total_wait = 0;
        for (int i = 0; i < waitingTime.size(); i++)
        {
            total_wait += waitingTime[i];
        }
        double avg = total_wait / total_jobs;
        double cpu_util = usedTime / hyperPeriod;
        cout << "Average Waiting Time : " << fixed << setprecision(2) << avg << endl;
        cout << "CPU utilization : " << fixed << setprecision(2) << cpu_util << endl;
    }
}
