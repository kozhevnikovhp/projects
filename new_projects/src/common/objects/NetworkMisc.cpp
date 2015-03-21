#include "NetworkMisc.h"

int GetLocalHostIpAddress(char *pszIpAddress)
{
	char name[100];
	struct hostent FAR *he;
	if (!gethostname(name,sizeof(name)))
	{
		name[99]=0;
		if (he=gethostbyname(name))
		{
			//char **pc;
			//str =  he->h_name; //" Official name: " << he->h_name << endl;
			//cout << " Aliases:" << endl << " [" << endl;
			/*for (pc=he->h_aliases; *pc; pc++)
			{
				str = *pc;
			}*/
			//cout << " Address type: " << he->h_addrtype << " Address length: " << he->h_length << endl;
			//cout << " Addresses:" << endl << " [" << endl;
			/*for (pc=he->h_addr_list; *pc; pc++)
			{
				str = inet_ntoa(*(struct in_addr*)*pc);
			}*/
			strcpy(pszIpAddress, inet_ntoa(*(struct in_addr*)*he->h_addr_list));
			return 1;
		}
		else
		{
			strcpy(pszIpAddress, "0.0.0.0");
		}
	}
	else
	{
		strcpy(pszIpAddress, "0.0.0.0");
	}
	return 0; // error
}