/*
*   C++ sockets on Unix and Windows
*   Copyright (C) 2002
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <ACM/INTERNAL/UDPSocket.h>
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <fcntl.h>
#include <stdio.h>

typedef void raw_type;       // Type used for raw data on this platform
#include <errno.h>             // For errno
// SocketException Code
SocketException::SocketException(const string &message, bool inclSysMsg)
	throw() : userMessage(message) {
		if (inclSysMsg) {
			userMessage.append(": ");
			userMessage.append(strerror(errno));
		}
}

SocketException::~SocketException() throw() {
}

const char *SocketException::what() const throw() {
	return userMessage.c_str();
}

// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port,
	sockaddr_in &addr) {
		memset(&addr, 0, sizeof(addr));  // Zero out address structure
		addr.sin_family = AF_INET;       // Internet address

		hostent *host;  // Resolve name
		if ((host = gethostbyname(address.c_str())) == NULL) {
			// strerror() will not work for gethostbyname() and hstrerror()
			// is supposedly obsolete
			throw SocketException("Failed to resolve name (gethostbyname())");
		}
		addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

		addr.sin_port = htons(port);     // Assign port in network byte order
}

// Socket Code

AbstractSocket::AbstractSocket(int type, int protocol) throw(SocketException) {

	// Make a new socket
	if ((sockDesc = socket(PF_INET, type, protocol)) < 0) {
		throw SocketException("Socket creation failed (socket())", true);
	}
	BLOCKING_STATUS=BLOCKING;
}

void AbstractSocket::make_non_blocking()
{
	int opts;
	opts = fcntl ( sockDesc,F_GETFL );
	if ( opts < 0 )
	{
		return;
	}
	//if ( b )
	//opts = ( opts | O_NONBLOCK );
	//else
	opts = ( opts|O_NONBLOCK );

	if(fcntl ( sockDesc,F_SETFL,opts ) <0)
		printf ("error in setting non blocking \n");
	else
	{
		BLOCKING_STATUS=NON_BLOCKING;
	}
	//	else
	//		printf ("Set to non blocking \n");

}

void AbstractSocket::make_blocking()
{
	int opts;
	opts = fcntl ( sockDesc,F_GETFL );
	if ( opts < 0 )
	{
		return;
	}
	//if ( b )
	//opts = ( opts | O_NONBLOCK );
	//else
	opts = ( opts & ~O_NONBLOCK );

	if(fcntl ( sockDesc,F_SETFL,opts ) <0)
		printf ("error in setting blocking \n");
	else
	{
		//	printf ("Set to blocking \n");
		BLOCKING_STATUS=BLOCKING;

	}

}

AbstractSocket::AbstractSocket(int sockDesc) {
	this->sockDesc = sockDesc;
}

AbstractSocket::~AbstractSocket() {

	::close(sockDesc);

	sockDesc = -1;
}

string AbstractSocket::getLocalAddress() throw(SocketException) {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
		throw SocketException("Fetch of local address failed (getsockname())", true);
	}
	return inet_ntoa(addr.sin_addr);
}

unsigned short AbstractSocket::getLocalPort() throw(SocketException) {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
		throw SocketException("Fetch of local port failed (getsockname())", true);
	}
	return ntohs(addr.sin_port);
}

void AbstractSocket::setLocalPort(unsigned short localPort) throw(SocketException) {
	// Bind the socket to its port
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(localPort);

	if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
		throw SocketException("Set of local port failed (bind())", true);
	}
}

void AbstractSocket::setLocalAddressAndPort(const string &localAddress,
	unsigned short localPort) throw(SocketException) {
		// Get the address of the requested host
		sockaddr_in localAddr;
		fillAddr(localAddress, localPort, localAddr);

		if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) {
			throw SocketException("Set of local address and port failed (bind())", true);
		}
}

void AbstractSocket::cleanUp() throw(SocketException) {

}

unsigned short AbstractSocket::resolveService(const string &service,
	const string &protocol) {
		struct servent *serv;        /* Structure containing service information */

		if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
			return atoi(service.c_str());  /* Service is port number */
		else
			return ntohs(serv->s_port);    /* Found port (network byte order) by name */
}

// CommunicatingSocket Code

CommunicatingSocket::CommunicatingSocket(int type, int protocol)
	throw(SocketException) : AbstractSocket(type, protocol) {
}

CommunicatingSocket::CommunicatingSocket(int newConnSD) : AbstractSocket(newConnSD) {
}

void CommunicatingSocket::connect(const string &foreignAddress,
	unsigned short foreignPort) throw(SocketException) {
		// Get the address of the requested host
		sockaddr_in destAddr;
		fillAddr(foreignAddress, foreignPort, destAddr);

		// Try to connect to the given port
		if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0) {
			throw SocketException("Connect failed (connect())", true);
		}
}

void CommunicatingSocket::send(const void *buffer, int bufferLen)
	throw(SocketException) {
		if (::send(sockDesc, (raw_type *) buffer, bufferLen, 0) < 0) {
			throw SocketException("Send failed (send())", true);
		}
}

