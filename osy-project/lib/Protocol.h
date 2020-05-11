//
// Created by Lukas on 11.05.2020.
//

#ifndef OSY_PROJECT_PROTOCOL_H
#define OSY_PROJECT_PROTOCOL_H

struct Command {
    static constexpr int INCOMING = 11;
    static constexpr const char * S_INCOMING = "I want to sit.";

    static constexpr const char * S_HUNGRY = "I want to eat.";
    static constexpr int HUNGRY = 12;

    static constexpr const char * S_FULL = "I am full and dont want to eat anymore.";
    static constexpr int FULL = 13;

    static constexpr const char * S_LEAVING = "Leaving.";
    static constexpr int LEAVING = 14;
};

struct Answer {
    static constexpr int SIT = 20;
    static constexpr const char * S_SIT = "Sit on char %d.";

    static constexpr int EAT = 21;
    static constexpr const char * S_EAT = "You can eat.";

    static constexpr int FORK = 22;
    static constexpr const char * S_FORK = "Forks have been removed and washed.";
};

struct Error {
    static constexpr int NO_ROOM = 80;
    static constexpr const char * S_NO_ROOM = "There is no room left, we are full.";

    static constexpr int SERVER_ERROR = 81;
    static constexpr const char * S_SERVER_ERROR = "General server error.";

    static constexpr int CLIENT_ERROR = 82;
    static constexpr const char * S_CLIENT_ERROR = "General client error.";
};

#endif //OSY_PROJECT_PROTOCOL_H
