/*
 * CFile.cpp
 *
 *  Created on: 2009. 12. 13.
 *      Author: buttonfly
 */

#include "CFile.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <string.h>

CFile::CFile() {
	// TODO Auto-generated constructor stub

}

CFile::CFile(const char* path)
{
	m_path = path;
}

CFile::~CFile() {
	// TODO Auto-generated destructor stub
}


bool CFile::Exist(void)
{
	int fd;
	fd = open(m_path.c_str(), O_RDONLY);
	if(fd < 0)
	{
		return false;
	}
	close(fd);
	return true;
}

const char* CFile::GetPath(void) const
{
	return m_path.c_str();
}

int CFile::Size(void) const
{
	int fd;
	fd = open(m_path.c_str(), O_RDONLY);
	if(fd < 0)
	{
		g_debug("%s doesn't exist", m_path.c_str());
		return -1;
	}

    int             ret;
    struct stat        st;
    memset(&st, 0, sizeof(st));

    ret = fstat(fd, &st);
    if(ret < 0)
    {
        return -1;
    }
    close(fd);
    return st.st_size;
}


