/*
 * pg_checksums_ext
 *
 * Verifies/enables/disables data checksums
 *
 *	Copyright (c) 2010-2025, PostgreSQL Global Development Group
 *
 *	port.h
 */

#include <unistd.h>

#include "catalog/pg_control.h"
#include "common/file_perm.h"

#include "common/logging.h" 
#if PG_VERSION_NUM >= 140000
#include "fe_utils/option_utils.h"
#endif

#if PG_VERSION_NUM < 170000
/*
 * Filename components.
 *
 * XXX: fd.h is not declared here as frontend side code is not able to
 * interact with the backend-side definitions for the various fsync
 * wrappers.
 */
#define PG_TEMP_FILES_DIR "pgsql_tmp"
#define PG_TEMP_FILE_PREFIX "pgsql_tmp"
#endif

#if PG_VERSION_NUM < 180000
/*
 * Tablespace path (relative to installation's $PGDATA).
 *
 * These values should not be changed as many tools rely on it.
 */
#define PG_TBLSPC_DIR "pg_tblspc"
#define PG_TBLSPC_DIR_SLASH "pg_tblspc/"       /* required for strings
												* comparisons */
#endif

/*
 * pg_xlog has been renamed to pg_wal in version 10.
 */
#define MINIMUM_VERSION_FOR_PG_WAL      100000

/*
 * The control file (relative to $PGDATA)
 */
#define XLOG_CONTROL_FILE	"global/pg_control"

extern char *DataDir;

#if PG_VERSION_NUM >=  170000
extern bool parse_sync_method(const char *optarg,
			      DataDirSyncMethod *sync_method);
#endif

void CheckDataVersion(char *DataDir);
