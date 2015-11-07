/*
// Create a mutex
		if ((in_handle = CreateMutex(NULL,
			(init_owner_flag == MTIF_TRUE) ? TRUE : FALSE, mtr_name)) != NULL)
			return_code = MTIF_SUCCESS;

// Release a mutex
		if (ReleaseMutex(in_handle) == 0) {
			*** ERROR ***
		}

// Wait until a mutex is available and lock it
// Will return when you have the mutex.
		if (((return_code = WaitForSingleObject(in_handle, time_out)) ==
			WAIT_OBJECT_0) || (return_code == WAIT_ABANDONED))
			return_code = MTIF_SUCCESS;
		else if (return_code == WAIT_TIMEOUT) {
			if (time_out != INFINITE) {
//				*** Wait wasn't infinite and timed-out (not really an error) ***
				if (error_text != NULL)
					strcat(strcat(strcat(val2str_ptr0_cat(in_handle,
						strcpy(error_text, "Call to 'WaitForSingleObject(")), ", "),
						val2str_ulong(((unsigned long) time_out), 10, NULL, buffer)),
						")' timed-out.");
				return_code= MTIF_LOCK_ACQ_FAILURE;
			}
			else {
// Wait WAS infinite: Should've gotten the mutex or waited forever, but didn't!
				if (error_text != NULL)
					strcat(strcat(val2str_ptr0_cat(in_handle,
						strcpy(error_text,
						"Error on call to 'WaitForSingleObject(")),
						", INFINITE)': time-out is 'INFINITE' but function "),
						"returned 'WAIT_TIMEOUT'.");
				return_code = MTIF_SYSTEM_FAILURE;
			}
		}
		else {
// Some other funky error... Dunno what happened...
			if (error_text != NULL)
				MTIF_SUPP_AppendLastErrorString_Win32(strcat(strcat(strcat(
					val2str_ptr0_cat(in_handle,
					strcpy(error_text, "Call to 'WaitForSingleObject(")), ", "),
					(time_out == INFINITE) ? "INFINITE" :
					val2str_ulong(((unsigned long) time_out), 10, NULL, buffer)),
					")' failed: "));
			return_code = MTIF_SYSTEM_FAILURE;
		}

*/

// Returns mutex handle on success, NULL on failure.
// On success, mutex is created AND locked.
HANDLE JAG_CreateMutex()
{
	return(CreateMutex(NULL, 1, NULL));
}

// in_handle is the handle from a JAG_CreateMutex call
// time_out is milliseconds to wait or INFINITE to wait forever.
//
// Returns 0 on lock acquired.
// 1 on time-out.
// -1 on error.
int JAG_LockMutex(in_handle, time_out)
{
	int return_code;

	if (((return_code = WaitForSingleObject(in_handle, time_out)) ==
		WAIT_OBJECT_0) || (return_code == WAIT_ABANDONED))
		return(0);
	else if ((return_code == WAIT_TIMEOUT) && (time_out != INFINITE))
		return(1);
	else
		return(-1);
}

void JAG_DestroyMutex(in_handle)
{
	CloseHandle(in_handle);
}


