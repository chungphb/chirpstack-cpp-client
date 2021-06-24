#include "test_config.h"
#include <chirpstack_client/chirpstack_client.h>
#include <iostream>

using namespace chirpstack_cpp_client;

struct test_cache {
    api::ServiceProfile service_profile;
    std::string device_profile_id;
    int64_t application_id;
    api::Device device;
    api::DeviceKeys device_keys;
    std::string device_address;
};

void get_service_profile(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    get_service_profile_request request;
    request.set_id(SERVICE_PROFILE_ID);

    // Send request
    auto response = client.get_service_profile(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to get service-profile: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.service_profile = response.get().service_profile();
}

void create_application(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    create_application_request request;
    request.mutable_application()->set_name(APPLICATION_NAME);
    request.mutable_application()->set_description(APPLICATION_NAME);
    request.mutable_application()->set_organization_id(cache.service_profile.organization_id());
    request.mutable_application()->set_service_profile_id(cache.service_profile.id());

    // Send request
    auto response = client.create_application(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to create application: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.application_id = response.get().id();
}

void delete_application(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    delete_application_request request;
    request.set_id(cache.application_id);

    // Send request
    auto response = client.delete_application(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to delete application: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void create_device_profile(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    create_device_profile_request request;
    request.mutable_device_profile()->set_name(DEVICE_PROFILE_NAME);
    request.mutable_device_profile()->set_organization_id(cache.service_profile.organization_id());
    request.mutable_device_profile()->set_network_server_id(cache.service_profile.network_server_id());
    request.mutable_device_profile()->set_mac_version("1.0.3");
    request.mutable_device_profile()->set_reg_params_revision("B");
    request.mutable_device_profile()->set_supports_join(true);

    // Send request
    auto response = client.create_device_profile(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to create device-profile: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.device_profile_id = response.get().id();
}

void delete_device_profile(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    delete_device_profile_request request;
    request.set_id(cache.device_profile_id);

    // Send request
    auto response = client.delete_device_profile(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to delete device-profile: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_create_device(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    create_device_request request;
    request.mutable_device()->set_dev_eui(DEVICE_EUI);
    request.mutable_device()->set_name(DEVICE_EUI);
    request.mutable_device()->set_description(DEVICE_EUI);
    request.mutable_device()->set_application_id(cache.application_id);
    request.mutable_device()->set_device_profile_id(cache.device_profile_id);

    // Send request
    auto response = client.create_device(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to create device: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_get_device(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    get_device_request request;
    request.set_dev_eui(DEVICE_EUI);

    // Send request
    auto response = client.get_device(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to get device: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.device = response.get().device();

    // Display response
    std::cout << "\tDevice " << cache.device.dev_eui() << std::endl;
    std::cout << "\t\tName: " << cache.device.name() << std::endl;
    std::cout << "\t\tDescription: " << cache.device.description() << std::endl;
    std::cout << "\t\tApplication ID: " << cache.device.application_id() << std::endl;
    std::cout << "\t\tDevice-profile ID: " << cache.device.device_profile_id() << std::endl;
}

void test_update_device(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    update_device_request request;
    *request.mutable_device() = cache.device;
    auto description = std::string(DEVICE_EUI);
    description += "-" + std::to_string(cache.device.application_id());
    description += "-" + cache.device.device_profile_id();
    request.mutable_device()->set_description(description);

    // Send request
    auto response = client.update_device(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to update device: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}
void test_list_device(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    list_device_request request;
    request.set_limit(10);
    request.set_application_id(cache.application_id);
    request.set_service_profile_id(cache.service_profile.id());

    // Send request
    auto response = client.list_device(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to list device: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Display response
    for (const auto& device : response.get().result()) {
        std::cout << "\tDevice " << device.dev_eui() << std::endl;
        std::cout << "\t\tName: " << device.name() << std::endl;
        std::cout << "\t\tDescription: " << device.description() << std::endl;
        std::cout << "\t\tApplication ID: " << device.application_id() << std::endl;
        std::cout << "\t\tDevice-profile ID: " << device.device_profile_id() << std::endl;
    }
}

void test_delete_device(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    delete_device_request request;
    request.set_dev_eui(DEVICE_EUI);

    // Send request
    auto response = client.delete_device(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to delete device: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_create_device_keys(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    create_device_keys_request request;
    request.mutable_device_keys()->set_dev_eui(DEVICE_EUI);
    request.mutable_device_keys()->set_nwk_key(NWK_KEY);
    request.mutable_device_keys()->set_app_key(APP_KEY);

    // Send request
    auto response = client.create_device_keys(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to create device-keys: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_get_device_keys(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    get_device_keys_request request;
    request.set_dev_eui(DEVICE_EUI);

    // Send request
    auto response = client.get_device_keys(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to get device-keys: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.device_keys = response.get().device_keys();

    // Display response
    std::cout << "\tDevice " << cache.device_keys.dev_eui() << "\'s keys" << std::endl;
    std::cout << "\t\tNetwork root key: " << cache.device_keys.nwk_key() << std::endl;
    std::cout << "\t\tApplication root key: " << cache.device_keys.app_key() << std::endl;
}

void test_update_device_keys(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    update_device_keys_request request;
    *request.mutable_device_keys() = cache.device_keys;
    auto app_key = "0f0e0d0c0b0a09080706050403020100";
    request.mutable_device_keys()->set_app_key(app_key);

    // Send request
    auto response = client.update_device_keys(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to update device-keys: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_delete_device_keys(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    delete_device_keys_request request;
    request.set_dev_eui(DEVICE_EUI);

    // Send request
    auto response = client.delete_device_keys(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to delete device-keys: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_get_random_dev_addr(chirpstack_client& client, test_cache& cache) {
    // Prepare request
    get_random_dev_addr_request request;
    request.set_dev_eui(DEVICE_EUI);

    // Send request
    auto response = client.get_random_dev_addr(request);
    if (!response.is_valid()) {
        std::cerr << "Failed to get random device address: " << response.error_code() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Save response
    cache.device_address = response.get().dev_addr();

    // Display response
    std::cout << "\tDevice " << cache.device.dev_eui() << "\'s address is " << cache.device_address << std::endl;
}

int main(int argc, char** argv) {
    chirpstack_client_config config{};
    config.jwt_token = JWT_TOKEN;
    chirpstack_client client{APPLICATION_SERVER, config};
    test_cache cache;

    std::cout << "GET SERVICE-PROFILE" << std::endl;
    get_service_profile(client, cache);

    std::cout << "CREATE DEVICE-PROFILE" << std::endl;
    create_device_profile(client, cache);

    std::cout << "CREATE APPLICATION" << std::endl;
    create_application(client, cache);

    std::cout << "TEST CREATE DEVICE" << std::endl;
    test_create_device(client, cache);

    std::cout << "TEST GET DEVICE" << std::endl;
    test_get_device(client, cache);

    std::cout << "TEST UPDATE DEVICE" << std::endl;
    test_update_device(client, cache);

    std::cout << "TEST LIST DEVICE" << std::endl;
    test_list_device(client, cache);

    std::cout << "TEST CREATE DEVICE-KEYS" << std::endl;
    test_create_device_keys(client, cache);

    std::cout << "TEST GET DEVICE-KEYS" << std::endl;
    test_get_device_keys(client, cache);

    std::cout << "TEST UPDATE DEVICE-KEYS" << std::endl;
    test_update_device_keys(client, cache);

    std::cout << "TEST DELETE DEVICE-KEYS" << std::endl;
    test_delete_device_keys(client, cache);

    std::cout << "TEST GET RANDOM DEVICE ADDRESS" << std::endl;
    test_get_random_dev_addr(client, cache);

    std::cout << "TEST DELETE DEVICE" << std::endl;
    test_delete_device(client, cache);

    std::cout << "DELETE APPLICATION" << std::endl;
    delete_application(client, cache);

    std::cout << "DELETE DEVICE-PROFILE" << std::endl;
    delete_device_profile(client, cache);

    return EXIT_SUCCESS;
}