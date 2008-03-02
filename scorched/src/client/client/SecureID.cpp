#include <client/SecureID.h>
#include <net/NetInterface.h>
#include <common/Defines.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

std::string SecureID::GetPrivateKey(void)
{
    IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for 16 cards
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

    DWORD dwStatus = GetAdaptersInfo ( AdapterInfo, &dwBufLen); // Get info
	if (dwStatus != NO_ERROR) return ""; // Check status

	std::string Key;
    for (unsigned int i=0; i<MIN(6, AdapterInfo[0].AddressLength); i++)
    {
        Key += (((unsigned int)AdapterInfo[0].Address[i])&255);
        if (i != 5) Key += ':';
    }
    return Key;
}

#elif defined(__DARWIN__)

std::string SecureID::GetPrivateKey(void)
{
	return "0:0:0:0:0:0";
}

#else

#include <sys/ioctl.h>
#include <net/if.h>

std::string SecureID::GetPrivateKey(void)
{
	std::string Key;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		return ""; //not a valid socket
	}

	struct ifreq dev; //container for the hardware data
	struct if_nameindex *NameList = if_nameindex(); //container for the interfaces list
	if (NameList == NULL)
	{
		close(sock);
		return ""; //cannot list the interfaces
	}

	int pos = 0;
	std::string InterfaceName;
	do
	{
		if (NameList[pos].if_index == 0)
		{
			close(sock);
			if_freenameindex(NameList);
			return ""; // no valid interfaces found
		}
		InterfaceName = NameList[pos].if_name;
		pos++;
	} while (InterfaceName.substr(0,2) == "lo" || InterfaceName.substr(0,3) == "sit");

	if_freenameindex(NameList); //free the memory

	strcpy (dev.ifr_name, InterfaceName.c_str()); //select from the name
	if (ioctl(sock, SIOCGIFHWADDR, &dev) < 0) //get the interface data
	{
		close(sock);
		return ""; //cannot list the interfaces
	}

    for (int i=0; i<6; i++)
    {
        Key += (((unsigned int)dev.ifr_hwaddr.sa_data[i])&255);
        if (i != 5) Key += ':';
    }
	close(sock);

    return Key;
}
#endif

std::string SecureID::MakeID(std::string ServerAddress)
{
    std::string Key = GetPrivateKey();
	if (Key.empty()) //is the key retrived?
    {
		return "";
	}

    Key += ServerAddress;
    return shacalc.GetHash(sha2::enuSHA256, 
		(const unsigned char *)Key.c_str(), Key.size()); //scrambles the key with sha2
}

std::string SecureID::getSecureID (unsigned int ip)
{
	std::string ServerAddress = NetInterface::getIpName(ip); //the ip we're connected to
	return MakeID (ServerAddress); //computes the SUI for that server & returns it
}
