/*
 * CGKeyFile.cpp
 *
 *  Created on: Mar 11, 2009
 *      Author: buttonfly
 */

#include "CGKeyFile.h"

G_LOCK_DEFINE(keyfile);

CGKeyFile::CGKeyFile(const gchar* file)
: m_filename(file)
{
	if(!g_thread_supported())
	{
		g_thread_init(NULL);
	}
	m_keyfile = g_key_file_new();
}

CGKeyFile::~CGKeyFile()
{
	Close();
}

void CGKeyFile::Close(void)
{
	if(m_keyfile)
	{
		g_key_file_free(m_keyfile);
		m_keyfile = NULL;
	}
	G_UNLOCK(keyfile);
}

bool CGKeyFile::Load(void)
{
	G_LOCK(keyfile);
	GError* error = NULL;
	if (!g_key_file_load_from_file(m_keyfile, m_filename, (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS| G_KEY_FILE_KEEP_TRANSLATIONS),&error))
	{
		g_debug("fail loading %s: %s", m_filename, error->message);
		g_error_free(error);
		G_UNLOCK(keyfile);
		return false;
	}
	G_UNLOCK(keyfile);
	return true;
}


bool CGKeyFile::Store(void)
{
	G_LOCK(keyfile);
	gsize length, nr;
	GError* error = NULL;
    char *data = g_key_file_to_data(m_keyfile, &length, &error);
    GIOChannel *gio;

    if (!data)
    {
    	G_UNLOCK(keyfile);
    	return false;
    }

    gio = g_io_channel_new_file(m_filename, "w+", &error);
    if (!gio)
    {
    	g_free(data);
    	G_UNLOCK(keyfile);
        return false;
    }

    g_io_channel_write_chars(gio,  (const gchar*)data, length, &nr, &error);
    g_free(data);
    g_io_channel_unref(gio);
    G_UNLOCK(keyfile);
    return true;
}

void CGKeyFile::Set(const char* group, const char* key, const char* value)
{
	G_LOCK(keyfile);
	g_key_file_set_string(m_keyfile,  (const gchar*)group,  (const gchar*)key, value);
	G_UNLOCK(keyfile);
}

void CGKeyFile::Set(const char* group, const char* key, gint value)
{
	G_LOCK(keyfile);
	g_key_file_set_integer(m_keyfile,  (const gchar*)group,  (const gchar*)key, value);
	G_UNLOCK(keyfile);
}

gchar* CGKeyFile::Get(const char* group, const char* key)
{
	G_LOCK(keyfile);
	GError* error = NULL;
	if(!g_key_file_has_key(m_keyfile, (const gchar*) group,  (const gchar*)key, &error))
	{
		g_debug("[%s::%s] - %s", group, key, error->message);
		G_UNLOCK(keyfile);
		return NULL;
	}

	gchar* value = g_key_file_get_string(m_keyfile,  (const gchar*)group,  (const gchar*)key, &error);
	G_UNLOCK(keyfile);
	return value;
}


gint CGKeyFile::GetAsInt(const char* group, const char* key)
{
	G_LOCK(keyfile);
	GError* error = NULL;
	if(!g_key_file_has_key(m_keyfile, group, key, &error))
	{
		g_debug("[%s::%s] - %s", group, key, error->message);
		G_UNLOCK(keyfile);
		return -1;
	}

	gint value = g_key_file_get_integer(m_keyfile, group, key, &error);
	G_UNLOCK(keyfile);
	return value;
}
