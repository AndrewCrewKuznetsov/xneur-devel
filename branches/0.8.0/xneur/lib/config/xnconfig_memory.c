/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  (c) XNeur Team 2006
 *
 */

#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "types.h"

#include "xnconfig_memory.h"

#define SEGMENT_PERMISSIONS	(SHM_R) | (SHM_W)

static int get_memory_id(int segment_size, int *need_init)
{
	key_t segment_key = ftok("/", getuid());
	if (segment_key == -1)
	{
		log_message(ERROR, "Can't generate shared memory segment key");
		return -1;
	}

	int shm_id = shmget(segment_key, segment_size, SEGMENT_PERMISSIONS);
	if (shm_id != -1)
	{
		*need_init = FALSE;
		return shm_id;
	}

	*need_init = TRUE;

	shm_id = shmget(segment_key, segment_size, SEGMENT_PERMISSIONS | IPC_CREAT);
	if (shm_id != -1)
		return shm_id;

	// Segment size was changed - removing
	shm_id = shmget(segment_key, 0, SEGMENT_PERMISSIONS);
	if (shm_id == -1)
	{
		log_message(ERROR, "Can't get exist shared memory segment id");
		return -1;
	}

	if (shmctl(shm_id, IPC_RMID, NULL) != 0)
	{
		log_message(ERROR, "Can't remove an old shared memory segment");
		return -1;
	}

	shm_id = shmget(segment_key, segment_size, SEGMENT_PERMISSIONS | IPC_CREAT);
	if (shm_id == -1)
	{
		log_message(ERROR, "Can't create new shared memory segment");
		return -1;
	}

	return shm_id;
}

void* attach_memory_segment(int segment_size)
{
	int need_init;

	int shm_id = get_memory_id(segment_size, &need_init);
	if (shm_id == -1)
		return NULL;

	void *memory_segment = shmat(shm_id, NULL, 0);
	if (memory_segment == NULL)
		return NULL;

	if (need_init == TRUE)
		memset(memory_segment, 0, segment_size);

	return memory_segment;
}
