/*
 * CGKeyFile.h
 *
 *  Created on: Mar 11, 2009
 *      Author: buttonfly
 */

#ifndef CGKEYFILE_H_
#define CGKEYFILE_H_

#include <glib.h>

class CGKeyFile {
public:

	CGKeyFile(const gchar* file);

	virtual ~CGKeyFile();

	bool Load(void);
	bool Store(void);
	void Set(const char* group, const char* key, const char* value);
	void Set(const char* group, const char* key, gint value);
	gchar* Get(const char* group, const char* key);
	gint GetAsInt(const char* group, const char* key);
	void Close(void);

private:
	GKeyFile*			m_keyfile;
	const gchar*		m_filename;
};

#endif /* CGKEYFILE_H_ */
