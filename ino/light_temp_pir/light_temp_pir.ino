#include "Arduino.h"
#include "Matter.h"
#include <app/server/OnboardingCodesUtil.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>
using namespace chip;
using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::endpoint;

/**
 * This program presents example Matter lite device with OnOff cluster by
 * controlling LED with Matter and toggle button.
 *
 * If your ESP does not have buildin LED, please connect it to LED_PIN
 *
 * You can toggle lite by both:
 *  - Matter (via CHIPTool or other Matter controller)
 *  - toggle button (by default attached to GPIO0 - reset button, with debouncing) 
 */

// Please configure your PINs
const int LED_PIN = 32;

// Cluster and attribute ID used by Matter lite device
const uint32_t LITE_CLUSTER_ID = OnOff::Id;
const uint32_t LITE_ATTRIBUTE_ID = OnOff::Attributes::OnOff::Id;
const uint32_t TEMP_CLUSTER_ID = TemperatureMeasurement::Id;
const uint32_t TEMP_ATTRIBUTE_ID = TemperatureMeasurement::Attributes::MeasuredValue::Id;
const uint32_t CONT_CLUSTER_ID = BooleanState::Id;
const uint32_t CONT_ATTRIBUTE_ID = BooleanState::Attributes::StateValue::Id;

// Endpoint and attribute ref that will be assigned to Matter device
uint16_t lite_endpoint_id = 0;
uint16_t temp_endpoint_id = 0;
uint16_t cont_endpoint_id = 0;
attribute_t *lite_attribute_ref;
attribute_t *temp_attribute_ref;
attribute_t *cont_attribute_ref;

// Global variables
int16_t last_millis = 0;
int16_t last_temp = 500;
boolean last_cont = false;
esp_matter_attr_val_t temp_value;
esp_matter_attr_val_t cont_value;

// There is possibility to listen for various device events, related for example to setup process
// Leaved as empty for simplicity
static void on_device_event(const ChipDeviceEvent *event, intptr_t arg) {}
static esp_err_t on_identification(identification::callback_type_t type, uint16_t endpoint_id,
                                   uint8_t effect_id, uint8_t effect_variant, void *priv_data) {
  return ESP_OK;
}

// Listener on attribute update requests.
// In this example, when update is requested, path (endpoint, cluster and attribute) is checked
// if it matches lite attribute. If yes, LED changes state to new one.
static esp_err_t on_attribute_update(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                     uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data) {
  if (endpoint_id == lite_endpoint_id) {
    if (type == attribute::PRE_UPDATE && cluster_id == LITE_CLUSTER_ID && attribute_id == LITE_ATTRIBUTE_ID) {
      // We got an lite on/off attribute update!
      bool new_state = val->val.b;
      digitalWrite(LED_PIN, new_state);
    }
  } else if (endpoint_id == temp_endpoint_id) {
    if (type == attribute::PRE_UPDATE && cluster_id == TEMP_CLUSTER_ID && attribute_id == TEMP_ATTRIBUTE_ID) {
    }
  }
  return ESP_OK;
}

void print_endpoint_info(String clusterName, endpoint_t *endpoint) {
  uint16_t endpoint_id = endpoint::get_id(endpoint);
  Serial.print("XXXXXX  ******** Antonio ******** ");
  Serial.print(clusterName);
  Serial.print(" is endpoint: ");
  Serial.println(endpoint_id);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Enable debug logging
  esp_log_level_set("*", ESP_LOG_DEBUG);

  // Setup Matter node
  node::config_t node_config;
  node_t *node = node::create(&node_config, on_attribute_update, on_identification);

  // Setup Light endpoint / cluster / attributes with default values
  on_off_light::config_t lite_config;
  lite_config.on_off.on_off = false;
  lite_config.on_off.lighting.start_up_on_off = false;
  endpoint_t *lite_endpoint = on_off_light::create(node, &lite_config, ENDPOINT_FLAG_NONE, NULL);
  lite_endpoint_id = endpoint::get_id(lite_endpoint);
  lite_attribute_ref = attribute::get(cluster::get(lite_endpoint, LITE_CLUSTER_ID), LITE_ATTRIBUTE_ID);
  print_endpoint_info("on_off_lite", lite_endpoint);

  // Setup Temperature Sensor
  temperature_sensor::config_t temp_config;
  temp_config.temperature_measurement.measured_value = 100;
  endpoint_t *temp_endpoint = temperature_sensor::create(node, &temp_config, ENDPOINT_FLAG_NONE, NULL);
  temp_endpoint_id = endpoint::get_id(temp_endpoint);
  temp_attribute_ref = attribute::get(cluster::get(temp_endpoint, TEMP_CLUSTER_ID), TEMP_ATTRIBUTE_ID);
  print_endpoint_info("temperature", temp_endpoint);

  // Setup Contact Sensor
  contact_sensor::config_t contact_sensor_config;
  contact_sensor_config.boolean_state.state_value = true;
  endpoint_t *cont_endpoint = contact_sensor::create(node, &contact_sensor_config, ENDPOINT_FLAG_NONE, NULL);
  cont_endpoint_id = endpoint::get_id(cont_endpoint);
  cont_attribute_ref = attribute::get(cluster::get(cont_endpoint, CONT_CLUSTER_ID), CONT_ATTRIBUTE_ID);
  print_endpoint_info("contact_sensor", cont_endpoint);

  // Setup DAC (this is good place to also set custom commission data, passcodes etc.)
  esp_matter::set_custom_dac_provider(chip::Credentials::Examples::GetExampleDACProvider());

  // Start Matter device
  esp_matter::start(on_device_event);

  // Print codes needed to setup Matter device
  PrintOnboardingCodes(chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE));
}

// When toggle lite button is pressed (with debouncing),
// lite attribute value is changed
void loop() {
////  if ((millis() - last_millis) > 10000) {
////    last_millis = millis();
  delay(30000);
  temp_value = esp_matter_int16(last_temp++);
  cont_value = esp_matter_bool(last_cont); last_cont = ! last_cont;
  attribute::update(temp_endpoint_id, TEMP_CLUSTER_ID, TEMP_ATTRIBUTE_ID, &temp_value);
  attribute::update(cont_endpoint_id, CONT_CLUSTER_ID, CONT_ATTRIBUTE_ID, &cont_value);
////  }
}
