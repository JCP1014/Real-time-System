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
	vector<struct task> q;
	int currTime = 0;
	int currTask = -1;
	int taskTime = -1;
	vector<bool> isReleased;
	int shortest = 0;
	vector<int> waitingTime;
	double hit = 0;		  // number of tasks whose finish time <= deadline
	vector<int> copyExec; // store the original execution time
	vector<int> age;
	int threshold = 10;
	bool isOld = false;

	for (int i = 0; i < m; i++)
	{
		cin >> p[i].id >> p[i].ability;
		//cout << "Processor " << p[i].id << " : ability " << p[i].ability << endl;
	}
	for (int i = 0; i < n; i++)
	{
		cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
		cout << "Task " << t[i].id << " : release time " << t[i].releTime << ", execution time " << t[i].execTime << ", deadline " << t[i].deadline << ", period " << t[i].period << ", preemption " << t[i].preempt << ", type " << t[i].type << endl;
		isReleased.push_back(false);
		waitingTime.push_back(0);
		age.push_back(0);
	}

	// Store original execution time in another vector
	for (int i = 0; i < n; i++)
	{
		copyExec.push_back(t[i].execTime);
	}

	for (int j = 1; j <= m; j++)
	{
		cout << "Processor " << j << ":" << endl;
		while (n > 0)
		{
			for (int i = 0; i < isReleased.size(); i++)
			{
				if (isReleased[i] == false && t[i].releTime <= currTime)
				{
					q.push_back(t[i]);
					isReleased[i] = true;
				}
			}
			if (currTask < 0)
			{
				for (int k = 0; k < q.size(); k++)
				{
					if (age[q[k].id] >= threshold)
					{
						currTask = q[k].id;
						taskTime = q[k].execTime;
						cout << currTime << " Task" << currTask << " ";
						q.erase(q.begin() + k);
						isOld = true;
					}
				}
				if (isOld == false)
				{
					shortest = 0;
					//cout << "qsize = " << q.size() << endl;
					for (int k = 0; k < q.size(); k++)
					{
						if (q[k].execTime < q[shortest].execTime)
						{
							shortest = k;
						}
					}
					currTask = q[shortest].id;
					taskTime = q[shortest].execTime;
					cout << currTime << " Task" << currTask << " ";
					q.erase(q.begin() + shortest);
				}
				isOld = false;
			}

			--taskTime;
			++currTime;
			for (int i = 0; i < q.size(); i++)
			{
				age[q[i].id]++;
			}

			if (taskTime == 0)
			{
				waitingTime[currTask] = currTime - t[currTask].releTime - copyExec[currTask];
                if (currTime <= t[currTask].deadline)
                {
                    ++hit;
                }
				cout << currTime << endl;
				--n;
				currTask = -1;
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
        cout << "Average Waiting Time : " << avg << endl;
        cout << "Hit Rate : " << hitRate << endl;
	}

	return 0;
}
