#include <fstream>
#include <queue>

using namespace std;

typedef long long int LLONG;

struct element {
	int v;
	int w;
	LLONG len;
	double surv;
	double maxSurv;

	element(int a, int j, LLONG l, double s) : v(a), w(j), len(l), surv(s), maxSurv(-DBL_MAX) {}

};

double P(int, LLONG, int, vector<vector<element*>>&, vector<int>&);
void collectBranches(int, vector<vector<element*>>&, vector<element*>&, vector<int>&);

int S = 0;
int T = -1;
bool collectFathers = false;


int main()
{
	ifstream fin("in.txt");
	ofstream fout("out.txt");

	int n, s, t;
	LLONG L;
	fin >> n;
	fin >> s >> t;
	fin >> L;

	if (s == t)
	{
		fout << "Trivial Route" << endl;
		return 0;
	}

	S = s;
	T = t;
	int v, w;
	LLONG l;
	double p;
	vector<vector<element*>> lists(n);
	vector<vector<element*>> preds(n);
	vector<element*> fathers;
	vector<int> fatherInds(n);
	bool *reachableFromS = new bool[n];
	bool *predOfT = new bool[n];

	for (int i = 0; i < n; ++i)
	{
		reachableFromS[i] = predOfT[i] = false;
		fatherInds.at(i) = -1;
	}


	while (!fin.eof())
	{
		fin >> v >> w >> l >> p;
		element* el = new element(v, w, l, p);
		lists.at(v).push_back(el);
		preds.at(w).push_back(el);
	}

	queue<int> bfsQ;
	reachableFromS[s] = true;
	bfsQ.push(s);
	while (!bfsQ.empty())
	{
		v = bfsQ.front();
		bfsQ.pop();
		if (v == t)
			continue;
		for (int i = 0; i < lists.at(v).size(); ++i)
		{
			w = lists.at(v).at(i)->w;
			if (!reachableFromS[w])
			{
				bfsQ.push(w);
				reachableFromS[w] = true;
			}
		}

	}




	predOfT[t] = true;
	bfsQ.push(t);
	while (!bfsQ.empty())
	{
		v = bfsQ.front();
		bfsQ.pop();
		if (v == s)
			continue;
		for (int i = 0; i < preds.at(v).size(); ++i)
		{
			w = preds.at(v).at(i)->v;
			if (!predOfT[w])
			{
				bfsQ.push(w);
				predOfT[w] = true;
			}
		}
	}




	for (int i = n - 1; i >= 0; --i)
	{
		if (!(predOfT[i] && reachableFromS[i]))
		{
			lists.erase(lists.begin() + i);
		//	preds.erase(preds.begin() + i);
		}

	}

	LLONG lmin = LLONG_MAX;
	for (int i = 0; i < preds.size(); ++i)
	{
		int size = preds.at(i).size();
		for (int j = size - 1; j >= 0; j--)
		{
			if (preds.at(i).at(j)->len < lmin)
				lmin = preds.at(i).at(j)->len;
			if (!(predOfT[preds.at(i).at(j)->v] && reachableFromS[preds.at(i).at(j)->v]))
				preds.at(i).erase(preds.at(i).begin() + j);
		}
	}

	double F1MAX = -DBL_MAX;
	double F1VAL = -DBL_MAX;
	std::pair<int, LLONG> vals = make_pair(-1, -1);

	for (int i = 1; i < n; ++i)
	{
		for (LLONG j = lmin; j <= L; ++j)
		{
			F1VAL = P(i, j, t, preds, fatherInds);
			if (F1VAL > F1MAX)
			{
				F1MAX = F1VAL;
				vals.first = i;
				vals.second = j;
			}
		}
	}

	collectFathers = true;
	P(vals.first, vals.second, t, preds, fatherInds);

	collectBranches(t, preds, fathers, fatherInds);
	if (fathers.size() == 0 || F1MAX == 0)
	{
		fout << "No way!!\n";
		return 0;
	}
	fout << F1MAX << '\n' << vals.second << '\n';
	
	fout << fathers.at(fathers.size() - 1)->v << " -> (" << fathers.at(fathers.size() - 1)->len << ", " << fathers.at(fathers.size() - 1)->surv << ") " << fathers.at(fathers.size() - 1)->w;
	for (int i = fathers.size() - 2; i >= 0; --i)
	{
		fout << " -> (" << fathers.at(i)->len << ", " << fathers.at(i)->surv << ") " << fathers.at(i)->w;
	}
	fout << '\n';

	return 0;
}


double P(int v, LLONG l, int j, vector<vector<element*>>& preds, vector<int>& fatherInds)
{
	if (v == 0 && l == 0 && j == S)
		return 1;
	if (v == 0)
		return 0;

	double max = -DBL_MAX;
	double cur = -DBL_MAX;
	bool change = false;
	int fatherArc = -1;
	for (int i = 0; i < preds.at(j).size(); ++i)
	{
		if (preds.at(j).at(i)->len > l)
			continue;
		cur = preds.at(j).at(i)->surv * P(v-1, l-preds.at(j).at(i)->len, preds.at(j).at(i)->v, preds, fatherInds);
		if (cur > max)
		{
			max = cur;
			fatherArc = i;
		}
	}

	if (collectFathers && fatherArc != -1)
	{
		if (fatherInds.at(j) == -1)
		{
			fatherInds.at(j) = fatherArc;
			preds.at(j).at(fatherArc)->maxSurv = max;
		}
		else if (preds.at(j).at(fatherInds.at(j))->maxSurv < max)
		{
			fatherInds.at(j) = fatherArc;
			preds.at(j).at(fatherArc)->maxSurv = max;
		}
	}

	return max;

}


void collectBranches(int t, vector<vector<element*>>& preds, vector<element*>& fathers, vector<int>& fatherInds)
{
	if (fatherInds.at(t) != -1)
	{
		fathers.push_back(preds.at(t).at(fatherInds.at(t)));
		collectBranches(preds.at(t).at(fatherInds.at(t))->v, preds, fathers, fatherInds);
	}
}