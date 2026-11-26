;// Romanian translation by 2011-2019 Ștefan Fulea (stefan.fulea@mail.com)
;// Romanian translation by 2023-2024 Andrei Miloiu (miloiuandrei@gmail.com)
;// Russian translation by Kudratov Olimjon (olim98@bk.ru)
;// Spanish translation by Ismael Ferreras Morezuelas <2014-11-07>
;// Turkish translation by 2015 Erdem Ersoy (eersoy93) (erdemersoy [at] erdemersoy [dot] net)
;// Simplified Chinese translation by Henry Tang Ih 2016 (henrytang2@hotmail.com)
;// Traditional Chinese translation by Henry Tang Ih 2016 (henrytang2@hotmail.com)
;// Polish translation Updated by pithwz - Piotr Hetnarowicz (piotrhwz@gmail.com) (April, 2020)

MessageIdTypedef=DWORD

SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )

FacilityNames=(System=0x0:FACILITY_SYSTEM
              )

LanguageNames=(English=0x409:MSG00409
               Chinese=0x804:MSG00804)


MessageId=10000
SymbolicName=MSG_ACCOUNTS_SYNTAX
Severity=Success
Facility=System
Language=English
NET ACCOUNTS [/FORCELOGOFF:{Minutes|NO}] [/MINPWLEN:Length]
             [/MAXPWAGE:{Days|UNLIMITED}] [/MINPWAGE:Days]
             [/UNIQUEPW:Count] [/DOMAIN]
.
Language=Chinese
NET ACCOUNTS [/FORCELOGOFF:{Minutes|NO}] [/MINPWLEN:Length]
             [/MAXPWAGE:{Days|UNLIMITED}] [/MINPWAGE:Days]
             [/UNIQUEPW:Count] [/DOMAIN]
.


MessageId=10001
SymbolicName=MSG_ACCOUNTS_HELP
Severity=Success
Facility=System
Language=English
NET ACCOUNTS updates the user accounts database and modifies password
and logon requirements for all accounts.
When used without parameters, NET ACCOUNTS displays the current settings for
passwords, logon limitations, and domain information.

Two conditions are required in order for options used with
NET ACCOUNTS to take effect.

- The password and logon requirements are only effective if user
  accounts have been set up (user User Manager or the NET USER command).

- The Net Logon service must be running on all servers in the domain
  that verify logon. Net Logon is started automatically when the
  operating system starts.

/FORECELOGOFF:{minutes | NO}   Sets the number of minutes a user has before
                               being forced to log off when the
                               account expires or valid logon hours expire.
                               NO, the default, prevents forced logoff.
/MINPWLEN:length               Sets the minimum number of characters for
                               a password. The range is 0-14 characters;
                               the default is 6 characters.
/MAXPWAGE:{days | UNLIMITED}   Sets the maximum numer of days that a
                               password is valid. No limit is specified
                               by using UNLIMITED. /MAXPWAGE cannot be less
                               than /MINPWAGE. The range is 1-999; the
                               default is to leave the value unchanged.
/MINPWAGE:days                 Sets the minimum number of days that must
                               pass before a user can change a password.
                               A value of 0 sets no minimum time. The range
                               is 0-999; the default is 0 days. /MINPWAGE
                               cannot be more than /MAXPWAGE.
/UNIQUEPW:number               Requires that a users passwords be unique
                               through the specified number of password
                               changes. The maximum value is 24.
/DOMAIN                        Performs the operation on a domain
                               controller of the current domain. Otherwise,
                               the operation is performed on the local
                               computer.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET ACCOUNTS updates the user accounts database and modifies password
and logon requirements for all accounts.
When used without parameters, NET ACCOUNTS displays the current settings for
passwords, logon limitations, and domain information.

Two conditions are required in order for options used with
NET ACCOUNTS to take effect.

- The password and logon requirements are only effective if user
  accounts havebeen set up (user User Manager or the NET USER command).

- The Net Logon service must be running on all servers in the domain
  that verify logon. Net Logon is started automatically when the
  operating system starts.

/FORECELOGOFF:{minutes | NO}   Sets the number of minutes a user has before
                               being forced to log off when the
                               account expires or valid logon hours expire.
                               NO, the default, prevents forced logoff.
/MINPWLEN:length               Sets the minimum number of characters for
                               a password. The range is 0-14 characters;
                               the default is 6 characters.
