#include "rpi4_callbacks.h"

// CREATE MEDIATOR OBJECT ???? 

static pthread_t listenThread;
static int listenSocket;

static void *listenUDP(void *_);

OPCUA_STATE_MACHINE(rpi4_connection_sm)
{
    int res = 0;
    if(targetState == *state)
        return UA_STATUSCODE_GOOD;
    
    switch(targetState) {
        /* Disabled or Error */
        case UA_PUBSUBSTATE_ERROR:
        case UA_PUBSUBSTATE_DISABLED:
        case UA_PUBSUBSTATE_PAUSED:
            printf("XXX Closing the UDP multicast connection\n");
            if(listenSocket != 0)
                shutdown(listenSocket, SHUT_RDWR);
            *state = targetState;
            break;

        /* Operational */
        case UA_PUBSUBSTATE_PREOPERATIONAL:
        case UA_PUBSUBSTATE_OPERATIONAL:
            if(listenSocket != 0) {
                *state = UA_PUBSUBSTATE_OPERATIONAL;
                break;
            }
            printf("XXX Opening the UDP multicast connection\n");
            *state = UA_PUBSUBSTATE_PREOPERATIONAL;
            res = pthread_create(&listenThread, NULL, listenUDP, NULL);
            if(res != 0)
                return UA_STATUSCODE_BADINTERNALERROR;
            break;

        /* Unknown state */
        default:
            return UA_STATUSCODE_BADINTERNALERROR;
    }
    return UA_STATUSCODE_GOOD;
}

static void *listenUDP(void *_) 
{
    (void)_;

    /* Block SIGINT for correct shutdown via the main thread */
    sigset_t blockset;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGINT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);

    /* Extract the hostname */
    UA_UInt16 port = 0;
    UA_String hostname = UA_STRING_NULL;
    UA_parseEndpointUrl(&networkAddressUrl.url, &hostname, &port, NULL);

    /* Get all the interface and IPv4/6 combinations for the configured hostname */
    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 and IPv6 */
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    /* getaddrinfo */
    char portstr[6];
    char hostnamebuf[256];
    snprintf(portstr, 6, "%d", port);
    memcpy(hostnamebuf, hostname.data, hostname.length);
    hostnamebuf[hostname.length] = 0;
    int result = getaddrinfo(hostnamebuf, portstr, &hints, &info);
    if(result != 0) {
        printf("XXX getaddrinfo failed\n");
        return NULL;
    }

    /* Open the socket */
    listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if(listenSocket <= 0) {
        printf("XXX Cannot create the socket\n");
        return NULL;
    }

    /* Set socket options */
    int opts = fcntl(listenSocket, F_GETFL);
    result |= fcntl(listenSocket, F_SETFL, opts | O_NONBLOCK);
    int optval = 1;
    result |= setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR,
                         (const char*)&optval, sizeof(optval));
    if(result < 0) {
        printf("XXX Cannot set the socket options\n");
        return NULL;
    }

    /* Bind the socket */
    result = bind(listenSocket, info->ai_addr, (socklen_t)info->ai_addrlen);
    if(result < 0) {
        printf("XXX Cannot bind the socket\n");
        return NULL;
    }

    /* Join the multicast group */
    if(info->ai_family == AF_INET) {
        struct ip_mreqn ipv4;
        struct sockaddr_in *sin = (struct sockaddr_in *)info->ai_addr;
        ipv4.imr_multiaddr = sin->sin_addr;
        ipv4.imr_address.s_addr = htonl(INADDR_ANY); /* default ANY */
        ipv4.imr_ifindex = 0;
        result = setsockopt(listenSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipv4, sizeof(ipv4));
    } else if(info->ai_family == AF_INET6) {
        struct ipv6_mreq ipv6;
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)info->ai_addr;
        ipv6.ipv6mr_multiaddr = sin6->sin6_addr;
        ipv6.ipv6mr_interface = 0; /* default ANY interface */
        result = setsockopt(listenSocket, IPPROTO_IPV6, IPV6_JOIN_GROUP, &ipv6, sizeof(ipv6));
    }
    if(result < 0) {
        printf("XXX Cannot join the multicast group\n");
        return NULL;
    }

    freeaddrinfo(info);

    /* The connection is open, change the state to OPERATIONAL.
     * The state machine checks whether listenSocket != 0. */
    printf("XXX Listening on UDP multicast (%s, port %u)\n",
           hostnamebuf, (unsigned)port);
    UA_Server_enablePubSubConnection(server, connectionIdentifier);

    /* Poll and process in a loop.
     * The socket is closed in the state machine and */
    struct pollfd pfd;
    pfd.fd = listenSocket;
    pfd.events = POLLIN;
    while(true) {
        result = poll(&pfd, 1, -1); /* infinite timeout */
        if(result < 0 || pfd.revents & POLLERR || pfd.revents & POLLHUP || pfd.revents & POLLNVAL)
            break;

        if(pfd.revents & POLLIN) {
            static char buf[1024];
            ssize_t size = read(listenSocket, buf, sizeof(buf));
            if(size > 0) {
                printf("XXX Received a packet\n");
                UA_ByteString packet = {(size_t)size, (UA_Byte*)buf};
                UA_Server_processPubSubConnectionReceive(server, connectionIdentifier, packet);
            }
        }
    }

    printf("XXX The UDP multicast connection is closed\n");

    /* Clean up and notify the state machine */
    close(listenSocket);
    listenSocket = 0;
    UA_Server_disablePubSubConnection(server, connectionIdentifier);
    return NULL;
}



