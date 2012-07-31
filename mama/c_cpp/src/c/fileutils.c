/* $Id: fileutils.c,v 1.5.32.2 2011/08/10 14:53:25 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <mama/log.h>
#include <mama/msgstatus.h>

#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "fileutils.h"

mama_status openWombatFile(
  const char*        fileName,
  const char*        mode,
  FILE**             file)
{ 
  FILE*        f          = NULL;
  mama_status status = MAMA_STATUS_OK;
  
  if ((fileName[0] != '/') && (fileName[0] != '.') && (fileName[1] != ':'))
  {
    /* Not an absolute pathname or an explicit relative pathname
     * so try to find it in $WOMBAT_PATH. */
    char* wombatPath = getenv ("WOMBAT_PATH");

    if (wombatPath)
    {
      char tmpFileName[1024];

      snprintf (tmpFileName, 1024, "%s%s%s", wombatPath, PATHSEP, fileName);
      mama_log (MAMA_LOG_LEVEL_FINE,
		"openWombatFile: checking for file: %s", 
		tmpFileName);
      f = fopen (tmpFileName, mode);
    }
  }
  if (!f)
  {
    mama_log (MAMA_LOG_LEVEL_FINE,
	      "openWombatFile: checking for file: %s", fileName);
    f = fopen (fileName, mode);
  }
  if (!f)
  {
    status = MAMA_STATUS_INVALID_ARG;
    mama_log (MAMA_LOG_LEVEL_FINE,
	      "openWombatFile: %s error: unable to open file %s", 
              mamaMsgStatus_stringForStatus(status), fileName);
  }
  else
  {
      mama_log (MAMA_LOG_LEVEL_FINE,
                "openWombatFile: sucessfully opened file %s", fileName);
      *file = f;
  }
  return status;
}


size_t stripTrailingWhiteSpace (char*  buffer)
{
    size_t len = strlen (buffer);
    while ((len > 0) && (isspace(buffer[len-1])))
    {
        buffer[len-1] = '\0';
        len--;
    }
    return len;
}