/MAXPWAGE:{days | UNLIMITED}   Sets the maximum numer of days that a
                               password is valid. No limit is specified
                               by using UNLIMITED. /MAXPWAGE cannot be less
                               than /MINPWAGE. The range is 1-999; the
                               default is to leave the value unchanged.
/MINPWAGE:days                 Sets the minimum number of days that must
                               pass before a user can change a password.
                               A value of 0 sets no minimum time. The range
                               is 0-999; the default is 0 days. /MINPWAGE
                               cannot be more than /MAXPWAGE.
/UNIQUEPW:number               Requires that a users passwords be unique
                               through the specified number of password
                               changes. The maximum value is 24.
/DOMAIN                        Performs the operation on a domain
                               controller of the current domain. Otherwise,
                               the operation is performed on the local
                               computer.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10002
SymbolicName=MSG_COMPUTER_SYNTAX
Severity=Success
Facility=System
Language=English
NET COMPUTER \\computername {/ADD | /DEL}
.
Language=Chinese
NET COMPUTER \\computername {/ADD | /DEL}
.


MessageId=10003
SymbolicName=MSG_COMPUTER_HELP
Severity=Success
Facility=System
Language=English
NET COMPUTER adds or deletes computers from a domain database. This
command is available only on server operating systems.

\\computername   Specifies the computer to add or delete from
                 the domain.
/ADD             Adds the specified computer to the domain.
/DEL             Removes the specified computer from the domain.
.
Language=Chinese
NET COMPUTER adds or deletes computers from a domain database. This
command is available only on server operating systems.

\\computername   Specifies the computer to add or delete from
                 the domain.
/ADD             Adds the specified computer to the domain.
/DEL             Removes the specified computer from the domain.
.


MessageId=10004
SymbolicName=MSG_CONFIG_SYNTAX
Severity=Success
Facility=System
Language=English
NET CONFIG [SERVER | WORKSTATION]
.
Language=Chinese
NET CONFIG [SERVER | WORKSTATION]
.


MessageId=10005
SymbolicName=MSG_CONFIG_HELP
Severity=Success
Facility=System
Language=English
NET CONFIG displays coniguration information of the Workstation or
Server service. When used without the WORKSTATION or SERVER switch,
it displays a list of configurable services. To get help with
configuring a service, type NET HELP CONFIG service.

SERVER        Displays information about the configuration of the
              Server service.
WORKSTATION   Displays information about the configuration of the
              Workstation service.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET CONFIG displays coniguration information of the Workstation or
Server service. When used without the WORKSTATION or SERVER switch,
it displays a list of configurable services. To get help with
configuring a service, type NET HELP CONFIG service.

SERVER        Displays information about the configuration of the
              Server service.
WORKSTATION   Displays information about the configuration of the
              Workstation service.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10006
SymbolicName=MSG_CONFIG_SERVER_SYNTAX
Severity=Success
Facility=System
Language=English
NET CONFIG SERVER [/AUTODISCONNECT:time] [/SRVCOMMENT:"text"]
                  [/HIDDEN:{YES | NO}]
.
Language=Chinese
NET CONFIG SERVER [/AUTODISCONNECT:time] [/SRVCOMMENT:"text"]
                  [/HIDDEN:{YES | NO}]
.


MessageId=10007
SymbolicName=MSG_CONFIG_SERVER_HELP
Severity=Success
Facility=System
Language=English
NET CONFIG SERVER displays or changes settings for the Server service.

/AUTODISCONNECT:time    Sets the maximum number of minutes a user's
                        session can be inactive before it is disconected.
                        You can specify -1 to never disconnect. The range
                        is -1-65535 minutes; the default is 15.
/SRVCOMMENT:"text"      Adds a comment for the server that is displayed on
                        screen and with the NET VIEW command.
                        Enclose the text in quotation marks.
/HIDDEN:{YES | NO}      Specifies whether the server's computer name
                        appears on displays listings of servers. Note that
                        hiding a serverdoes not alter the permissions
                        on that server. The default is NO.

To display the current configuration for the Server service,
type NET CONFIG SERVER without parameters.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET CONFIG SERVER displays or changes settings for the Server service.

/AUTODISCONNECT:time    Sets the maximum number of minutes a user's
                        session can be inactive before it is disconected.
                        You can specify -1 to never disconnect. The range
                        is -1-65535 minutes; the default is 15.
/SRVCOMMENT:"text"      Adds a comment for the server that is displayed on
                        screen and with the NET VIEW command.
                        Enclose the text in quotation marks.
