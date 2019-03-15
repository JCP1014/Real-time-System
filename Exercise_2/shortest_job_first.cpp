#include<cstdio>
#include<iostream>
#include<queue>
#include<vector>
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
	int m=0,n=0;
	freopen("input.txt","r",stdin);
	cin >> m >> n;
	struct processor p[m];
	struct task t[n];
	vector<struct task>q;
	int currTime = 0;
	int currTask = -1;
	int taskTime = -1;
	vector<int> restTask;
	int shortest = 0;

	for(int i=0; i<m; i++)
	{
		cin >> p[i].id >> p[i].ability;
	}
	for(int i=0; i<n; i++)
	{
		cin >> t[i].id >> t[i].releTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
		restTask.push_back(i);
	}
	for(int j=1; j<=m; j++)
	{
		cout << "Processor " << j << ":" << endl;
		while(n>0)
		{
			for(int i=0; i<n; i++)
			{
				if(!restTask.empty())
				{
					if(t[restTask[i]].releTime <= currTime)
					{
						q.push_back(t[restTask[i]]);
						restTask.erase(restTask.begin()+i);
					}
				}
			}
			if(currTask < 0)
			{
				shortest = 0;
				for(int k=0; k<q.size();k++)
				{
					if(q[k].execTime < q[shortest].execTime )
					{
							shortest = k;
					}
				}
				currTask = q[shortest].id;
				taskTime = q[shortest].execTime;
				cout << currTime << " Task " << currTask << " ";
				q.erase(q.begin()+shortest);
						
			}

			--taskTime;
			++currTime;
			if(taskTime==0)
			{
				cout << currTime << endl;
				--n;
				currTask = -1;
			}

				
		}
	}

	return 0;
 
}

