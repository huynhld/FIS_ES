#include "synoapi.h"
#include <iostream>


int main() {
	
	SynoApi *api = new SynoApi();
	if(!api->is_opened()) {
		api->show_message(-1);
		return 0;
	}
	int ret = api->get_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}
	ret = api->upload_img();
	if(ret != PS_OK) {
		api->show_message(ret);
		return 0;
	}
	delete api;
    return 0;
}
