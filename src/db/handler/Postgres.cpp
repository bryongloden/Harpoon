#include "Postgres.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "utils/Ini.hpp"

#include <iostream>
#include <soci/soci.h>

using namespace std;


namespace Database {

    PROVIDE_MODULE("database", "postgres", Postgres);


    struct Postgres_Impl {
        EventQueue* appQueue;
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);

        shared_ptr<soci::session> sqlSession;

        friend Postgres;
    };

    Postgres::Postgres(EventQueue* appQueue)
        : EventLoop{}
        , impl{make_shared<Postgres_Impl>(appQueue)}
    {
    }

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    bool Postgres_Impl::onEvent(std::shared_ptr<IEvent> event) {
        UUID eventType = event->getEventUuid();
        if (eventType == EventQuit::uuid) {
            return false;
        } else if (eventType == EventInit::uuid) {
            using namespace soci;

            string username, password, database;

            Ini dbIni("config/postgres.ini");
            auto& auth = dbIni.expectCategory("auth");
            dbIni.getEntry(auth, "username", username);
            dbIni.getEntry(auth, "password", password);
            dbIni.getEntry(auth, "database", database);

#warning check if values are 'evil'
            stringstream login;
            login << "postgresql://"
                  << "dbname=" << database << " "
                  << "user=" << username << " "
                  << "password=" << password;

            try {
                sqlSession = make_shared<soci::session>(login.str());
            } catch(soci_error& e) {
                cout << "Could not connect to database server. Reason: " << endl << e.what() << endl << endl;
                return false;
            }
        }
        return true;
    }

}