int CommunicatingSocket::recv(void *buffer, int bufferLen)
	throw(SocketException) {
		int rtn;
		if ((rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0)) < 0) {
			throw SocketException("Received failed (recv())", true);
		}

		return rtn;
}

string CommunicatingSocket::getForeignAddress()
	throw(SocketException) {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);

		if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
			throw SocketException("Fetch of foreign address failed (getpeername())", true);
		}
		return inet_ntoa(addr.sin_addr);
}

unsigned short CommunicatingSocket::getForeignPort() throw(SocketException) {
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
		throw SocketException("Fetch of foreign port failed (getpeername())", true);
	}
	return ntohs(addr.sin_port);
}


// UDPSocket Code

UDPSocket::UDPSocket() throw(SocketException) : CommunicatingSocket(SOCK_DGRAM,
	IPPROTO_UDP),ref(1) {
		setBroadcast();
}

UDPSocket::UDPSocket(unsigned short localPort)  throw(SocketException) :
	CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP),ref(1) {
		setLocalPort(localPort);
		setBroadcast();
}

UDPSocket::UDPSocket(const string &localAddress, unsigned short localPort)
	throw(SocketException) : CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP),ref(1) {
		setLocalAddressAndPort(localAddress, localPort);
		setBroadcast();
}

void UDPSocket::setBroadcast() {
	// If this fails, we'll hear about it when we try to send.  This will allow
	// system that cannot broadcast to continue if they don't plan to broadcast
	int broadcastPermission = 1;
	setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST,
		(raw_type *) &broadcastPermission, sizeof(broadcastPermission));
}

void UDPSocket::disconnect() throw(SocketException) {
	sockaddr_in nullAddr;
	memset(&nullAddr, 0, sizeof(nullAddr));
	nullAddr.sin_family = AF_UNSPEC;

	// Try to disconnect
	if (::connect(sockDesc, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0) {
#ifdef WIN32
		if (errno != WSAEAFNOSUPPORT) {
#else
		if (errno != EAFNOSUPPORT) {
#endif
			throw SocketException("Disconnect failed (connect())", true);
		}
	}
}

void UDPSocket::sendTo(const void *buffer, int bufferLen,
	const string &foreignAddress, unsigned short foreignPort)
	throw(SocketException) {
		sockaddr_in destAddr;
		fillAddr(foreignAddress, foreignPort, destAddr);
		//printf("Address filled\n");

		// Write out the whole buffer as a single message.
		if (sendto(sockDesc, (raw_type *) buffer, bufferLen, 0,
			(sockaddr *) &destAddr, sizeof(destAddr)) != bufferLen) {
				printf("Send failed (sendto())\n");
				throw SocketException("Send failed (sendto())", true);
		}
}

int UDPSocket::recvFrom(void *buffer, int bufferLen, string &sourceAddress,
	unsigned short &sourcePort) throw(SocketException) {
		sockaddr_in clntAddr;
		socklen_t addrLen = sizeof(clntAddr);
		int rtn;
		if ((rtn = recvfrom(sockDesc, (raw_type *) buffer, bufferLen, 0,
			(sockaddr *) &clntAddr, (socklen_t *) &addrLen)) < 0) {
				return rtn;
				//throw SocketException("Receive failed (recvfrom())", true);
		}
		sourceAddress = inet_ntoa(clntAddr.sin_addr);
		sourcePort = ntohs(clntAddr.sin_port);

		return rtn;
}

void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(SocketException) {
	if (setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_TTL,
		(raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0) {
			throw SocketException("Multicast TTL set failed (setsockopt())", true);
	}
}

void UDPSocket::joinGroup(const string &multicastGroup) throw(SocketException) {
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(raw_type *) &multicastRequest,
		sizeof(multicastRequest)) < 0) {
			throw SocketException("Multicast group join failed (setsockopt())", true);
	}
}

void UDPSocket::setReuse()
{
	int yes = 1;
		setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR,
			(raw_type *) &yes, sizeof(yes));
}
void UDPSocket::leaveGroup(const string &multicastGroup) throw(SocketException) {
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(raw_type *) &multicastRequest,
		sizeof(multicastRequest)) < 0) {
			throw SocketException("Multicast group leave failed (setsockopt())", true);
	}
}

void UDPSocket::clearBuffer()
{
	char buf[100];
	if (this->BLOCKING_STATUS==BLOCKING)
	{
		this->make_non_blocking();
		int recvMsgSize=0;
		string sourceAddress;             // Address of datagram source
		unsigned short sourcePort;        // Port of datagram source
		while(true)
		{
			recvMsgSize = this->recvFrom(&(buf[0]), 100, sourceAddress,	sourcePort);
			if (recvMsgSize==-1 && (errno==EAGAIN || errno==EWOULDBLOCK))
			{
				this->make_blocking();
				return;
			}
		}
	}
	else
	{
		int recvMsgSize=0;
		string sourceAddress;             // Address of datagram source
		unsigned short sourcePort;        // Port of datagram source
		while(true)
		{
			recvMsgSize = this->recvFrom(&(buf[0]), 100, sourceAddress,	sourcePort);
			if (recvMsgSize==-1 && (errno==EAGAIN || errno==EWOULDBLOCK))
			{
				return;
			}
		}
	}
}