/HIDDEN:{YES | NO}      Specifies whether the server's computer name
                        appears on displays listings of servers. Note that
                        hiding a serverdoes not alter the permissions
                        on that server. The default is NO.

To display the current configuration for the Server service,
type NET CONFIG SERVER without parameters.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10008
SymbolicName=MSG_CONTINUE_SYNTAX
Severity=Success
Facility=System
Language=English
NET CONTINUE service
.
Language=Chinese
NET CONTINUE <Service Name>
.


MessageId=10009
SymbolicName=MSG_CONTINUE_HELP
Severity=Success
Facility=System
Language=English
NET CONTINUE reactivates a service that has been suspended by NET PAUSE.

service             Is the paused service
                    For example, one of the following:
                    NET LOGON
                    NT LM SECURITY SUPPORT PROVIDER
                    SCHEDULE
                    SERVER
                    WORKSTATION

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET CONTINUE reactivates a service that has been suspended by NET PAUSE.

service             Is the paused service
                    For example, one of the following:
                    NET LOGON
                    NT LM SECURITY SUPPORT PROVIDER
                    SCHEDULE
                    SERVER
                    WORKSTATION

NET HELP command | MORE displays one screen at a time.
.


MessageId=10010
SymbolicName=MSG_FILE_SYNTAX
Severity=Success
Facility=System
Language=English
NET FILE [id [/CLOSE]]
.
Language=Chinese
NET FILE [id [/CLOSE]]
.


MessageId=10011
SymbolicName=MSG_FILE_HELP
Severity=Success
Facility=System
Language=English
NET FILE
...
.
..
.
..
.
..
.
..
.
..
.
Language=Chinese
NET FILE
...
.
..
.


MessageId=10012
SymbolicName=MSG_GROUP_SYNTAX
Severity=Success
Facility=System
Language=English
NET GROUP [groupname [/COMMENT:"text"]] [/DOMAIN]
          groupname {/ADD [/COMMENT:"text"] | /DELETE} [/DOMAIN]
          groupname username [...] {/ADD | /DELETE} [/DOMAIN]
.
Language=Chinese
NET GROUP [groupname [/COMMENT:"text"]] [/DOMAIN]
          groupname {/ADD [/COMMENT:"text"] | /DELETE} [/DOMAIN]
          groupname username [...] {/ADD | /DELETE} [/DOMAIN]
.


MessageId=10013
SymbolicName=MSG_GROUP_HELP
Severity=Success
Facility=System
Language=English
NET GROUP adds, displays or modifies global groups on servers. When
used without parameters, it displays the groupnames on the server.

groupname        Is the name of the group to add, expand, or delete.
                 Supply only a groupname to view a list of users
                 in a group.
/COMMENT:"text"  Adds a comment for a new or existing group.
                 Enclose the text in quotation marks.
/DOMAIN          Performs the operation on a domain controller
                 of the current domain. Otherwise, the operation is
                 performed on the local computer.
username[ ...]   List one or more usernames to add to or remove from
                 a group. Separate multiple username entries with a space.
/ADD             Adds a group, or adds a username to a group.
/DELETE          Removes a group, or removes a username from a group.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET GROUP adds, displays or modifies global groups on servers. When
used without parameters, it displays the groupnames on the server.

groupname        Is the name of the group to add, expand, or delete.
                 Supply only a groupname to view a list of users
                 in a group.
/COMMENT:"text"  Adds a comment for a new or existing group.
                 Enclose the text in quotation marks.
/DOMAIN          Performs the operation on a domain controller
                 of the current domain. Otherwise, the operation is
                 performed on the local computer.
username[ ...]   List one or more usernames to add to or remove from
                 a group. Separate multiple username entries with a space.
/ADD             Adds a group, or adds a username to a group.
/DELETE          Removes a group, or removes a username from a group.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10014
SymbolicName=MSG_HELP_SYNTAX
Severity=Success
Facility=System
Language=English
NET HELP command
   - or -
NET command /HELP
.
Language=Chinese
NET HELP <Command>
   - or -
NET <Command> /HELP
.


MessageId=10015
SymbolicName=MSG_HELP_HELP
Severity=Success
Facility=System
Language=English
   The following commands are available:

   NET ACCOUNTS             NET HELP                 NET STOP
   NET COMPUTER             NET HELPMSG              NET TIME
   NET CONFIG               NET LOCALGROUP           NET USE
   NET CONFIG SERVER        NET PAUSE                NET USER
   NET CONFIG WORKSTATION   NET SESSION              NET VIEW
   NET CONTINUE             NET SHARE
   NET FILE                 NET START
   NET GROUP                NET STATISTCS

   NET HELP SYNTAX explains how to read NET HELP syntax lines.
   NET HELP command | MORE displays Help one screen at a time.
