#include <unistd.h> /* getpid */
#include <string.h> /* strcmp, memset, strcpy */
#include <arpa/inet.h> /* sockaddr_in, INET_ADDRSTRLEN, inet_ntop */

#include "uid.h"

static size_t counter_glob = 0;

const ilrd_uid_t bad_uid = {0};

void GetIPAddrs(unsigned char *ip);


/*_______________________user func_______________________*/

ilrd_uid_t UIDCreate()
{
	ilrd_uid_t uid = {0};
	
	uid.time_stamp = time(NULL);
	uid.counter = ++counter_glob;
	uid.pid = getpid();
	
	GetIPAddrs(uid.ip);

	if ('\0' == *uid.ip)
	{	
		uid = UIDGetBad();
	}

	return uid;
}

 
ilrd_uid_t UIDGetBad(void)
{
	return bad_uid;
}


int UIDIsSame(ilrd_uid_t lhs, ilrd_uid_t rhs)
{
	return (lhs.time_stamp == rhs.time_stamp 
	&& lhs.counter == rhs.counter 
	&& lhs.pid == rhs.pid 
	&& 0 == strcmp((char *)lhs.ip, (char *)rhs.ip));
}


/*_______________________inner func_______________________*/

void GetIPAddrs(unsigned char *ip)
{
	struct ifaddrs *ifaddrs_strct = {0};
	struct ifaddrs *ifa = {0};
	void *temp = NULL;
	unsigned char addrs_buff[14] = {0};
	
	if (0 == getifaddrs(&ifaddrs_strct))
	{
	
	
		for (ifa = ifaddrs_strct; ifa != NULL; ifa = ifa->ifa_next)
		{
			if (!ifa->ifa_addr)
			{
				memset((void *)ip, '\0', 14);
			}
			
			if (ifa->ifa_addr->sa_family == AF_INET) /*IPV4*/
			{
				temp = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
				
				inet_ntop(AF_INET, temp, (char *)addrs_buff, INET_ADDRSTRLEN);
				
				if(0 != strcmp(ifa->ifa_name, "lo"))
				{
					strcpy((char *)ip, (char *)addrs_buff);
				}
			}
		}
		
		if (NULL != ifaddrs_strct)
		{
			freeifaddrs(ifaddrs_strct);
		}	
	}
}















