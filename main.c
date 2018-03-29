#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_INTERFACES 128

int
main()
{

	/**
	 * A zero-initialized structure that holds the coniguration
	 * parameters for the SIOCGIFCONF IOCTL that we issue against
	 * a socket to retrieve a list of network interfaces that have
	 * an underlying IP associated with it.
	 */
	struct ifconf config = { 0 };

	/**
	 * A list that will be filled by Linux to give us back information
	 * about the interfaces.
	 */
	struct ifreq ifreq[MAX_INTERFACES];

	/**
	 * A temporary buffer for holding the humanized string that
	 * represents the address assigned to that interface.
	 */
	char ip_buffer[16] = { 0 };

	/**
	 * A `sockaddr` address structure casted to the internet form
	 * `sockaddr_in` as retrieved from `ioctl`.
	 */
	struct sockaddr_in* iface_addr;

	int devices_fd;
	int number_of_ifaces;
	int err;

	/**
	 * Open a generic stream-based socket to issue the ioctl
	 * syscall against.
	 *
	 * Given that Linux does not enforce a particular socket type
	 * when issuing the IOCTL request for netdevice, we can create
	 * one that is just like one you'd create for tcp, i.e.,
	 * AF_INET, SOCK_STREAM (but it doesn't really matter).
	 */
	err = (devices_fd = socket(AF_INET, SOCK_STREAM, 0));
	if (err == -1) {
		perror("cannot open socket");
		return 1;
	}

	/**
	 * Fill-in the config request structure, including a buffer to hold
	 * the answer that's going to be filled by the kernel.
	 */
	config.ifc_buf = (char*)ifreq;
	config.ifc_len = MAX_INTERFACES * sizeof(struct ifreq);

	/**
	 * Issue the actual syscall using the request configuration that we
	 * filled above (giving an address to a buffer that will hold the
	 * answer and a hint of how big that buffer is).
	 *
	 * SIOCGIFCONF returns a list  of interface (transport layer) addresses.
	 */
	err = ioctl(devices_fd, SIOCGIFCONF, (char*)&config);
	if (err == -1) {
		perror("ioctl SIOCGIFCONF failed\n");
		close(devices_fd);
		return 2;
	}

	/**
	 * Parse the results and then print the interfaces names.
	 */
	number_of_ifaces = config.ifc_len / (sizeof(struct ifreq));

	for (int i = 0; i < number_of_ifaces; i++) {
		printf("iface: %s\n", ifreq[i].ifr_name);

		/**
		 * Retrieve the address of the specified interface
		 * by making use of SIOCGIFADDR ioctl using the ifreq
		 * of the current interface in the loop as the argument
		 * for the call such that we can retrieve the address for
		 * the right interface.
		 */
		err = ioctl(devices_fd, SIOCGIFADDR, (char*)&ifreq[i]);
		if (err == -1) {
			perror("ioctl failed\n");
			close(devices_fd);
			return 2;
		}

		iface_addr = (struct sockaddr_in*)(&ifreq[i].ifr_addr);
		inet_ntop(AF_INET, &iface_addr->sin_addr, ip_buffer, 16);

		printf("ip: %s\n", ip_buffer);
		printf("\n");
	}

	close(devices_fd);
}