.
Language=Chinese
   以下命令可用:

   NET ACCOUNTS             NET HELP                 NET STOP
   NET COMPUTER             NET HELPMSG              NET TIME
   NET CONFIG               NET LOCALGROUP           NET USE
   NET CONFIG SERVER        NET PAUSE                NET USER
   NET CONFIG WORKSTATION   NET SESSION              NET VIEW
   NET CONTINUE             NET SHARE
   NET FILE                 NET START
   NET GROUP                NET STATISTCS

   NET HELP SYNTAX explains how to read NET HELP syntax lines.
   NET HELP command | MORE displays Help one screen at a time.
.


MessageId=10016
SymbolicName=MSG_HELPMSG_SYNTAX
Severity=Success
Facility=System
Language=English
NET HELPMSG message#
.
Language=Chinese
NET HELPMSG message#
.


MessageId=10017
SymbolicName=MSG_HELPMSG_HELP
Severity=Success
Facility=System
Language=English
NET HELPMSG displays information about network messages (such as
error, warning and alert messages). When you type NET HELPMSG and the numeric
error (for example, "net helpmsg 2182"), you will get information about the
message and suggested actions you can take to solve the problem.

message#   Is the numerical error with which you need help.
.
Language=Chinese
NET HELPMSG displays information about network messages (such as
error, warning and alert messages). When you type NET HELPMSG and the numeric
error (for example, "net helpmsg 2182"), you will get information about the
message and suggested actions you can take to solve the problem.

message#   Is the numerical error with which you need help.
.


MessageId=10018
SymbolicName=MSG_LOCALGROUP_SYNTAX
Severity=Success
Facility=System
Language=English
NET LOCALGROUP [groupname [/COMMENT:"text"]] [/DOMAIN]
               groupname {/ADD [/COMMENT:"text"] | /DELETE} [/DOMAIN]
               groupname name [...] {/ADD | /DELETE} [/DOMAIN]
.
Language=Chinese
NET LOCALGROUP [groupname [/COMMENT:"text"]] [/DOMAIN]
               groupname {/ADD [/COMMENT:"text"] | /DELETE} [/DOMAIN]
               groupname name [...] {/ADD | /DELETE} [/DOMAIN]
.


MessageId=10019
SymbolicName=MSG_LOCALGROUP_HELP
Severity=Success
Facility=System
Language=English
NET LOCALGROUP adds, displays or modifies local groups on computers. When
used without parameters, it displays the local groups on the computer.

groupname        Is the name of the local group to add, expand, or
                 delete. Supply only a groupname to view a list of
                 users or global groups in a local group.
/COMMENT:"text"  Adds a comment for a new or existing group.
                 Enclose the text in quotation marks.
/DOMAIN          Performs the operation on a domain controller
                 of the current domain. Otherwise, the operation is
                 performed on the local computer.
name[ ...]       List one or more usernames or groupnams to add to or
                 remove from a local group. Separate multiple entries with
                 a space. Names may be users or global groups, but not
                 other local groups. If a user is from another doamin,
                 preface the username with the domain name (for
                 example, SALES\RALPHR).
/ADD             Adds a groupname or username to a local group. An account
                 must be established for users or global groups added to a
                 local group with this command.
/DELETE          Removes a groupname or username from a local group.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET LOCALGROUP adds, displays or modifies local groups on computers. When
used without parameters, it displays the local groups on the computer.

groupname        Is the name of the local group to add, expand, or
                 delete. Supply only a groupname to view a list of
                 users or global groups in a local group.
/COMMENT:"text"  Adds a comment for a new or existing group.
                 Enclose the text in quotation marks.
/DOMAIN          Performs the operation on a domain controller
                 of the current domain. Otherwise, the operation is
                 performed on the local computer.
name[ ...]       List one or more usernames or groupnams to add to or
                 remove from a local group. Separate multiple entries with
                 a space. Names may be users or global groups, but not
                 other local groups. If a user is from another doamin,
                 preface the username with the domain name (for
                 example, SALES\RALPHR).
/ADD             Adds a groupname or username to a local group. An account
                 must be established for users or global groups added to a
                 local group with this command.
