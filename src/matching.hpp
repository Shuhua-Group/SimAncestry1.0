/*
 * matching.hpp
 *
 *  Created on: Oct 10, 2018
 *      Author: yuankai
 */

#ifndef MATCHING_HPP_
#define MATCHING_HPP_

# include <iostream>
# include <map>
# include <vector>
# include <set>
# include <math.h>
# include <sstream>
# include <fstream>

# include "gzfstream.hpp"

class matchModel;

class newickFunc;

class newickDirFunc;

class newickDirRetFunc;

class newickNode
{
public:
	newickNode() : label(""), dis(0), nchild(0), child(NULL), parent(NULL), \
			isArch(false), isAfr(false) {};
	newickNode(const std::string& tree, const std::set<std::string>& archLab, \
			const std::set<std::string>& arfLab);
	~newickNode();

	std::string getLab() const { return label; }
	double getDis() const { return dis; }

	void traversal(newickFunc& func);
	void search(newickDirFunc& func);
	//if func() return -1, stop search
	void searchWithStop(newickDirRetFunc& func);

	std::string _load(const std::string &subTree, newickNode* upper, \
			const std::set<std::string>& archLab, const std::set<std::string>& afrLab);

	std::string label;
	double dis;
	int nchild;
	newickNode *child, *parent;
	bool isArch, isAfr;
};

class newickFunc
{
public:
	virtual ~newickFunc() {};
	virtual void operator() (newickNode& node) = 0;
};

class newickDirFunc
{
public:
	newickDirFunc() : preNode(NULL), upper(true), isFirst(true), upSearchStat(-1) {};
	virtual ~newickDirFunc() {};
	virtual void operator() (newickNode& node) = 0;

	newickNode* preNode;
	bool upper;

	bool isFirst;
	int upSearchStat; // 1 up search, -1 down search, -1 cur
	std::set<newickNode*> check;
};

class newickDirRetFunc
{
public:
	newickDirRetFunc() : preNode(NULL), upper(true), isFirst(true), upSearchStat(-1) {};
	virtual ~newickDirRetFunc() {};
	virtual int operator() (newickNode& node) = 0;

	newickNode* preNode;
	bool upper;

	bool isFirst;
	int upSearchStat; // 1 up search, -1 down search, -1 cur
	std::set<newickNode*> check;
};

class labcheck : public newickFunc
{
public:
	void operator() (newickNode& node);
	std::map<std::string, newickNode*> access;
};

class getNodeLab : public newickFunc
{
public:
	getNodeLab(std::vector<std::string>& _labels, std::vector<std::string>& _leafLab, \
			std::vector<std::string>& _midLab) : labels(_labels), leafLab(_leafLab), midLab(_midLab) {}
	void operator() (newickNode& node);
	std::vector<std::string>& labels, & leafLab, & midLab;
};

#ifdef DEBUG

class labprint : public newickFunc
{
public:
	void operator() (newickNode& node);
};

#endif//DEBUG

class disSearch : public newickDirFunc
{
public:
	disSearch() : newickDirFunc() {};
	void operator() (newickNode& node);
	std::map<newickNode*, double> disMem;
	std::map<std::string, double> labDis;
	std::map<std::string, int> isUpSearch; // 1 up search, -1 down search, -1 cur
};

class closeSearch : public newickDirRetFunc
{
public:
	closeSearch(const std::set<std::string>& _labSet1, \
			const std::set<std::string>& _labSet2) : newickDirRetFunc(), \
			labSet1(_labSet1), labSet2(_labSet2)
			{ memset(find, 0, sizeof(bool) * 2); memset(min, 0, sizeof(double) * 2); };
	int operator() (newickNode& node);
	std::map<newickNode*, double> disMem;

	const std::set<std::string>& labSet1, & labSet2;
	bool find[2];
	double min[2];
};

class msModel
{
public:
	msModel(const std::string &m, const std::set<std::string>& archLab, \
			const std::set<std::string>& afrLab) : model(m, archLab, afrLab), \
			access() {model.traversal(access);}
	void msPhyArchaicSeek(const std::set<std::string>& archLab, \
			const std::set<std::string>& afrLab, \
			const std::vector<std::string>& checkLab, std::vector<int>& isArchaic);
	newickNode model;
	labcheck access;
};

void fastSearch(const msModel& model, const std::vector<std::string>& checkLab, \
		std::vector<int>& isArchaic);

void archaicSeekerSimAnalysis(const std::string& msPath, const std::string& outPrefix, \
		const std::string& archaicLab, const std::string& africanLab, \
		const std::string& testLab);


#endif /* MATCHING_HPP_ */
