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
	int taskTime = -1;
	vector<bool> isReleased;
	int shortest = 0;
	vector<int> copyExec;
	vector<int> periods;
	vector<int> newRele; // For updating release time
	int hyperPeriod = 0;
	vector<int> waitingTime;
	double usedTime = 0;
	double avg_wait = 0;
	double cpu_util = 0;

	for (int i = 0; i < m; i++)
	{
		cin >> p[i].id >> p[i].ability;
	}
	for (int i = 0; i < n; i++)
	{
		cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
		isReleased.push_back(false);
		waitingTime.push_back(0);
	}
	/* Store original execution time in another vector */
	for (int i = 0; i < n; i++)
	{
		copyExec.push_back(t[i].execTime); // Store original execution time in another vector
		periods.push_back(t[i].period);	// Put periods of tasks in an array
		newRele.push_back(t[i].releTime);
	}

	hyperPeriod = find_hyperPeriod(periods, n);

	for (int j = 1; j <= m; j++)
	{
		cout << "Processor " << j << ":" << endl;
		while (currTime < hyperPeriod)
		{
			for (int i = 0; i < isReleased.size(); i++)
			{
				if (isReleased[i] == false && t[i].releTime <= currTime)
				{
					//cout << "i = " << i << "rele = " << t[i].releTime << endl;
					q.push_back(t[i]);
					isReleased[i] = true;
				}
			}
			if (currTask < 0)
			{
				shortest = 0;
				for (int k = 0; k < q.size(); k++)
				{
					if (q[k].execTime < q[shortest].execTime)
					{
						shortest = k;
					}
				}
				if (!q.empty())
				{
					currTask = q[shortest].id;
					taskTime = q[shortest].execTime;
					cout << currTime << " Task" << currTask << " ";
					q.erase(q.begin() + shortest);
				}
			}
			--taskTime;
			++currTime;
			if (currTask > -1)
			{
				++usedTime;
			}

			if (taskTime == 0)
			{
				waitingTime[currTask] += currTime - t[currTask].releTime - copyExec[currTask];
				cout << currTime << endl;
				newRele[currTask] += periods[currTask];
				memcpy(&t[currTask].releTime, &newRele[currTask], sizeof(int));
				memcpy(&t[currTask].execTime, &copyExec[currTask], sizeof(int));
				isReleased[currTask] = false;
				currTask = -1;
			}
		}
		// Cauculate the average waiting time and CPU utilization
		double total = 0;
		for (int i = 0; i < waitingTime.size(); i++)
		{
			total += waitingTime[i];
		}
		avg_wait = total / waitingTime.size();
		cpu_util = usedTime / hyperPeriod;
		cout << "Average Waiting Time : " << fixed << setprecision(2) << avg_wait << endl;
		cout << "CPU Utilization : " << fixed << setprecision(2) << cpu_util << endl;
	}

	return 0;
}

int gcd(int a, int b)
{
	if (b == 0)
		return a;
	return gcd(b, a % b);
}
int find_hyperPeriod(vector<int> &period, int n)
{
	int hyper = period[0];
	for (int i = 1; i < n; i++)
	{
		hyper = period[i] * hyper / gcd(period[i], hyper);
	}
	return hyper;
}