/DELETE          Removes a groupname or username from a local group.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10020
SymbolicName=MSG_PAUSE_SYNTAX
Severity=Success
Facility=System
Language=English
NET PAUSE service
.
Language=Chinese
NET PAUSE service
.


MessageId=10021
SymbolicName=MSG_PAUSE_HELP
Severity=Success
Facility=System
Language=English
NET PAUSE suspends a service. Pausing a service puts it on hold.

service   The name of the service to be paused.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET PAUSE suspends a service. Pausing a service puts it on hold.

service   The name of the service to be paused.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10022
SymbolicName=MSG_SESSION_SYNTAX
Severity=Success
Facility=System
Language=English
NET SESSION ...
.
Language=Chinese
NET SESSION ...
.


MessageId=10023
SymbolicName=MSG_SESSION_HELP
Severity=Success
Facility=System
Language=English
SESSION
...
.
..
.
..
.
..
.
..
.
..
.
Language=Chinese
SESSION
...
.
..
.


MessageId=10024
SymbolicName=MSG_SHARE_SYNTAX
Severity=Success
Facility=System
Language=English
NET SHARE sharename=drive:path [/GRANT:user,[READ | CHANGE | FULL]]
                               [/USERS:number | /UNLIMITED]
                               [/REMARK:"text"]
                               [/CACHE:Manual | Documents| Programs | None]
          sharename [/USERS:number | /UNLIMITED]
                    [/REMARK:"text"]
                    [/CACHE:Manual | Documents | Programs | None]
          {sharename | devicename | drive:path} /DELETE
.
Language=Chinese
NET SHARE sharename=drive:path [/GRANT:user,[READ | CHANGE | FULL]]
                               [/USERS:number | /UNLIMITED]
                               [/REMARK:"text"]
                               [/CACHE:Manual | Documents| Programs | None]
          sharename [/USERS:number | /UNLIMITED]
                    [/REMARK:"text"]
                    [/CACHE:Manual | Documents | Programs | None]
          {sharename | devicename | drive:path} /DELETE
.


MessageId=10025
SymbolicName=MSG_SHARE_HELP
Severity=Success
Facility=System
Language=English
NET SHARE makes a server's resources available to network users. When
used without options, it lists information about all resources being
shared on the computer. For each resource, Windows reports the
devicename(s) or pathname(s) and a descriptive comment associated with it.

sharename          Is the network name of the shared resource. Type
                   NET SHARE with a sharename only to display information
                   about that share.
drive:path         Specifies the absolute path of the directory to
                   be shared.
/GRANT:user,perm   Creates the share with a security descriptor that gives
                   the requested permissions to the specified user. This
                   option may be used more than once to give share permissions
                   to multiple users.
/USERS:number      Sets the maximum number of users who can
                   simultaneously access the shared resource.
/UNLIMITED         Specifies an unlimited number of users can
                   simultaneously access the shared resource.
/REMARK:"text"     Adds a descriptive comment about the resource.
                   Enclose the text in quotation marks.
devicename         Is one or more printers (LPT1: through LPT9:)
                   shared by sharename.
/DELETE            Stops sharing the resource.
/CACHE:Manual      Enables manual client caching of programs and documents
                   from this share.
/CACHE:Documents   Enables automatic caching of documents from this share.
/CACHE:Programs    Enables automatic caching of documents and programs
                   from this share.
/CACHE:None        Disables caching from this share.

NET HELP command | MORE displays Help one screen at a time.
.
Language=Chinese
NET SHARE makes a server's resources available to network users. When
used without options, it lists information about all resources being
shared on the computer. For each resource, Windows reports the
devicename(s) or pathname(s) and a descriptive comment associated with it.

sharename          Is the network name of the shared resource. Type
                   NET SHARE with a sharename only to display information
                   about that share.
drive:path         Specifies the absolute path of the directory to
                   be shared.
/GRANT:user,perm   Creates the share with a security descriptor that gives
                   the requested permissions to the specified user. This
                   option may be used more than once to give share permissions
                   to multiple users.
/USERS:number      Sets the maximum number of users who can
                   simultaneously access the shared resource.
/UNLIMITED         Specifies an unlimited number of users can
                   simultaneously access the shared resource.
/REMARK:"text"     Adds a descriptive comment about the resource.
                   Enclose the text in quotation marks.
devicename         Is one or more printers (LPT1: through LPT9:)
                   shared by sharename.
/DELETE            Stops sharing the resource.
/CACHE:Manual      Enables manual client caching of programs and documents
                   from this share.
