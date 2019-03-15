#include<cstdio>
#include<iostream>
using namespace std;

struct processor
{
	int id;
	int ability;
};

struct task
{
	int id;
	int relTime;
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
	cout << "number of processors : " << m << endl;
	cout << "number of tasks : " << n << endl;
	struct processor p[m];
	struct task t[n];
	for(int i=0; i<m; i++)
	{
		cin >> p[i].id >> p[i].ability;
		cout << "Processor " << p[i].id << " : ability " << p[i].ability << endl;
	}
	for(int i=0; i<n; i++)
	{
		cin >> t[i].id >> t[i].relTime >> t[i].execTime >> t[i].deadline >> t[i].period >> t[i].preempt >> t[i].type;
		cout << "Task " << t[i].id << " : release time " << t[i].relTime << ", execution time " << t[i].execTime << ", deadline " << t[i].deadline << ", period " << t[i].period << ", preemption " << t[i].preempt << ", type " << t[i].type << endl;
	}

	return 0;
 
}
