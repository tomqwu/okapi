#include "win32extras.h"

int gettimeofday(struct timeval *tp, void *tz)
{
	time_t			ltime;
	struct _timeb	tstruct;
    
	time(&ltime);							// Determine the system time (Since UTC 1/1/70)	
	_ftime(&tstruct);						// Populate tstruct to obtain milliseconds

	tp->tv_sec = ltime;						// Note : tstruct.millitm is divided by 1000 to give microseconds
	tp->tv_usec = tstruct.millitm * 1000;

	return 0;
}

pid_t getpid(void)
{
	return (pid_t)GetCurrentProcessId();
}

uid_t getuid(void)
{
	return 0;
}

gid_t getgid(void)
{
	return 0;
}

int getgroups(int gidsetsize, gid_t *grouplist)
{
	return 0;
}

char *cuserid(char *s)
{
	DWORD	dwBufSize = 32;
	char	szBuffer[32];

	if (GetUserName(szBuffer, &dwBufSize))
	{
		strcpy(s, szBuffer);
	}

	return 0;
}

int ftruncate(int fildes, off_t length)
{
	return 0;
}

BYTE* GetSetting(LPCSTR strSettingName)
{
	LONG	Result			= 0;
	HKEY	hKey			= 0;
	DWORD	dwBuffSize		= 0;
	BYTE	*szBuffer		= NULL;
	
	// Open the registry path for BSS
	
	if((Result = RegOpenKeyEx(BSS_REG_BASE_KEY, BSS_REG_PATH, 0, KEY_QUERY_VALUE, &hKey)) == ERROR_SUCCESS)	
	{
		if((Result = RegQueryValueEx(hKey, strSettingName, NULL, NULL, NULL, &dwBuffSize)) == ERROR_SUCCESS)
		{
			// Allocate the memory to hold the data
		
			szBuffer = (BYTE*)malloc((size_t)dwBuffSize);
			
			// Read the data into the buffer

			RegQueryValueEx(hKey, strSettingName, NULL, NULL, szBuffer, &dwBuffSize);			
		}

		RegCloseKey(hKey);
	}

	return szBuffer;
}
