/*
 * matching.cpp
 *
 *  Created on: Oct 10, 2018
 *      Author: yuankai
 */

# include "matching.hpp"

# include <cassert>
# include <cstdlib>
# include <vector>
# include <math.h>

# include "omp.h"

# include "boost/algorithm/string.hpp"

newickNode::newickNode(const std::string& tree, const std::set<std::string>& archLab,\
		const std::set<std::string>& afrLab) : label(""), dis(0), \
		nchild(0), child(NULL), parent(NULL), isArch(false), isAfr(false)
{
	std::string str = boost::algorithm::trim_copy(tree);
	if(str[str.size() - 1] == ';')
		str = str.substr(0, str.size() - 1);
	str += ":0";
	label = _load(str, NULL, archLab, afrLab);
}

newickNode::~newickNode()
{
	if(nchild)
		delete[] child;
}

//Go through all the node from up to bottom
void newickNode::traversal(newickFunc& func)
{
	func(*this);
	for(int i = 0 ; i < nchild; ++i)
		child[i].traversal(func);
}

void newickNode::search(newickDirFunc& func)
{
	if(func.check.count(this))
		return;
	func.check.insert(this);
	func(*this);
	bool curFirst(func.isFirst);
	if(func.isFirst)
		func.isFirst = false;
	if(parent)
	{
		func.preNode = this;
		func.upper = true;
		if(curFirst)
			func.upSearchStat = 1;
		parent->search(func);
	}
	if(nchild)
		for(int i = 0 ; i < nchild; ++i)
		{
			func.preNode = this;
			func.upper = false;
			if(curFirst)
				func.upSearchStat = -1;
			child[i].search(func);
		}
}

void newickNode::searchWithStop(newickDirRetFunc& func)
{
	if(func.check.count(this))
		return;
	func.check.insert(this);
	int ret = func(*this);
	if(ret == -1)
		return;
	bool curFirst(func.isFirst);
	if(func.isFirst)
		func.isFirst = false;
	if(parent)
	{
		func.preNode = this;
		func.upper = true;
		if(curFirst)
			func.upSearchStat = 1;
		parent->searchWithStop(func);
	}
	if(nchild)
		for(int i = 0 ; i < nchild; ++i)
		{
			func.preNode = this;
			func.upper = false;
			if(curFirst)
				func.upSearchStat = -1;
			child[i].searchWithStop(func);
		}
	return;
}

void labcheck::operator ()(newickNode& node)
{
	assert(access.count(node.getLab()) == 0);
	access[node.getLab()] = &node;
}

void getNodeLab::operator ()(newickNode& node)
{
	if(node.nchild == 0)
		leafLab.push_back(node.getLab());
	else if(node.parent != NULL)
		midLab.push_back(node.getLab());
	labels.push_back(node.getLab());
}

#ifdef DEBUG

void labprint::operator ()(newickNode& node)
{
	std::cout << node.getLab() << '\t' << node.getDis() << std::endl;
	if(node.parent != NULL)
		std::cout << "\tParent:\t" << node.parent->getLab() << std::endl;
	if(node.nchild != 0)
	{
		std::cout << "\tChild:";
		for(int i = 0 ; i < node.nchild; ++i)
			std::cout << '\t' << node.child[i].getLab() ;
		std::cout << std::endl;
	}
}

#endif//DEBUG

void disSearch::operator ()(newickNode& node)
{
	if(preNode)
	{
		if(upper)
			disMem[&node] = preNode->getDis() + disMem[preNode];
		else
			disMem[&node] = node.getDis() + disMem[preNode];
	}
	else
		disMem[&node] = 0;
	labDis[node.getLab()] = disMem[&node];
	isUpSearch[node.getLab()] = upSearchStat;
}

int closeSearch::operator ()(newickNode& node)
{
	double curDis;
	int n(-1);
	if(labSet1.count(node.label))
		n = 0;
	else if(labSet2.count(node.label))
		n = 1;
	if(n == -1)
		return 0;
	if(preNode)
	{
		if(upper)
			curDis = preNode->getDis() + disMem[preNode];
		else
			curDis = node.getDis() + disMem[preNode];
	}
	else
		curDis = 0;
	if(find[n])
	{
		if(min[n] < curDis)
			min[n] = curDis;
	}
	else
	{
		find[n] = true;
		min[n] = curDis;
	}
	return -1;
}

