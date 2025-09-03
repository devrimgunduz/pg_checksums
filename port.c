/*
 * pg_checksums_ext
 *
 * Verifies/enables/disables data checksums
 *
 *	Copyright (c) 2010-2025, PostgreSQL Global Development Group
 *
 *	port.c
 */

#include "postgres_fe.h"

#include "port.h"

#include <dirent.h>
#include <sys/stat.h>

/*
 * Look at the version string stored in PG_VERSION and decide if this utility
 * can be run safely or not (adpated from pg_resetwal).
 */
void
CheckDataVersion(char *DataDir)
{
	const char *ver_file = "PG_VERSION";
	char		ver_filepath[MAXPGPATH];
	FILE	   *ver_fd;
	char		rawline[64];
	int			len;

	strcpy(ver_filepath, DataDir);
	strcat(ver_filepath, "/");
	strcat(ver_filepath, ver_file);

	if ((ver_fd = fopen(ver_filepath, "r")) == NULL)
	{
		pg_log_error("could not open file \"%s\" for reading: %m",
					 ver_filepath);
		exit(1);
	}

	/* version number has to be the first line read */
	if (!fgets(rawline, sizeof(rawline), ver_fd))
	{
		if (!ferror(ver_fd))
			pg_log_error("unexpected empty file \"%s\"", ver_filepath);
		else
			pg_log_error("could not read file \"%s\": %m", ver_filepath);
		exit(1);
	}

	/* strip trailing newline and carriage return */
	len = strlen(rawline);
	if (len > 0 && rawline[len - 1] == '\n')
	{
		rawline[--len] = '\0';
		if (len > 0 && rawline[len - 1] == '\r')
			rawline[--len] = '\0';
	}

	if (strcmp(rawline, PG_MAJORVERSION) != 0)
	{
		pg_log_error("data directory is of wrong version");
		pg_log_info("File \"%s\" contains \"%s\", which is not compatible with this program's version \"%s\".",
					ver_file, rawline, PG_MAJORVERSION);
		exit(1);
	}

	fclose(ver_fd);
}

#if PG_VERSION_NUM >= 170000
/*
 * Provide strictly harmonized handling of the --sync-method option.
 */
bool
parse_sync_method(const char *optarg, DataDirSyncMethod *sync_method)
{
        if (strcmp(optarg, "fsync") == 0)
                *sync_method = DATA_DIR_SYNC_METHOD_FSYNC;
        else if (strcmp(optarg, "syncfs") == 0)
        {
#ifdef HAVE_SYNCFS
                *sync_method = DATA_DIR_SYNC_METHOD_SYNCFS;
#else
                pg_log_error("this build does not support sync method \"%s\"",
                                         "syncfs");
                return false;
#endif
        }
        else
        {
                pg_log_error("unrecognized sync method: %s", optarg);
                return false;
        }

        return true;
}
#endif /* PG_VERSION_NUM >= 170000 */
