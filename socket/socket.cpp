#include "socket.hpp"
#include "../config/src/Config.hpp"

_socket::_socket() : fd(0), port(0), host(""), addrLen(0), max_fd(0)
{
    start_socket();
}

_socket::~_socket()
{
	for(std::vector<int>::iterator it = all_fd.begin(); it != all_fd.end(); it++)
        close(*it);
}

int _socket::max(int x, int y)
{
	if (x > y)
		return x;
	else
		return y;
}

void _socket::creat_socket()
{
	opt = 1;
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1)
        throw "error socket";
    if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        throw "error fcntl";
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
        throw "error setsocketop";
}

void _socket::bind_socket()
{
    std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(port);
    // if(host == "ANY_IP")
    //     serverAddr.sin_addr.s_addr =  htonl(INADDR_ANY);
    // else
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
	if(bind(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        std::cout<< "error fcntl";
}

void _socket::listen_socket()
{
    if(listen(fd, 1024) == -1)
        throw "error listen";
}

void _socket::store_fd()
{
	FD_SET(fd, &struct_fd); 
    max_fd = max(max_fd,fd);
    all_fd.push_back(fd);
}

void _socket::ft_clear(int fd_sock)
{
    close(fd_sock);
    FD_CLR(fd_sock, &struct_fd);
    FD_CLR(fd_sock, &struct_write_fd);
}

void _socket::ft_restart()
{
    for(std::vector<int>::iterator it = all_fd.begin(); it != all_fd.end(); it++)
        close(*it);
    FD_ZERO(&struct_fd);
	FD_ZERO(&struct_read_fd);
	FD_ZERO(&struct_write_fd);
}

void _socket::ft_wait()
{
    int n;

    while (TRUE)
    {
        FD_ZERO(&struct_read_fd); 
	    struct_read_fd = struct_fd;
        tv.tv_sec = 0;
        tv.tv_usec = 10;
        n = select(max_fd + 1, &struct_read_fd, &struct_write_fd, NULL, &tv);            
        if(n == -1)
            throw "error select";
        if(n == 0)
            continue;
        if (FD_ISSET(0, &struct_read_fd))
		{
            ft_restart();
	        break;
        }
        for (int fd_of_socket = 0; fd_of_socket < max_fd + 1; fd_of_socket++)
		{
			if (FD_ISSET(fd_of_socket, &struct_read_fd))
			{
				int created_Connect = 0;
				for (std::vector<int>::iterator it = all_fd.begin(); it != all_fd.end(); it++)
				{
					if (fd_of_socket == *it)
					{ 
						created_Connect = 1;
						break;
					}
				}
                if(created_Connect)
                    _socket::add_conection(fd_of_socket);
                else
                    _socket::ft_accept(fd_of_socket);
			}
		}
    }
}

void _socket::add_conection(int fd_sock)
{
    addrLen = sizeof(clientAddr);
    int accpt = accept(fd_sock, (struct sockaddr *)&clientAddr, &addrLen);
    if(accpt == -1)
        throw "error accept the connection";
    if(fcntl(accpt, F_SETFL, O_NONBLOCK) == -1)
        throw "error set the socket";
    FD_SET(accpt, &struct_fd);
	FD_SET(accpt, &struct_write_fd); 
    max_fd = max(max_fd,accpt);
}

long long int _socket::calculateSizeDone(std::string filename)
{

	FILE* fp = fopen(filename.c_str(), "r");
	if (fp == NULL) {
		return 0;
	}
	fseek(fp, 0L, SEEK_END);
	long int ans = ftell(fp);
	fclose(fp);
	return ans;
}

void _socket::ft_accept(int fd_sock)
{
    std::string		file_name;	
	char 			buff[1024];
    struct timeval  time;
	int retVal;
    gettimeofday(&time, NULL);
    file_name = std::to_string(time.tv_sec) + std::to_string(fd_sock);
	memset(buff ,0 , 1024);
	retVal = recv(fd_sock, buff, 1024, 0);
	if(retVal >= 0)
    {
		int fd_o = open(file_name.c_str(), O_CREAT | O_RDWR |O_APPEND, 0666);
		write(fd_o, buff, retVal);
		if (retVal != 1024 || calculateSizeDone(file_name) == 1024)
		{
			Request _request(file_name, fd_sock);
			if(FD_ISSET(fd_sock, &struct_write_fd))
				Response _response(fd_sock, _request);
			remove(file_name.c_str());
			ft_clear(fd_sock);
		}
    }
}

void _socket::start_socket()
{
    FD_ZERO(&struct_fd);
	FD_ZERO(&struct_write_fd);
	FD_SET(0, &struct_fd);
    max_fd = 0;
    for (int i = 0; i < Config::getInstance().getServerCount(); i++)
    {
        host = Config::getInstance().getServer(i).getHost();
        port = Config::getInstance().getServer(i).getPort();
        try
        {
            this->creat_socket(); 
            this->bind_socket();
            this->listen_socket();
            this->store_fd();
        }
        catch(const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    this->ft_wait();
}