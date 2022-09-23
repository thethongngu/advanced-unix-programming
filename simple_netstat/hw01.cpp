#include <getopt.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <regex.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

#define debug(a) std::cout << #a << " = " << a << std::endl;
#define check() std::cout << "here" << std::endl

std::string a[] = {"abc", "def", "df", "c", "aac"};
int cnta = -1;

struct in_addr get_in_addr(std::string hex) {
	struct in_addr a;
	std::stringstream hex_stream;
	hex_stream << std::hex << hex;
	int b;
	hex_stream >> b;
	a.s_addr = b;

	return a;
}

struct in6_addr get_in6_addr(std::string hex) {
	std::string big = "";
	for(int i = 0; i < 32; i += 8) {
		big.append(hex.substr(i + 6, 2));
		big.append(hex.substr(i + 4, 2));
		big.append(hex.substr(i + 2, 2));
		big.append(hex.substr(i + 0, 2));
	}
	
	struct in6_addr a;
	for(int i = 0; i < 16; i++) {
		std::stringstream hex_stream;
		hex_stream << std::hex << big.substr(i * 2, 2);
		int b;
		hex_stream >> b;
		a.s6_addr[i] = b;
	}

	return a;
}

std::string convert_port(std::string hex) {
	std::stringstream hex_stream;
	int res;
	
	hex_stream << std::hex << hex;
	hex_stream >> res;

	return res == 0 ? "*" : std::to_string(res);
}

bool is_pid(char name[256]) {
	int i = 0;
	while (name[i] != '\0') {
		if (name[i] < '0' || name[i] > '9') return false;
		i++;
	}
	return true;
}

std::string get_program_info(std::string pid) {
	std::string path = "/proc/";
	path.append(pid);
	path.append("/cmdline");

	std::ifstream f(path);
	std::string name = "";

	if (f.is_open()) {
		std::getline(f, name);
		name = name.substr(name.find_last_of('/') + 1);		
	}

	return name;
}

void show_info(std::vector<std::string>& inode_info, std::vector<std::string>& pid_info, 
				std::string path, const std::string& filter_string) {

	if (path.substr(10, 3)[0] == 't') {
		std::cout << "List of TCP connections:" << std::endl;
	} else {
		std::cout << "List of UDP connections:" << std::endl;
	}
	std::cout << std::setw(8) << std::left << "Proto" << 
				 std::setw(30) << std::left << "Local Address" <<
				 std::setw(30) << std::left << "Foreign Address" <<
				 std::setw(40) << std::left << "PID/Program name and arguments" << std::endl;
				 
	std::string paths[2] = {path, path.append("6")};

	for(int k = 0; k < 2; k++) {

		std::ifstream f(paths[k]);
		std::string protocol = paths[k].substr(10, 4);
		std::string line;

		if (f.is_open()) {
			std::getline(f, line);  // header line	
			while (std::getline(f, line)) {

				/* read net info line by line */
				std::stringstream ss(line);

				/* parse string by space */
				std::vector<std::string> tcp_info;
				std::string ele;
				while (ss >> ele) tcp_info.push_back(ele);

				/* get local address */
				int pos = tcp_info[1].find(':');
				std::string local_addr = tcp_info[1].substr(0, pos);
				std::string local_port = tcp_info[1].substr(pos + 1);

				/* get remote address */
				pos = tcp_info[2].find(':');
				std::string remote_addr = tcp_info[2].substr(0, pos);
				std::string remote_port = tcp_info[2].substr(pos + 1);
				std::string inode = tcp_info[9];		
				
				/* get process info */
				std::string pid = "";
				std::string pinfo = "";
				for(int i = 0; i < inode_info.size(); i++) {
					if (inode_info[i] == inode) {
						pid = pid_info[i];
						pinfo = get_program_info(pid);	
						break;
					}
				}
				cnta = (cnta + 1) % 5;
				// debug(ii);
				pinfo = a[cnta];
					
				/* filter string */
				int t;
				regex_t re_buffer;
				regmatch_t pmatch[1024];
				size_t nmatch;

				if ((t = regcomp(&re_buffer, filter_string.c_str(), REG_NOSUB)) == 0) {
					if (regexec(&re_buffer, pinfo.c_str(), 0, NULL, 0) != 0) continue;
				}

				/* convert binary to readable address */
				char buffer[INET6_ADDRSTRLEN];
				if (protocol.size() == 3) { /* v4 */
					struct in_addr local = get_in_addr(local_addr);
					struct in_addr remote = get_in_addr(remote_addr);

					local_addr = inet_ntop(AF_INET, &local, buffer, sizeof(buffer));
					local_port = convert_port(local_port);
					remote_addr = inet_ntop(AF_INET, &remote, buffer, sizeof(buffer));
					remote_port = convert_port(remote_port);
				} else {  /* v6 */
					struct in6_addr local = get_in6_addr(local_addr);
					struct in6_addr remote = get_in6_addr(remote_addr);

					local_addr = inet_ntop(AF_INET6, &local, buffer, sizeof(buffer));
					local_port = convert_port(local_port);
					remote_addr = inet_ntop(AF_INET6, &remote, buffer, sizeof(buffer));
					remote_port = convert_port(remote_port);
				}

				/* print info */
				std::string pp;
				if (pinfo != "") pp = pid.append("/"); pp.append(pinfo);
				local_addr.append(":");   local_addr.append(local_port);
				remote_addr.append(":");  remote_addr.append(remote_port);

				std::cout
				<< std::setw(8) << std::left << protocol 
				<< std::setw(30) << std::left << local_addr
				<< std::setw(30) << std::left << remote_addr
				<< std::setw(40) << std::left << pp
				<< std::endl;
			}
		}
		f.close();	
	}
	std::cout << std::endl;
}


