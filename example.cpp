
#include "jsm.h"

#include <iostream>
#include <cassert>


struct User {
	int user_id;
	std::string full_name, email, hash;
	std::vector<int> positions;
	std::map<std::string, std::string> settings;
	bool is_admin;

	JSM_START_MAPPING()
	JSM_POD_MAPPING("user_id", user_id)
	JSM_POD_MAPPING("is_admin", is_admin)
	JSM_POD_MAPPING("hash", hash)
	JSM_POD_MAPPING("full_name", full_name)
	JSM_POD_MAPPING("email", email)
	JSM_MAP_MAPPING("settings", settings)
	JSM_ARR_MAPPING("positions", positions)
	JSM_FINISH_MAPPING()
};


int
main()
{
	using namespace rapidjson;
    const char json[] = R"*({
	
	"user_id": 123456,
	"email": "user@example.com",
	"full_name": "jefferson",
	"positions": [3,4, 17, 640],
	"settings": {
		"enable_sound": "on",
		"enable_video": "yes"
	},
	"is_admin": true,
	"hash": "b667c4c25d5fe8af808f51300fb8e4c4"

})*";

    MessageHandler<User> handler;
    Reader reader;
    StringStream ss(json);
    reader.Parse<64, StringStream, MessageHandler<User>>(ss, handler);

    assert(handler.isParsedClean());

    assert(123456 == handler.msg.user_id);
    assert(handler.msg.full_name == "jefferson");
    assert(handler.msg.email == "user@example.com");
    assert(handler.msg.is_admin);
    assert(handler.msg.hash == "b667c4c25d5fe8af808f51300fb8e4c4");

    assert(handler.msg.positions.size() == 4);
    assert(handler.msg.positions[1] == 4);

    assert(handler.msg.settings.size() == 2);
    assert(handler.msg.settings["enable_video"] == "yes");

    std::cout << "Done\n";

    return 0;
}