std::string newickNode::_load(const std::string& subTree, newickNode* upper, \
		const std::set<std::string>& archLab, const std::set<std::string>& afrLab)
{
	parent = upper;
	int p(subTree.size() - 1);
	while(p > 0 && subTree[p] != ':')
		--p;
	assert(p != 0);
	assert(p != (int)subTree.size() - 1);

	dis = atof(subTree.substr(p + 1).c_str());

	if(subTree[0] == '(')
	{
		assert(p >= 2);
		assert(subTree[p - 1] == ')');
		std::vector<std::string> sp;
		std::string t = subTree.substr(1, p - 2);
		int bracket(0);
		uint pre(0);
		for(uint i = 0 ; i < t.size(); ++i)
		{
			if(t[i] == '(')
				++bracket;
			else if(t[i] == ')')
				--bracket;
			else if(t[i] == ',' && bracket == 0)
			{
				sp.push_back(t.substr(pre, i - pre));
				pre = i + 1;
			}
		}
		sp.push_back(t.substr(pre));
		nchild = sp.size();
		child = new newickNode[nchild];
		label = child[0]._load(sp[0], this, archLab, afrLab);
		if(child[0].isAfr)
			isAfr = true;
		if(child[0].isArch)
			isArch = true;
		for(int i = 1 ; i < nchild ; ++i)
		{
			label += "_" + child[i]._load(sp[i], this, archLab, afrLab);
			if(child[i].isAfr)
				isAfr = true;
			if(child[i].isArch)
				isArch = true;
		}
	}
	else
	{
		label = subTree.substr(0, p);
		if(archLab.count(label))
			isArch = true;
		else if(afrLab.count(label))
			isAfr = true;
	}
	return label;
}

void msModel::msPhyArchaicSeek(const std::set<std::string>& archLab, \
		const std::set<std::string>& afrLab,\
		const std::vector<std::string>& checkLab, std::vector<int>& isArchaic)
{
	std::vector<double> minAfr, minArch;
	int ncheck = checkLab.size();

	for(int i = 0 ; i < ncheck ; ++i)
	{
		closeSearch d(archLab, afrLab);
		access.access[checkLab[i]]->searchWithStop(d);
		if(d.find[0] && d.find[1])
		{
			if(d.min[0] < d.min[1])
				isArchaic[i] = 1;
			else
				isArchaic[i] = 0;
		}
		else if(d.find[0])
			isArchaic[i] = 1;
		else if(d.find[1])
			isArchaic[i] = 0;
		else
		{
			std::cerr << "Err" << std::endl;
			return;
		}
	}
}

void fastSearch(const msModel& model, const std::vector<std::string>& checkLab, \
		std::vector<int>& isArchaic)
{
	int ncheck(checkLab.size());
	for(int i = 0 ; i < ncheck ; ++i)
	{
		const newickNode* curNode = model.access.access.at(checkLab[i]);
		const newickNode* root = &model.model;
		while(curNode != root)
		{
			if(!(curNode->isArch || curNode->isAfr))
			{
				curNode = curNode->parent;
				continue;
			}
			if(curNode->isArch && !curNode->isAfr)
				isArchaic[i] = 1;
			else
				isArchaic[i] = 0;
			break;
		}
		if(curNode == root)
			isArchaic[i] = 0;
	}
}

