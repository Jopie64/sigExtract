// sigExtract.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CmdLine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "JStd.h"
using namespace std;
using namespace JStd;

typedef ostream::pos_type pos_type;
typedef ostream::off_type off_type;


struct Signature
{
	Signature() :next(0), maxBytes(0) {}
	string header;
	string footer;
	size_t maxBytes;
	string ext;

	pos_type next;
};

void HexToString(string& s)
{
	istringstream ss(s);
	s.clear();
	string nr;
	while (getline(ss, nr, ' '))
	{
		char* dummy;
		s += (char)strtol(nr.c_str(), &dummy, 16);
	}
}

void ReadSignature(vector<Signature>& dest, const wstring& src)
{
	ifstream file(src);
	string line;
	while (getline(file, line))
	{
		istringstream sLine(line);
		Signature sig;
		string type;
		getline(sLine, type, '\t');
		int iType;
		if      (type == "txt") iType = 1;
		else if (type == "hex") iType = 2;
		else continue;
		getline(sLine, sig.header, '\t');
		getline(sLine, sig.footer, '\t');
		if (iType == 2)
		{
			HexToString(sig.header);
			HexToString(sig.footer);
		}
		string maxSize;
		getline(sLine, maxSize, '\t');
		sig.maxBytes = atol(maxSize.c_str());
		getline(sLine, sig.ext, '\t');
		if (sig.header.empty() || sig.footer.empty())
			continue;
		dest.push_back(sig);
	}
}

pos_type findInStream(istream& s, const std::string& str)
{
	static string readBuf;
	readBuf.reserve(65535);
	while (true)
	{
		readBuf.resize(readBuf.capacity());
		pos_type currPos = s.tellg();
		s.read(&readBuf[0], readBuf.size());
		s.clear();
		std::streamsize readCnt = s.gcount();
		if (readCnt == 0)
			return -1;
		readBuf.resize(readCnt);
		string::size_type found = readBuf.find(str);
		if (found != string::npos)
			return currPos + (pos_type)found;

		s.seekg(s.tellg() - (pos_type)str.size());
	}
	return -1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CmdLine::CmdLine cmdLine(argc, argv);
		wstring sigFileName	= cmdLine.next(L's', L"signature");
		wstring outDir		= cmdLine.next(L'o', L"output");
		wstring inFileName	= cmdLine.next(L'i', L"input");

		if (sigFileName.empty()) throw std::runtime_error("Missing signature file name (supply with -s)");
		if (inFileName.empty()) throw std::runtime_error("Missing input file name (supply with -i)");
		if (outDir.empty()) throw std::runtime_error("Missing output directory (supply with -o)");

		vector<Signature> vSig;
		ReadSignature(vSig, sigFileName);

		ifstream in(inFileName, ios::binary | ios::in);
		pos_type currPos = 0;
		while (true)
		{
			off_type lowestPos = -1;
			Signature* useSig = nullptr;
			for (auto &i : vSig)
			{
				if (i.next == (pos_type) -1)
					continue; //Not found anymore... skip it.
				if (!i.next)
				{
					in.seekg(currPos);
					i.next = findInStream(in, i.header);
				}
				if (i.next == (pos_type) -1)
					continue; //Not found anymore... skip it.
				if (lowestPos < 0 || i.next < lowestPos)
				{
					lowestPos = i.next;
					useSig = &i;
				}
			}
			if (lowestPos < 0 || !useSig)
				break; //All done.
			in.seekg(lowestPos + (pos_type)useSig->header.size());
			pos_type footerPos = findInStream(in, useSig->footer);
			size_t outFileSize = footerPos >= 0 ? size_t(footerPos + (pos_type)useSig->footer.size() - useSig->next) : 10000000;
			if (outFileSize > useSig->maxBytes && useSig->maxBytes != 0)
				outFileSize = useSig->maxBytes;
			wstring outFileName = JStd::String::Format(L"%s\\%010I64d.%s", outDir.c_str(), (long long) useSig->next, String::ToWide(useSig->ext.c_str(), CP_ACP).c_str());
			ofstream out(outFileName.c_str(), ios::binary | ios::out);
			in.seekg(useSig->next);
			std::string buf;
			buf.resize(outFileSize);
			in.read(&buf[0], buf.size());
			buf.resize(in.gcount());
			out.write(&buf[0], buf.size());
			currPos = in.tellg();
			useSig->next = 0;
		}
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}
#ifdef _DEBUG
	char c;
	cin >> c;
#endif
	return 0;
}