void get_all_pid(std::vector<char*>& pid) {
	DIR *pdir;
	struct dirent *d;

	if ((pdir = opendir("/proc")) != NULL) {
		while ((d = readdir(pdir)) != NULL) {
			if (is_pid(d->d_name)) pid.push_back(d->d_name);
		}
		closedir(pdir);
	}
}

void get_inode_data(std::vector<std::string>& inode_info, std::vector<std::string>& pid_info, 
					const std::vector<char*>& pid) {

	for(int i = 0; i < pid.size(); i++) {

		std::string s = "/proc/";
		s.append(std::string(pid[i]));
		s.append("/fd");
		DIR *pdir;
		struct dirent *d;

		

		if ((pdir = opendir(s.c_str())) != NULL) {
			while ((d = readdir(pdir)) != NULL) {
				if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)	continue;
				
				std::string socket_link = s;
				socket_link.append("/");
				socket_link.append(std::string(d->d_name));
				// debug(socket_link.c_str());

				char sym_link[1024];
				readlink(socket_link.c_str(), sym_link, sizeof(sym_link));
				// debug(sym_link);				
				if (strncmp(sym_link, "socket", 6) == 0 || strncmp(sym_link, "[0000]", 6) == 0) {
					// debug(std::string(sym_link).substr(8, 6));
					inode_info.push_back(std::string(sym_link).substr(8, 6));
					pid_info.push_back(pid[i]);
				}
			}
		}
		closedir(pdir);
	}
}

int get_args(bool& is_tcp, bool& is_udp, std::string& filter_string, int argc, char **argv) {
	struct option long_options[] = {
        {"tcp", optional_argument, 0, 't'},
        {"udp", optional_argument, 0, 'u'}
    };

    is_tcp = is_udp = false;
    int c;
	while ((c = getopt_long(argc, argv, "tu", long_options, 0)) != -1) {
		if (c == 't') is_tcp = true;
		if (c == 'u') is_udp = true;
		if (c == '?') return -1;
	}
	
	if (!is_tcp && !is_udp) is_tcp = is_udp = true;
	for(int i = optind; i < argc; i++) {
		if (i > optind) filter_string.append(" ");
		filter_string.append(argv[i]);
	}

	return 0;
}


std::vector<char*> pid;
std::vector<std::string> inode_info;
std::vector<std::string> pid_info;
int c;
bool is_tcp, is_udp;
std::string filter_string;


int main(int argc, char **argv) {

	get_all_pid(pid);
	get_inode_data(inode_info, pid_info, pid);
	if (get_args(is_tcp, is_udp, filter_string, argc, argv) != -1) {
   		if (is_tcp) show_info(inode_info, pid_info, "/proc/net/tcp", filter_string);
    	if (is_udp) show_info(inode_info, pid_info, "/proc/net/udp", filter_string);
    }
}
