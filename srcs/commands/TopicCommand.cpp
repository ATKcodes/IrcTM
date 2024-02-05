#include "Command.hpp"

TopicCommand::TopicCommand(Server *server, int auth) : Command(server, auth) {}

TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Client *client, std::vector<std::string> args)
{
	Channel	*channel = client->getChannel();
	if (channel == nullp)
	{
		client->msgReply(client->getNickname() + " :You're not on a channel");
		return ;
	}
	std::string old_topic = client->getChannel()->getTopic();
	if (args.empty())
	{
		if (old_topic.empty())
		{
			client->msgReply(RPL_NOTOPIC(client->getNickname(), client->getChannel()->getName()));
			return ;
		}
		client->msgReply("Topic for " + client->getChannel()->getName() + " is :" + client->getChannel()->getTopic());
		return ;
	}

	if (client->getStatus() == 1 && !channel->isTopicOnly()) //bypassing the else if
	{
		channel->setTopicOnly(false);
	}
	else if (client->getStatus() != 2)
	{
		client->msgReply(ERR_NOTADMIN(client->getNickname()));
		return ;
	}
	std::string topic;
	std::vector<std::string>::iterator it = args.begin();
	while (it != args.end())
	{
		topic += *it;
		it++;
		if (it != args.end())
			topic += " ";
	}

	std::string	channelName = channel->getName();

	channel->setTopic(topic);
	client->msgChannel(channel, "TOPIC " + channelName + " :" + topic);
}