/CACHE:Documents   Enables automatic caching of documents from this share.
/CACHE:Programs    Enables automatic caching of documents and programs
                   from this share.
/CACHE:None        Disables caching from this share.

NET HELP command | MORE displays Help one screen at a time.
.


MessageId=10026
SymbolicName=MSG_START_SYNTAX
Severity=Success
Facility=System
Language=English
NET START [service]
.
Language=Chinese
NET START [service]
.


MessageId=10027
SymbolicName=MSG_START_HELP
Severity=Success
Facility=System
Language=English
NET START starts services or lists running services.

service   The name of the service to be started.

When typed at the command prompt, service name of two words or more must
be enclosed in quotation marks. For example, NET START "NET LOGON"
starts the net logon service.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET START starts services or lists running services.

service   The name of the service to be started.

When typed at the command prompt, service name of two words or more must
be enclosed in quotation marks. For example, NET START "NET LOGON"
starts the net logon service.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10028
SymbolicName=MSG_STATISTICS_SYNTAX
Severity=Success
Facility=System
Language=English
NET STATISTICS [SERVER | WORKSTATION]
.
Language=Chinese
NET STATISTICS [SERVER | WORKSTATION]
.


MessageId=10029
SymbolicName=MSG_STATISTICS_HELP
Severity=Success
Facility=System
Language=English
NET STATISTICS displays the statisticslog for the local Workstation or
Server service. Used without parameters, NET STATISTICS displays
the services for which statistics are available.

SERVER        Displays the Server service statistics.
WORKSTATION   Displays the Workstation service statistics.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET STATISTICS displays the statisticslog for the local Workstation or
Server service. Used without parameters, NET STATISTICS displays
the services for which statistics are available.

SERVER        Displays the Server service statistics.
WORKSTATION   Displays the Workstation service statistics.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10030
SymbolicName=MSG_STOP_SYNTAX
Severity=Success
Facility=System
Language=English
NET STOP service
.
Language=Chinese
NET STOP service
.


MessageId=10031
SymbolicName=MSG_STOP_HELP
Severity=Success
Facility=System
Language=English
NET STOP stops services.

service   The name of the service to be stopped.

Stopping a service cancels any network connection the service is
using. Also, some services are dependent on others. Stopping one
service can stop others. Some services cannot be stopped.

NET HELP command | MORE displays one screen at a time.
.
Language=Chinese
NET STOP stops services.

service   The name of the service to be stopped.

Stopping a service cancels any network connection the service is
using. Also, some services are dependent on others. Stopping one
service can stop others. Some services cannot be stopped.

NET HELP command | MORE displays one screen at a time.
.


MessageId=10032
SymbolicName=MSG_TIME_SYNTAX
Severity=Success
Facility=System
Language=English
NET TIME ...
.
Language=Chinese
NET TIME ...
.


MessageId=10033
SymbolicName=MSG_TIME_HELP
Severity=Success
Facility=System
Language=English
TIME
...
.
..
.
..
.
..
.
..
.
..
.
Language=Chinese
TIME
...
.
..
.


MessageId=10034
SymbolicName=MSG_USE_SYNTAX
Severity=Success
Facility=System
Language=English
NET USE ...
.
Language=Chinese
NET USE ...
.


MessageId=10035
SymbolicName=MSG_USE_HELP
Severity=Success
Facility=System
Language=English
USE
...
.
..
.
..
.
..
.
..
.
..
.
Language=Chinese
USE
...
.
..
.


MessageId=10036
SymbolicName=MSG_USER_SYNTAX
Severity=Success
Facility=System
Language=English
NET USER [username [password | *] [options]] [/DOMAIN]
         username {password | *} /ADD [options] [/DOMAIN]
         username [/DELETE] [/DOMAIN]
.
Language=Chinese
NET USER [username [password | *] [options]] [/DOMAIN]
         username {password | *} /ADD [options] [/DOMAIN]
         username [/DELETE] [/DOMAIN]
.


MessageId=10037
SymbolicName=MSG_USER_HELP
Severity=Success
Facility=System
Language=English
NET USER creates and modifies user accounts on computers. When used
without switches, it lists the user accounts for the computer. The
user account information is stored in the user accounts database.

username     Is the name of the user account to add, delete, modify, or
             view. The name of the user account can have as many as
             20 characters.
password     Assigns or changes a password for the user's account.
             A password must satisfy the minimum length set with the
             /MINPWLEN option of the NET ACCOUNTS command. It can have as
             many as 14 characters.
