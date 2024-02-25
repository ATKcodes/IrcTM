#include "Client.hpp"

Client::Client(int fd, std::string hostname, int port)
	: fd(fd), hostname(hostname), port(port), status(0), channel(nullp)
{
}

Client::~Client()
{
}

void	Client::msgReply(std::string const &msg)
{
	if (DEBUG)
		print_time(msg);
	this->reply(":" + this->getPrefix() + " " + msg);
}

void	Client::welcome()
{
	if (!this->status || this->username.empty() || this->realname.empty() || this->nickname.empty())
		return ;
	this->msgReply(REPLYMEET(this->nickname));
	print_time(this->log("is known as " + this->nickname));
}

std::string	Client::identify() const
{
	std::stringstream ss;
	ss << this->getHostname() << ":" << this->getPort();
	return ss.str();
}

void	Client::reply(std::string const &msg) const
{
	if (DEBUG)
		print_time(msg);
	std::string	tmp = msg + "\r\n";
	if (send(this->fd, tmp.c_str(), tmp.length(), 0) < 0)
		throw std::runtime_error("Error while sending");
}

std::string	Client::log(std::string const &log)
{
	std::string	msg;
	char		tmp[1000];

	sprintf(tmp, "%s %s", this->identify().c_str(), log.c_str());
	msg.append(tmp);
	return (msg);
}

void	Client::join(Channel *channel)
{
	std::string					users_string;
	std::vector<std::string>	users;

	channel->addClient((this));
	this->channel = channel;
	users = channel->getNicknames();
	for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
		users_string.append((*it)).append(" ");
	
	this->msgReply(REPLYNAMEDMESSAGE(this->nickname, channel->getName(), users_string));
	this->msgReply(REPLYLASTNAME(this->nickname, channel->getName()));
	
	channel->broadcast(REPLYCMDJOIN(this->getPrefix(), channel->getName()));
	print_time(this->nickname + " has joined channel " + channel->getName());
}

void	Client::leave()
{
	if (this->channel == nullp)
		return ;
	this->channel->broadcast(REPLYCMDPART(this->getPrefix(), this->channel->getName()));
	print_time(this->nickname + " has left channel " + channel->getName());
	this->channel->deleteclient((this));
}

std::string	Client::getPrefix() const
{
	std::string	tmp;

	tmp = this->nickname;
	if (!this->username.empty())
		tmp.append("!").append(this->username);
	if (!this->hostname.empty())
		tmp.append("@").append(this->hostname);
	return (tmp);
}

void	Client::invite(Client *client, Channel *channel)
{
	this->msgReply(REPLYINVITING(client->getNickname(), this->nickname, channel->getName()));
	channel->invite(this, client);
}

void Client::msgChannel(Channel *channel, std::string const &msg)
{
	std::vector<Client *>	clients = channel->getClients();

	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it) == this)
			continue ;
		(*it)->msgReply(msg);
	}
}