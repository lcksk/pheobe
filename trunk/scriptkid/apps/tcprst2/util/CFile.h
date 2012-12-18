/*
 * CFile.h
 *
 *  Created on: 2009. 12. 13.
 *      Author: buttonfly
 */

#ifndef CFILE_H_
#define CFILE_H_

#include <string>

using namespace std;

class CFile {
public:
	CFile();
	CFile(const char* path);
	virtual ~CFile();

	bool Exist(void);

	const char* GetPath(void) const;
	int Size(void) const;

private:
	string m_path;
};

#endif /* CFILE_H_ */