*            Produces a prompt for the password. The password is not
             displayed when you type it at a password prompt.
/DOMAIN      Performs the operation on a domain controller of
             the current domain.
/ADD         Adds a user account to the user accounts database.
/DELETE      Removes a user account from the user accounts database.

Options      Are as follows:

   Options                    Description
   --------------------------------------------------------------------
   /ACTIVE:{YES | NO}         Activates or deactivates the account. If
                              the account is not active, the user cannot
                              access the server. The default is YES.
   /COMMENT:"text"            Provides a descriptive comment about the
                              user's account.  Enclose the text in
                              quotation marks.
   /COUNTRYCODE:nnn           Uses the operating system country code to
                              implement the specified language files for a
                              user's help and error messages. A value of
                              0 signifies the default country code.
   /EXPIRES:{date | NEVER}    Causes the account to expire if date is
                              set. NEVER sets no time limit on the
                              account. An expiration date is in the
                              form mm/dd/yy(yy). Months can be a number,
                              spelled out, or abbreviated with three
                              letters. Year can be two or four numbers.
                              Use slashes(/) (no spaces) to separate
                              parts of the date.
   /FULLNAME:"name"           Is a user's full name (rather than a
                              username). Enclose the name in quotation
                              marks.
   /HOMEDIR:pathname          Sets the path for the user's home directory.
                              The path must exist.
   /PASSWORDCHG:{YES | NO}    Specifies whether users can change their
                              own password. The default is YES.
   /PASSWORDREQ:{YES | NO}    Specifies whether a user account must have
                              a password. The default is YES.
   /PROFILEPATH[:path]        Sets a path for the user's logon profile.
   /SCRIPTPATH:pathname       Is the location of the user's logon
                              script.
   /TIMES:{times | ALL}       Is the logon hours. TIMES is expressed as
                              day[-day][,day[-day]],time[-time][,time
                              [-time]], limited to 1-hour increments.
                              Days can be spelled out or abbreviated.
                              Hours can be 12- or 24-hour notation. For
                              12-hour notation, use am, pm, a.m., or
                              p.m. ALL means a user can always log on,
                              and a blank value means a user can never
                              log on. Separate day and time entries with
                              a comma, and separate multiple day and time
                              entries with a semicolon.
   /USERCOMMENT:"text"        Lets an administrator add or change the User
                              Comment for the account.
   /WORKSTATIONS:{computername[,...] | *}
                              Lists as many as eight computers from
                              which a user can log on to the network. If
                              /WORKSTATIONS has no list or if the list is *,
                              the user can log on from any computer.

NET HELP command | MORE displays Help one screen at a time.
.
Language=Chinese
NET USER creates and modifies user accounts on computers. When used
without switches, it lists the user accounts for the computer. The
user account information is stored in the user accounts database.

username     Is the name of the user account to add, delete, modify, or
             view. The name of the user account can have as many as
             20 characters.
password     Assigns or changes a password for the user's account.
             A password must satisfy the minimum length set with the
             /MINPWLEN option of the NET ACCOUNTS command. It can have as
             many as 14 characters.
*            Produces a prompt for the password. The password is not
             displayed when you type it at a password prompt.
/DOMAIN      Performs the operation on a domain controller of
             the current domain.
/ADD         Adds a user account to the user accounts database.
/DELETE      Removes a user account from the user accounts database.