void archaicSeekerSimAnalysis(const std::string& msPath, const std::string& outPrefix, \
		const std::string& archaicLab, const std::string& africanLab, \
		const std::string& testLab)
{
	gzifstream fpms(msPath.c_str());
	std::string line;
	std::vector<std::string> sepTest, checkLab;
	std::set<std::string> archLab, afrLab;
	boost::split(sepTest, africanLab, boost::is_any_of(","), boost::token_compress_on);
	for(uint i = 0 ; i < sepTest.size(); ++i)
	{
		uint n(0);
		for(; n < sepTest[i].size(); ++n)
			if(sepTest[i][n] == '-')
				break;
		if(n == sepTest[i].size())
			afrLab.insert(sepTest[i]);
		else
		{
			int start = atoi(sepTest[i].substr(0, n).c_str());
			int end = atoi(sepTest[i].substr(n + 1).c_str());
			for(int j = start ; j <= end; ++j)
			{
				std::ostringstream num;
				num << j;
				afrLab.insert(num.str());
			}
		}
	}

	sepTest.clear();
	boost::split(sepTest, archaicLab, boost::is_any_of(","), boost::token_compress_on);
	for(uint i = 0 ; i < sepTest.size(); ++i)
	{
		uint n(0);
		for(; n < sepTest[i].size(); ++n)
			if(sepTest[i][n] == '-')
				break;
		if(n == sepTest[i].size())
			archLab.insert(sepTest[i]);
		else
		{
			int start = atoi(sepTest[i].substr(0, n).c_str());
			int end = atoi(sepTest[i].substr(n + 1).c_str());
			for(int j = start ; j <= end; ++j)
			{
				std::ostringstream num;
				num << j;
				archLab.insert(num.str());
			}
		}
	}

	sepTest.clear();
	boost::split(sepTest, testLab, boost::is_any_of(","), boost::token_compress_on);
	for(uint i = 0 ; i < sepTest.size(); ++i)
	{
		uint n(0);
		for(; n < sepTest[i].size(); ++n)
			if(sepTest[i][n] == '-')
				break;
		if(n == sepTest[i].size())
			checkLab.push_back(sepTest[i]);
		else
		{
			int start = atoi(sepTest[i].substr(0, n).c_str());
			int end = atoi(sepTest[i].substr(n + 1).c_str());
			for(int j = start ; j <= end; ++j)
			{
				std::ostringstream num;
				num << j;
				checkLab.push_back(num.str());
			}
		}
	}
	int nind = checkLab.size();
	int count(1);
	while(getline(fpms, line))
	{
		if(line == "//")
		{
			std::vector<int> nmks;
			std::vector<std::vector<int> > archaic;
			std::vector<std::string> infos;
			while(getline(fpms, line))
			{
				if(line[0] == 's')
					break;
				infos.push_back(line);
			}
			int ninfo = infos.size();
			archaic.resize(ninfo);
			for(int i = 0 ; i < ninfo; ++i)
				archaic[i].resize(nind, 0);
			nmks.resize(ninfo, 0);
#pragma omp parallel for
			for(int i = 0 ; i < ninfo; ++i)
			{
				const std::string& l = infos[i];
				uint p(1);
				while(p < l.size() && l[p] != ']')
					++p;
				assert(p < l.size());
				nmks[i] = atoi(l.substr(1, p - 1).c_str());
				msModel tree(l.substr(p + 1), archLab, afrLab);
//				tree.msPhyArchaicSeek(archLab, afrLab, checkLab, archaic[i]);
				fastSearch(tree, checkLab, archaic[i]);
			}
			infos.clear();
			std::ostringstream path;
			path << outPrefix << "_" << count << ".seg";
			++count;
			std::ofstream fpo(path.str().c_str());
			fpo << "ind\tstart\tend" << std::endl;
			for(int i = 0 ; i < nind; ++i)
			{
				int pos(0), pre(0);
				bool isarchaic(false);
				if(archaic.front()[i])
					isarchaic = true;
				for(uint j = 0 ; j < archaic.size(); ++j)
				{
					if(archaic[j][i] && !isarchaic)
					{
						pre = pos;
						isarchaic = true;
					}
					else if(!archaic[j][i] && isarchaic)
					{
						fpo << "ind-" << checkLab[i] << '\t' << pre << '\t' << pos \
								<< std::endl;
						isarchaic = false;
					}
					pos += nmks[j];
				}
				if(isarchaic)
					fpo << "ind-" << checkLab[i] << '\t' << pre << '\t' << pos << std::endl;
			}
		}
	}
}





