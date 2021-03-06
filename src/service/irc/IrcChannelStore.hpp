#ifndef IRCCHANNELSTORE_H
#define IRCCHANNELSTORE_H

#include <map>
#include <string>


class IrcUserStore {
    std::string nick;
    std::string mode;
public:
    IrcUserStore(const std::string& nick, const std::string& mode);
    void setNick(const std::string& nick);
    void setMode(const std::string& mode);
    std::string getNick() const;
    std::string getMode() const;
};

class IrcChannelStore {
    std::string channelPassword;
    std::map<std::string, IrcUserStore> users;
    bool disabled;
public:
    IrcChannelStore(const std::string& channelPassword,
                    bool disabled);

    void clear();
    void addUser(const std::string& nick, const std::string& mode);
    void removeUser(const std::string& nick);
    void renameUser(const std::string& nick, const std::string& newNick);
    const std::map<std::string, IrcUserStore>& getUsers() const;
    std::string getChannelPassword() const;
    bool getDisabled() const;
    void setDisabled(bool ldisabled);
};

#endif