Options      Are as follows:

   Options                    Description
   --------------------------------------------------------------------
   /ACTIVE:{YES | NO}         Activates or deactivates the account. If
                              the account is not active, the user cannot
                              access the server. The default is YES.
   /COMMENT:"text"            Provides a descriptive comment about the
                              user's account.  Enclose the text in
                              quotation marks.
   /COUNTRYCODE:nnn           Uses the operating system country code to
                              implement the specified language files for a
                              user's help and error messages. A value of
                              0 signifies the default country code.
   /EXPIRES:{date | NEVER}    Causes the account to expire if date is
                              set. NEVER sets no time limit on the
                              account. An expiration date is in the
                              form mm/dd/yy(yy). Months can be a number,
                              spelled out, or abbreviated with three
                              letters. Year can be two or four numbers.
                              Use slashes(/) (no spaces) to separate
                              parts of the date.
   /FULLNAME:"name"           Is a user's full name (rather than a
                              username). Enclose the name in quotation
                              marks.
   /HOMEDIR:pathname          Sets the path for the user's home directory.
                              The path must exist.
   /PASSWORDCHG:{YES | NO}    Specifies whether users can change their
                              own password. The default is YES.
   /PASSWORDREQ:{YES | NO}    Specifies whether a user account must have
                              a password. The default is YES.
   /PROFILEPATH[:path]        Sets a path for the user's logon profile.
   /SCRIPTPATH:pathname       Is the location of the user's logon
                              script.
   /TIMES:{times | ALL}       Is the logon hours. TIMES is expressed as
                              day[-day][,day[-day]],time[-time][,time
                              [-time]], limited to 1-hour increments.
                              Days can be spelled out or abbreviated.
                              Hours can be 12- or 24-hour notation. For
                              12-hour notation, use am, pm, a.m., or
                              p.m. ALL means a user can always log on,
                              and a blank value means a user can never
                              log on. Separate day and time entries with
                              a comma, and separate multiple day and time
                              entries with a semicolon.
   /USERCOMMENT:"text"        Lets an administrator add or change the User
                              Comment for the account.
   /WORKSTATIONS:{computername[,...] | *}
                              Lists as many as eight computers from
                              which a user can log on to the network. If
                              /WORKSTATIONS has no list or if the list is *,
                              the user can log on from any computer.

NET HELP command | MORE displays Help one screen at a time.
.


MessageId=10038
SymbolicName=MSG_VIEW_SYNTAX
Severity=Success
Facility=System
Language=English
NET VIEW ...
.
Language=Chinese
NET VIEW ...
.


MessageId=10039
SymbolicName=MSG_VIEW_HELP
Severity=Success
Facility=System
Language=English
VIEW
...
.
..
.
..
.
..
.
..
.
..
.
Language=Chinese
VIEW
...
.
..
.


MessageId=10040
SymbolicName=MSG_NET_SYNTAX
Severity=Success
Facility=System
Language=English
NET [ ACCOUNTS | COMPUTER | CONFIG | CONTINUE | FILE | GROUP | HELP |
      HELPMSG | LOCALGROUP | PAUSE | SESSION | SHARE | START |
      STATISTICS | STOP | TIME | USE | USER | VIEW ]
.
Language=Chinese
NET [ ACCOUNTS | COMPUTER | CONFIG | CONTINUE | FILE | GROUP | HELP |
      HELPMSG | LOCALGROUP | PAUSE | SESSION | SHARE | START |
      STATISTICS | STOP | TIME | USE | USER | VIEW ]
.


MessageId=10041
SymbolicName=MSG_SYNTAX_HELP
Severity=Success
Facility=System
Language=English
SYNTAX
The following conventions are used to indicate command syntax:

-  Capital letters represent words that must be typed as shown. Lower-
   case letters represent names of items that may vary, such as filenames.

-  The [ and ] characters surround optional items that can be supplied
   with the command.

-  The { and } characters surround lists of items. You must supply one
   of the items with the command.

-  The | character separates items in a list. Only one of the items can
   be supplied with the command.

   For example, in the following syntax, you must type NET COMMAND and
   either SWITCH1 or SWITCH2. Supplying a name is optional.
       NET COMMAND [name] {SWITCH1 | SWITCH2}

-  The [...] characters mean you can repeat the previous item.
   Separate items with spaces.

-  The [,...] characters mean you can repeat the previous item, but
   you must separate items with commas or semicolons, not spaces.

-  When typed at the command prompt, service names of two words or
   more must be enclosed in quotation marks. For example,
   NET START "COMPUTER BROWSER" starts the computer browser service.
.
Language=Chinese
SYNTAX
The following conventions are used to indicate command syntax:

-  Capital letters represent words that must be typed as shown. Lower-
   case letters represent names of items that may vary, such as filenames.

-  The [ and ] characters surround optional items that can be supplied
   with the command.

-  The { and } characters surround lists of items. You must supply one
   of the items with the command.

-  The | character separates items in a list. Only one of the items can
   be supplied with the command.

   For example, in the following syntax, you must type NET COMMAND and
   either SWITCH1 or SWITCH2. Supplying a name is optional.
       NET COMMAND [name] {SWITCH1 | SWITCH2}

-  The [...] characters mean you can repeat the previous item.
   Separate items with spaces.

-  The [,...] characters mean you can repeat the previous item, but
   you must separate items with commas or semicolons, not spaces.

-  When typed at the command prompt, service names of two words or
   more must be enclosed in quotation marks. For example,
   NET START "COMPUTER BROWSER" starts the computer browser service.
.
